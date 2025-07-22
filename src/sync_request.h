#pragma once

#include "dto.h"
#include <mutex>
#include <condition_variable>

class SyncRequest {
    std::mutex mutex_{};
    std::condition_variable cv_{};

//    bool response_received_ = false;
//    std::string expected_key_{};
//    std::string expected_request_{};

    Response response_ = Response::success("key", "action");
    bool ready_ = false;

public:
    SyncRequest() = default;

    void set_response(const Response& response) {
        std::lock_guard<std::mutex> lock(mutex_);
        response_ = response;
        ready_ = true;
        cv_.notify_one();
    }

    bool wait(Response& out, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, timeout, [this] { return ready_; })
               && (out = response_, true);
    }


//    void setup(const std::string& client_key, const std::string& request_type) {
//        std::lock_guard<std::mutex> lock(mutex_);
//        expected_key_ = client_key;
//        expected_request_ = request_type;
//        response_received_ = false;
//    }

//    bool check_match(const Response& response) const {
//        return response.key == expected_key_ &&
//               response.request == expected_request_;
//    }

//    void notify(const Response& response) {
//        std::lock_guard<std::mutex> lock(mutex_);
//        response_ = response;
//        response_received_ = true;
//        cv_.notify_one();
//    }

//    bool wait(Response& out_response, std::chrono::milliseconds timeout) {
//        std::unique_lock<std::mutex> lock(mutex_);
//        auto start = std::chrono::steady_clock::now();
//
//        bool result = cv_.wait_for(lock, timeout, [this]() {
//            return response_received_;
//        });
//
//        if (result) {
//            out_response = response_;
//        }
//
//        return result;
//    }
};