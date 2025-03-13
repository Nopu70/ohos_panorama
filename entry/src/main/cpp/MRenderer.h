//
// Created on 2024/12/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef PANORAMA_MRENDERER_H
#define PANORAMA_MRENDERER_H

#include <gles/Renderer.h>
#include <handler/Handler.h>

class MRenderer : public gles::Renderer {
    mutable ITC::Handler* handler;
    mutable int width, height;
    mutable float fovy;
public:
    MRenderer();
    void onInit() const override;
    void onSizeChange(const int w, const int h) const override;
    void onDrawFrame() const override;
    ~MRenderer();
    
    void onEulerAngleChange(const float yaw, const float pitch);
    void onScaleChange(const float fovy);
};

#endif //PANORAMA_MRENDERER_H
