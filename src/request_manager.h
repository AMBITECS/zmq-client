#pragma once

#include "sync_request.h"

class RequestManager {
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<SyncRequest>> requests_;
    //std::vector<std::shared_ptr<SyncRequest>> pending_requests_;

public:
    std::shared_ptr<SyncRequest> create(const std::string& key, const std::string& request) {
        auto req = std::make_shared<SyncRequest>();
        std::lock_guard<std::mutex> lock(mutex_);
        requests_[key + ":" + request] = req;
        return req;
    }

    bool process_response(const Response& response) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto key = response.key + ":" + response.request;
        if (auto it = requests_.find(key); it != requests_.end()) {
            it->second->set_response(response);
            requests_.erase(it);
            return true;
        }
        return false;
    }

//    std::shared_ptr<SyncRequest> create(const std::string& client_key,
//                                        const std::string& request_type) {
//        auto req = std::make_shared<SyncRequest>();
//        req->setup(client_key, request_type);
//
//        std::lock_guard<std::mutex> lock(mutex_);
//        pending_requests_.push_back(req);
//        return req;
//    }
//
//    bool process_response(const Response& response) {
//        std::lock_guard<std::mutex> lock(mutex_);
//        for (auto it = pending_requests_.begin(); it != pending_requests_.end(); ) {
//            if ((*it)->check_match(response)) {
//                (*it)->notify(response);
//                it = pending_requests_.erase(it);
//                return true;
//            } else {
//                ++it;
//            }
//        }
//        return false;
//    }
//
//    void cleanup() {
//        std::lock_guard<std::mutex> lock(mutex_);
//        pending_requests_.clear();
//    }
};
