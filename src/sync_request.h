#pragma once

#include "dto.h"
#include <mutex>
#include <condition_variable>

class SyncRequest {
    std::mutex mutex_{};
    std::condition_variable cv_{};
    bool response_received_ = false;
    std::string expected_key_{};
    std::string expected_request_{};
    Response response_ = Response::success("key", "action");

public:
    SyncRequest() = default;

    void setup(const std::string& client_key, const std::string& request_type) {
        std::lock_guard<std::mutex> lock(mutex_);
        expected_key_ = client_key;
        expected_request_ = request_type;
        response_received_ = false;
    }

    bool check_match(const Response& response) const {
        return response.key == expected_key_ &&
               response.request == expected_request_;
    }

    void notify(const Response& response) {
        std::lock_guard<std::mutex> lock(mutex_);
        response_ = response;
        response_received_ = true;
        cv_.notify_one();
    }

//    bool wait(Response& out_response, std::chrono::milliseconds timeout) {
//        std::unique_lock<std::mutex> lock(mutex_);
//        bool bRes = cv_.wait_for(lock, timeout, [this]() {
//            return response_received_;
//        });
//        if (bRes) out_response = response_;
//        return bRes;
//    }
    bool wait(Response& out_response, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto start = std::chrono::steady_clock::now();

        bool result = cv_.wait_for(lock, timeout, [this]() {
            return response_received_;
        });

        if (result) {
            out_response = response_;
        }

//        if (debug_mode_) {
//            auto end = std::chrono::steady_clock::now();
//            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//
//            std::cout << "[SYNC] Wait " << (result ? "OK" : "TIMEOUT")
//                      << " after " << elapsed.count() << "ms"
//                      << ", expected: " << expected_key_ << "/" << expected_request_
//                      << ", received: " << out_response.key << "/" << out_response.request << "\n";
//        }

        return result;
    }
};