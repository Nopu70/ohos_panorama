//
// Created on 2024/12/23.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef PANORAMA_CONTEXT_H
#define PANORAMA_CONTEXT_H

#include <EGL/egl.h>
#include <thread>
#include <mutex>
#include "Renderer.h"
#include <handler/Looper.h>

namespace gles {

static std::mutex m_mutex;

class Context 
{
public:
    Context(const Context& instance) = delete;
    Context& operator=(const Context& instance) = delete;
    Context& operator=(Context&& instance) = delete;
    
    Context(const EGLNativeWindowType window, const Renderer* renderer, int w, int h);
    ~Context();
private:
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    const Renderer* renderer;
	std::thread renderThread;
    
    void init(const EGLNativeWindowType window, int w, int h);
};

}


#endif //PANORAMA_CONTEXT_H
