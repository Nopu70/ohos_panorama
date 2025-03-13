//
// Created on 2024/12/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "Shader.h"
#include <hilog/log.h>

static GLuint CompileShader(GLenum type, const char* shader)
{
	GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &shader, nullptr);
	glCompileShader(id);

	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* msg = (char*)alloca(length);
		glGetShaderInfoLog(id, length, &length, msg);
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_Shader__", "Failed compile: %{public}s :%{public}s",(type == GL_VERTEX_SHADER ? "vertex" : "fragment"), msg);
		glDeleteShader(id);
		return 0;
	}
	return id;
}

GLuint CreateShader(const char* vss, const char* fss)
{
	GLuint program = glCreateProgram();
	GLuint vs = CompileShader(GL_VERTEX_SHADER, vss);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fss);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

gles::Shader::Shader(const char* vs, const char* fs)
	:program(CreateShader(vs, fs))
{}

void gles::Shader::use() const
{
	glUseProgram(program);
}

void gles::Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void gles::Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void gles::Shader::setFloat(const std::string& name, float value) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

GLint gles::Shader::getLocation(const std::string& name) const
{
	return glGetUniformLocation(program, name.c_str());
}
