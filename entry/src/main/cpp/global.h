//
// Created on 2024/12/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef PANORAMA_GLOBAL_H
#define PANORAMA_GLOBAL_H

#include <rawfile/raw_file.h> 
#include <rawfile/raw_file_manager.h> 

extern NativeResourceManager *mNativeResMgr;

#endif //PANORAMA_GLOBAL_H
