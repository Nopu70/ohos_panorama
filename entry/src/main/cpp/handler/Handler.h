//
// Created on 2024/12/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef PANORAMA_HANDLER_H
#define PANORAMA_HANDLER_H

#include "Looper.h"
#include "handler/Message.h"
#include <mutex>

namespace ITC {

    class Handler {
        UVHandler handler;
        bool isQueue1Active;
        std::mutex mMutex;
        std::vector<Message> queue1;
        std::vector<Message> queue2;
    
        std::vector<Message>& getCurrentQueue();
        std::vector<Message>& changeCurrentQueue();
    public:
    
        Handler(const UVLooper& looper = UVLooper());
        virtual ~Handler();
        Handler(const Handler& handler) = delete;
        Handler& operator=(const Handler& handler) = delete;
    
        friend void async_cb(UVHandler uVHandler);
    
        void sendMessage(Message& msg);
        void sendMessage(Message&& msg);
        template<typename... Args>
        void sendMessage(Args&& ...args)
        {   
            std::lock_guard<std::mutex> lock(mMutex);
            getCurrentQueue().emplace_back(std::forward<Args>(args)...);
            uv_async_send(handler);
        }
        virtual void handleMessage(const Message& msg) const {};
    };

}


#endif //PANORAMA_HANDLER_H
