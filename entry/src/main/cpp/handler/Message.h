//
// Created on 2024/12/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef PANORAMA_MESSAGE_H
#define PANORAMA_MESSAGE_H

#include <any>
#include <functional>

namespace ITC {

    class Message {
    public:
        int what;
        bool coalesce;
        std::any params;
        std::function<void()> callback;
        Message() = delete;
        Message(int what, bool coalesce = false, std::any param = std::any());
        Message(int what, std::function<void()> callback, bool coalesce = false);
    };

}



#endif //PANORAMA_MESSAGE_H
