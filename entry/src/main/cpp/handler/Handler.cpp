//
// Created on 2024/12/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "Handler.h"
#include <hilog/log.h>

static void handleMessage(const ITC::Handler& handle, const ITC::Message& msg)
{
    if (msg.callback) {
        msg.callback();
    } else {
        handle.handleMessage(msg);
    }
}

void ITC::async_cb(UVHandler uvHandler)
{
    ITC::Handler* handle = static_cast<ITC::Handler*>(uv_handle_get_data((uv_handle_t*)uvHandler));
    
    std::vector<ITC::Message>& queue = handle->changeCurrentQueue();
    
    std::unordered_map<int, int> coalesceMap;
    for (int i = 0; i < queue.size(); i++) {
        auto& msg = (queue)[i];
        if (msg.coalesce){
            //将不重要的消息合并，放到后面执行
            coalesceMap[msg.what] = i;
        } else {
            handleMessage(*handle, msg);
        }
    }
    for (const auto& [what, index] : coalesceMap) {
        handleMessage(*handle, queue[index]);
    }
    coalesceMap.clear();
    queue.clear();
}

ITC::Handler::Handler(const UVLooper& looper)
    :handler(new uv_async_t),
    mMutex(),
    queue1(),
    queue2(),
    isQueue1Active(true)
{
    if (looper) {
        uv_async_init(looper.get(), handler, async_cb);
    } else if (auto& looper = Looper::getLooper()) {
        uv_async_init(looper.get(), handler, async_cb);
    } else {
        throw std::runtime_error("The looper is Invalid");
    }
    
    queue1.reserve(20);
    queue2.reserve(20);
    uv_handle_set_data((uv_handle_t*)handler, this);
}

ITC::Handler::~Handler()
{
    uv_close((uv_handle_t*)handler, [](uv_handle_t* handle){
        delete handle;
    });
}


void ITC::Handler::sendMessage(Message& msg)
{
    std::lock_guard<std::mutex> lock(mMutex);
    getCurrentQueue().push_back(msg);
    uv_async_send(handler);
}
void ITC::Handler::sendMessage(Message&& msg)
{
    std::lock_guard<std::mutex> lock(mMutex);
    getCurrentQueue().push_back(std::move(msg));
    uv_async_send(handler);
}

std::vector<ITC::Message>& ITC::Handler::getCurrentQueue()
{
    if (isQueue1Active) {
        return queue1;
    } else {
        return queue2;
    }
}
std::vector<ITC::Message>& ITC::Handler::changeCurrentQueue()
{
    std::lock_guard<std::mutex> lock(mMutex);
    isQueue1Active = !isQueue1Active;
    if (isQueue1Active) {
        return queue2;
    } else {
        return queue1;
    }
}