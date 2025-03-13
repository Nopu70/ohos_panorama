//
// Created on 2024/12/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "Looper.h"

static thread_local UVLooper looper;

const UVLooper& ITC::Looper::getLooper()
{
    return looper;
}

void ITC::Looper::prepare()
{
    if (looper)
    {
        throw new std::runtime_error("Looper has been prepared");
    }
    looper = std::make_unique<uv_loop_t>();
    uv_loop_init(looper.get());
}

void ITC::Looper::loop()
{
    if (!looper)
    {
        throw new std::runtime_error("Looper is not prepared");
    }
    uv_run(looper.get(), UV_RUN_DEFAULT);
    uv_loop_delete(looper.release());
}

void ITC::Looper::run(std::function<void()> loop_task)
{
    if (!looper)
    {
        throw new std::runtime_error("Looper is not prepared");
    }
    while (uv_run(looper.get(), UV_RUN_NOWAIT) != 0)
    {
        loop_task();
    }
    uv_loop_delete(looper.release());
}