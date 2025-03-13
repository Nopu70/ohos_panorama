//
// Created on 2024/12/23.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef PANORAMA_RENDERER_H
#define PANORAMA_RENDERER_H

namespace gles {

static void test()
{}

class Renderer
{
public:
	virtual void onInit() const = 0;
	virtual void onSizeChange(const int w, const int h) const = 0;
	virtual void onDrawFrame() const = 0;
    virtual ~Renderer()
    {
        
    }
};

}

#endif //PANORAMA_RENDERER_H
