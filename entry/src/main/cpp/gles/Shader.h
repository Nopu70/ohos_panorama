//
// Created on 2024/12/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef PANORAMA_SHADER_H
#define PANORAMA_SHADER_H


#include <GLES3/gl32.h>
#include <string>

namespace gles 
{

class Shader
{
    const GLuint program;
public:
	Shader(const char* vs, const char* fs);
    void use() const;
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    GLint getLocation(const std::string& name) const;
};

}

#endif //PANORAMA_SHADER_H
