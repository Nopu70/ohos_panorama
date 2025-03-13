# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(Panorama)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(entry SHARED
    napi_init.cpp
    MRenderer.cpp
    stb_img/stb_image.cpp
    gles/Context.cpp
    gles/Shader.cpp
    handler/Handler.cpp
    handler/Looper.cpp
    handler/Message.cpp)
target_link_libraries(entry PUBLIC
    libace_napi.z.so
    libhilog_ndk.z.so
    libEGL.so
    libGLESv3.so
    libuv.so
    libace_ndk.z.so
    libnative_window.so
    librawfile.z.so)