//
// Created on 2024/12/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "Message.h"


ITC::Message::Message(int what, bool coalesce, std::any param)
    :what(what),
    coalesce(coalesce),
    params(param),
    callback()
{
    if (what <= 0)
    {
        throw std::runtime_error("what must not less 0");
    }
}

ITC::Message::Message(int what, std::function<void()> callback, bool coalesce)
    :what(what),
    coalesce(coalesce),
    params(),
    callback(callback)
{
}