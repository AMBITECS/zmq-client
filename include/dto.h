//-----------------------------------------------------------------------------
// Copyright © 2016-2025 AMBITECS <info@ambi.biz>
//-----------------------------------------------------------------------------
#pragma once

//#include "variant.h"
#include <string>
#include <vector>
#include <utility>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using sysclk = std::chrono::system_clock;

// Качество (достоверность значения)
// ----------------------------------------------------------------------------
enum class Quality : int {
    GOOD      = 0,
    BAD       = 1,
    UNCERTAIN = 2
};

inline const char* toString(Quality quality) {
    switch (quality) {
        case Quality::GOOD:      return "GOOD";
        case Quality::BAD:       return "BAD";
        case Quality::UNCERTAIN: return "UNCERTAIN";
        default:                 return "UNKNOWN";
    }
}

inline Quality fromInt(int value) {
    switch (value) {
        case 0:  return Quality::GOOD;
        case 1:  return Quality::BAD;
        case 2:  return Quality::UNCERTAIN;
        default: return Quality::BAD;
    }
}

// Структура тега (для передачи значений)
// ----------------------------------------------------------------------------
struct Tag {
    std::string         key{"%ID0"};

    //VARIANT             value = VARIANT(0);
    uint64_t            value = 0; //***

    Quality             quality{Quality::GOOD};
    sysclk::time_point  timestamp = sysclk::now();

    [[nodiscard]] json toJSON() const {
        json j;
        j["key"]        = key;

        //j["value"]      = value.toString();
        j["value"]      = value;  //*** uint64_t напрямую (библиотека json поддерживает)

        j["quality"]    = static_cast<int>(quality);
        j["timestamp"]  = std::chrono::duration_cast<std::chrono::milliseconds>(
                timestamp.time_since_epoch()).count();
        return j;
    }

    static Tag fromJSON(const json& j) {
        Tag t;
        t.key = j["key"].get<std::string>();

        //t.value = VARIANT::of(j["value"].get<std::string>());
        t.value = j["value"].get<uint64_t>();  //*** Получаем напрямую uint64_t

        t.quality = fromInt(j["quality"].get<int>());
        t.timestamp = sysclk::time_point(
                std::chrono::milliseconds(j["timestamp"].get<int64_t>()));
        return t;
    }
};

// IDto interface (базовый класс всех сообщений)
// ----------------------------------------------------------------------------
class IDto
{
public:
    virtual ~IDto() = default;
    [[nodiscard]] virtual std::string getKey() const = 0;
    [[nodiscard]] virtual std::string toJSON() const = 0;

    template <typename T>
    static T fromJSON(const std::string& jsonStr) {
        static_assert(std::is_base_of_v<IDto, T>, "T must inherit from IDto");
        return T::fromJSON(jsonStr);
    }
};

// Request basic class (стандартный запрос)
// ----------------------------------------------------------------------------
struct Request : public IDto
{
    std::string key;
    std::string request;
    Request(std::string k, std::string r): key(std::move(k)), request(std::move(r)) {}

    [[nodiscard]] std::string getKey() const override { return key; }
    [[nodiscard]] std::string toJSON() const override {
        json j;
        j["key"] = key;
        j["request"] = request;
        return j.dump();
    }

    static Request fromJSON(const std::string& jsonStr) {
        auto j = json::parse(jsonStr);
        return Request{
            j["key"].get<std::string>(),
            j["request"].get<std::string>()
        };
    }
};

// Response basic class (стандартный ответ)
// ----------------------------------------------------------------------------
struct Response : public IDto
{
    static constexpr int SUCCESS = 200;
    static constexpr int BAD_REQUEST = 400;
    static constexpr int NOT_JSON = 415;
    static constexpr int INTERNAL_ERROR = 500;
    static constexpr int REQUEST_HANDLE_ERROR = 502;

    std::string key = "unknown";
    std::string request = "unknown";
    int         result = SUCCESS;
    std::string message = "unknown";

    Response() = default;
    Response(std::string key_, std::string req, int res, std::string m) :
            key(std::move(key_)),
            request(std::move(req)),
            result(res),
            message(std::move(m)) {}

    [[nodiscard]] std::string getKey() const override { return key; }
    [[nodiscard]] std::string toJSON() const override {
        json j;
        j["key"]     = key;
        j["request"] = request;
        j["result"]  = result;
        j["message"] = message;
        return j.dump();
    }

    static Response fromJSON(const std::string& jsonStr) {
        auto j = json::parse(jsonStr);
        return Response{
                j["key"].get<std::string>(),
                j["request"].get<std::string>(),
                j["result"].get<int>(),
                j["message"].get<std::string>()
        };
    }

    [[nodiscard]] bool isSuccess() const { return result >= 200 && result < 300; }

    // Фабричные методы
    static Response success(std::string key, std::string req, std::string message = "Ok") {
        return {std::move(key), std::move(req), SUCCESS, std::move(message)};
    }

    static Response error(std::string key, std::string req, std::string message = "Server error") {
        return {std::move(key), std::move(req), INTERNAL_ERROR, std::move(message)};
    }
};

// Subscribtion Management
// ----------------------------------------------------------------------------
// Пример использования:
//  auto sub = Subscribe::create(
//        "client123",
//        "temperature_sensors",
//        {"%ID100", "%ID101", "%ID102"} );
//
// Сериализация в JSON:
//  std::string json = sub->toJSON();

