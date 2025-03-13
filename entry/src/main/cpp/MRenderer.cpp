//
// Created on 2024/12/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "MRenderer.h"
#include <stb_img/stb_image.h>
#include <gles/Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <global.h>
#include <hilog/log.h>

static const char* vs = R"(
#version 310 es

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view *  model * vec4(position, 1.0f);
    TexCoord = aTexCoord;
}
)";
static const char* fs = R"(
#version 310 es
precision mediump float;

in vec2 TexCoord;
uniform sampler2D texture1;
layout(location = 0) out vec4 color;

void main()
{
	color = texture(texture1, TexCoord);
}
)";


static auto startTime = std::chrono::high_resolution_clock::now();
static constexpr float PI = 3.14159265358979323846f;
static const int radius = 1; //半径
static const int latCount = 50; //纬度网格数量(比纬度线少1)
static const int lonCount = 50; //经度网格数量(和经度线数量相同)
static int indices1_size;

static gles::Shader* shader = nullptr;
GLint modelLoc, viewLoc, projectionLoc; 

MRenderer::MRenderer()
    :handler(nullptr),
    fovy(PI / 4)
{
}

void MRenderer::onInit() const
{
    class MHandler : public ITC::Handler {
    public:
        void handleMessage(const ITC::Message& msg) const override {
            
        }
    };
    handler = new MHandler();
    
	glEnable(GL_DEPTH_TEST);
    
	shader = new gles::Shader(vs, fs);
    std::vector<float> vertices;
    vertices.reserve((latCount + 1) * (lonCount + 1) * 5);
    
    std::vector<int> indices1;
    indices1_size = (lonCount + 1) * 2 * latCount;
    indices1.reserve(indices1_size);
    
    for (int i = 0; i <= latCount; i++)
    {
        //自定义纬度：（北极->南极）定义为 （0 -> PI）
        float lat = PI * i / latCount;
        float y = radius * cos(lat);
        //纬度圈的半径
        float latRadius = radius * sin(lat);
    
        for (int j = 0; j <= lonCount; j++)
        {
            //自定义经度：（初始方向为X轴正方向围绕y轴旋转）定义为 （0 -> 2 * PI）
            float lon = PI * 2 * j / lonCount;
            float x = cos(lon) * latRadius;
            float z = sin(lon) * latRadius;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
    
            //按照上面的自定义精度和纬度逻辑计算的顶点，使用下面这行映射横向坐标时
            // 在球体外观察图片左右是颠倒的，球体内是正常的，可以用来渲染全景图
            vertices.push_back((float)j / lonCount);
            //下一行代码设置纹理坐标时没有按照opengl UV坐标系(左下角0,0 右上角(1,1))映射
            // 如果按照UV坐标系映射，图像会上下颠倒，那就需要在加载图片资源时颠倒图片
//             vertices.push_back((float)i / latCount);
    
            //按照上面的自定义精度和纬度逻辑计算的顶点，使用下面这行映射横向坐标时
            // 在球体外观察图片左右是正常的，球体内是颠倒的，可以用来渲染地球仪
//             vertices.push_back((float)(lonCount - j) / lonCount);
            //下面这行代码按照opengl UV坐标系(左下角0,0 右上角(1,1))映射纹理坐标
            //图片(地球柱状投影图)加载时做了颠倒处理
            vertices.push_back((float)(latCount - i) / latCount);
        }
    }
    
    for (int i = 0; i < latCount; i++)
    {
        const int currentLatIndex = i * (lonCount + 1);
        const int nextLatIndex = (i + 1) * (lonCount + 1);
        for (int j = 0; j <= lonCount; j++)
        {
            indices1.push_back(currentLatIndex + j);
            indices1.push_back(nextLatIndex + j);
        }
    }
    
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
    
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices1.size(), &indices1[0], GL_STATIC_DRAW);
    
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
    
    RawFile *rawFile = OH_ResourceManager_OpenRawFile(mNativeResMgr, "fjqj1.jpg");
    if (rawFile != nullptr) {
        long len = OH_ResourceManager_GetRawFileSize(rawFile); 
        std::unique_ptr<uint8_t[]> rawfileData= std::make_unique<uint8_t[]>(len);
        OH_ResourceManager_ReadRawFile(rawFile, rawfileData.get(), len);
        OH_ResourceManager_CloseRawFile(rawFile);
        
	    int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load_from_memory(rawfileData.get(), len, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__MRender__", "Failed to load texture1: %{public}s", stbi_failure_reason()); 
        }
        stbi_image_free(data);
    } 
    

	shader->use();
    modelLoc = shader->getLocation("model");
    viewLoc = shader->getLocation("view");
    projectionLoc = shader->getLocation("projection");
    
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    
    auto view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__MRender__", "init complete"); 
}

void MRenderer::onSizeChange(const int w, const int h) const
{
    if (handler) {
        handler->sendMessage(0xff, [=](){
            width = w;
            height = h;
            glViewport(0, 0, width, height);
            auto projection = glm::perspective(fovy, (float)width / (float)height, 0.1f, 100.0f);
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        }, false);
    }
}

void MRenderer::onDrawFrame() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLE_STRIP, indices1_size, GL_UNSIGNED_INT, nullptr);
}

void MRenderer::onEulerAngleChange(const float yaw, const float pitch)
{
    if (handler) {
        handler->sendMessage(0xff, [=](){
            //假设相机初始观察点为 (1.0f, 0.0f, 0.0f)
            float y = sin(pitch);
            float cos_pitch = cos(pitch);
            float x = cos_pitch * cos(yaw);
            float z = cos_pitch * sin(yaw);
            auto view = glm::lookAt(
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(x, y, z),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        }, true);
    }
}

void MRenderer::onScaleChange(const float f)
{
    if (handler) {
        handler->sendMessage(0xff, [=](){
            fovy = f;
            auto projection = glm::perspective(fovy, (float)width / (float)height, 0.1f, 100.0f);
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        }, true);
    }
}

MRenderer::~MRenderer()
{
	delete shader;
    delete handler;
}
