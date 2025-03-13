//
// Created on 2024/12/23.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "Context.h"
#include <GLES3/gl3.h>
#include <hilog/log.h>

void gles::Context::init(const EGLNativeWindowType window, int w, int h)
{
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY)
    {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_CONTEXT__", "eglGetDisplay failed");
        return;
    }
    
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_CONTEXT__", "eglInitialize failed");
        return;
    }
    
    EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, // OpenGL ES 3.0
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs) || numConfigs == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_CONTEXT__", "Failed to choose EGL config");
        return;
    }
    
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    if (context == EGL_NO_CONTEXT) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_CONTEXT__", "Failed to create EGL context");
        return;
    }
    
    surface = eglCreateWindowSurface(display, config, window, NULL);
    if (surface == EGL_NO_SURFACE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_CONTEXT__", "Failed to create EGL window surface");
        return;
    }
    
    if (!eglMakeCurrent(display, surface, surface, context)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_CONTEXT__", "Failed to make EGL context curren");
        return;
    }
    
    ITC::Looper::prepare();
    
    renderer->onInit();
    renderer->onSizeChange(w, h);
    
    ITC::Looper::run([&](){
        renderer->onDrawFrame();
        eglSwapBuffers(display, surface);
    });
    
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);
}

gles::Context::Context(const EGLNativeWindowType window, const Renderer* renderer, int w, int h)
    :renderer(renderer),
    renderThread(std::thread(&gles::Context::init, this, window, w, h))
{
}

gles::Context::~Context()
{
    delete renderer;
    if (renderThread.joinable())
    {
        renderThread.join();
    }
}