// ----------------------------------------------------------------------------
struct Subscribe : public Request
{
    // Дополнительные поля
    std::string              topic;
    std::vector<std::string> keys;

    Subscribe(
            std::string clientKey,      // client key
            std::string topicStr,       // topic
            std::vector<std::string> v  // tag keys vector
            ) :
            Request(std::move(clientKey), "subscribe_values"),
            topic(std::move(topicStr)),
            keys(std::move(v)) {}

    [[nodiscard]] std::string toJSON() const override {
        json j;
        j["key"]     = key;
        j["request"] = request;
        j["topic"]   = topic;
        j["keys"]    = keys;
        return j.dump();
    }

    static Subscribe fromJSON(const std::string& jsonStr) {
        auto j = json::parse(jsonStr);
        return Subscribe{
                j["key"].get<std::string>(),
                j["topic"].get<std::string>(),
                j["keys"].get<std::vector<std::string>>()
        };
    }
};

// ----------------------------------------------------------------------------
struct Unsubscribe : public Request {
    // Дополнительные поля
    std::string topic;

    Unsubscribe(std::string clientKey, std::string t) :
            Request(std::move(clientKey), "unsubscribe_values"),
            topic(std::move(t)) {}

    [[nodiscard]] std::string toJSON() const override {
        json j;
        j["key"]     = key;
        j["request"] = request;
        j["topic"]   = topic;
        return j.dump();
    }

    static Unsubscribe fromJSON(const std::string& jsonStr) {
        auto j = json::parse(jsonStr);
        return Unsubscribe{
                j["key"].get<std::string>(),
                j["topic"].get<std::string>()
        };
    }
};

// SendValues (отправка изменений)
// ----------------------------------------------------------------------------
struct SendValues : public IDto {
    std::string      key;
    std::string      topic;
    std::vector<Tag> values;

    SendValues(std::string k, std::string t, std::vector<Tag> v) :
            key(std::move(k)),
            topic(std::move(t)),
            values(std::move(v)) {}

    [[nodiscard]] std::string getKey() const override { return key; }
    [[nodiscard]] std::string toJSON() const override {
        json j;
        j["key"] = key;
        j["topic"] = topic;

        json valuesArray = json::array();
        for (const auto& tag : values) {
            valuesArray.push_back(tag.toJSON());
        }
        j["values"] = valuesArray;

        return j.dump();
    }

    static SendValues fromJSON(const std::string& jsonStr) {
        auto j = json::parse(jsonStr);

        std::vector<Tag> values;
        for (const auto& item : j["values"]) {
            values.push_back(Tag::fromJSON(item));
        }

        return SendValues{
                j["key"].get<std::string>(),
                j["topic"].get<std::string>(),
                std::move(values)
        };
    }
};

// File transfer Management
// ----------------------------------------------------------------------------
struct ProgStart : public Request {
    std::string prog_name;
    uint64_t prog_hash;

    ProgStart(std::string clientKey, std::string name, uint64_t hash):
            Request(std::move(clientKey), "prog_start"),
            prog_name(std::move(name)),
            prog_hash(hash) {}

    [[nodiscard]] std::string toJSON() const override {
        json j;
        j["key"]       = key;
        j["request"]   = request;
        j["prog_name"] = prog_name;
        j["prog_hash"] = prog_hash;
        return j.dump();
    }

    static ProgStart fromJSON(const std::string& jsonStr) {
        auto j = json::parse(jsonStr);
        return ProgStart{
                j["key"].get<std::string>(),
                j["prog_name"].get<std::string>(),
                j["prog_hash"].get<uint64_t>()
        };
    }
};

struct FileStart : public Request {
    std::string file_name;
    uint64_t    file_size;

    FileStart(std::string clientKey, std::string name, uint64_t size):
            Request(std::move(clientKey), "file_start"),
            file_name(std::move(name)),
            file_size(size) {}

    [[nodiscard]] std::string toJSON() const override {
        json j;
        j["key"]        = key;
        j["request"]    = request;
        j["file_name"]  = file_name;
        j["file_size"]  = file_size;
        return j.dump();
    }

    static FileStart fromJSON(const std::string& jsonStr) {
        auto j = json::parse(jsonStr);
        return FileStart{
                j["key"].get<std::string>(),
                j["file_name"].get<std::string>(),
                j["file_size"].get<uint64_t>()
        };
    }
};

struct FileChunk : public Request {
    std::string chunk_data;
    uint64_t    chunk_size;

    FileChunk(std::string clientKey, std::string data, uint64_t size):
            Request(std::move(clientKey), "file_chunk"),
            chunk_data(std::move(data)),
            chunk_size(size) {}

    [[nodiscard]] std::string toJSON() const override {
        json j;
        j["key"]        = key;
        j["request"]    = request;
        j["chunk_data"] = chunk_data;
        j["chunk_size"] = chunk_size;
        return j.dump();
    }

    static FileChunk fromJSON(const std::string& jsonStr) {
        auto j = json::parse(jsonStr);
        return FileChunk{
                j["key"].get<std::string>(),
                j["chunk_data"].get<std::string>(),
                j["chunk_size"].get<uint64_t>()
        };
    }
};

using FileEnd = Request;
using ProgEnd = Request;

// ----------------------------------------------------------------------------
