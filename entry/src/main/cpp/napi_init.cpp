#include "handler/Handler.h"
#include "napi/native_api.h"
#include <cstdint>

#include <native_window/external_window.h>
#include <hilog/log.h>
#include "gles/Context.h"
#include "MRenderer.h"
#include "global.h"

static gles::Context* mContext = nullptr;
static MRenderer* mRenderer = nullptr;

static OHNativeWindow *nativeWindow;
NativeResourceManager *mNativeResMgr = nullptr;

static napi_value create(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {nullptr};

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    napi_valuetype valuetype1;
    if (napi_typeof(env, args[1], &valuetype1) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_INIT__", "Node-API napi_typeof fail");
        return nullptr;
    }
    
    mNativeResMgr = OH_ResourceManager_InitNativeResourceManager(env, args[1]);
    return nullptr;
}

static napi_value changeSize(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value args[3] = {nullptr};

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    napi_valuetype valuetype0;
    if (napi_typeof(env, args[0], &valuetype0) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_INIT__", "Node-API napi_typeof fail");
        return nullptr;
    }

    napi_valuetype valuetype1;
    if (napi_typeof(env, args[1], &valuetype1) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_INIT__", "Node-API napi_typeof fail");
        return nullptr;
    }

    napi_valuetype valuetype2;
    if (napi_typeof(env, args[2], &valuetype2) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_INIT__", "Node-API napi_typeof fail");
        return nullptr;
    }
    
    
    int64_t surfaceId;
    bool lossless = true;
    if (napi_ok != napi_get_value_bigint_int64(env, args[0], &surfaceId, &lossless)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xff, "__GLES_INIT__", "ParseId Get value failed");
        return nullptr;
    }
    
    OH_NativeWindow_CreateNativeWindowFromSurfaceId(surfaceId, &nativeWindow);

    int width;
    napi_get_value_int32(env, args[1], &width);

    int height;
    napi_get_value_int32(env, args[2], &height);
    
    if (mRenderer == nullptr)
    {
        mRenderer = new MRenderer();
        mContext = new gles::Context(reinterpret_cast<EGLNativeWindowType>(nativeWindow), mRenderer, width, height);
    } else {
        mRenderer->onSizeChange(width, height);
    }
    
    
    return nullptr;

}

static napi_value onEulerAngleChange(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {nullptr};

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);
    
    double yaw;
    napi_get_value_double(env, args[0], &yaw);
    double pitch;
    napi_get_value_double(env, args[1], &pitch);
    
    if (mRenderer) {
//         OH_LOG_Print(LOG_APP, LOG_INFO, 0xff, "__INIT__eac", "yaw:%{public}f, pitch:%{public}f", yaw, pitch);
        mRenderer->onEulerAngleChange(yaw, pitch);
    }
    
    return nullptr;
}

static napi_value destory(napi_env env, napi_callback_info info)
{
//     delete mRenderer;
    delete mContext;
    
    OH_ResourceManager_ReleaseNativeResourceManager(mNativeResMgr);
    OH_NativeWindow_DestroyNativeWindow(nativeWindow);
    
    return nullptr;

}

static napi_value onScaleChange(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);
    
    double fovy;
    napi_get_value_double(env, args[0], &fovy);
    
    if (mRenderer) {
        mRenderer->onScaleChange(fovy);
    }
    
    return nullptr;
}
EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"onSurfaceCreated", nullptr, create, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"onSurfaceChanged", nullptr, changeSize, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"onSurfaceDestroyed", nullptr, destory, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"onEulerAngleChange", nullptr, onEulerAngleChange, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"onScaleChange", nullptr, onScaleChange, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}
