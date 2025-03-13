//
// Created on 2024/12/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef PANORAMA_LOOPER_H
#define PANORAMA_LOOPER_H
#include <functional>
#include <uv.h>

typedef std::unique_ptr<uv_loop_t> UVLooper;
typedef uv_async_t* UVHandler;
 
namespace ITC {
    
    namespace Looper {

        const UVLooper& getLooper();
        
        void prepare();
        
        void loop();
        
        void run(std::function<void()> loop_task);
    
    }

    
    template<typename Req>
    void runOnWork(std::function<Req()> reqF, std::function<void(Req)> respF)
    {
        if (!Looper::getLooper())
        {
//             throw new std::runtime_error("Looper is not prepared");
            respF(reqF());
            return;
        }
        
        struct WorkData {
            Req req;
            std::function<Req()> reqF;
            std::function<void(Req)> respF;
        };
    
        uv_work_t* work = new uv_work_t;
        WorkData* data = {Req(), reqF, respF};
        uv_req_set_data((uv_req_t*)work, data);
    
        int ret = uv_queue_work(Looper::getLooper().get(), work, [](uv_work_t* work) {
            WorkData* data = uv_req_get_data((uv_req_t*)work);
            data->req = data->reqF();
        }, [](uv_work_t* work, int status) {
            WorkData* data = uv_req_get_data((uv_req_t*)work);
            data->respF(data->req);
            delete work;
            delete data;
        });
    
        if (ret != 0) {
            delete work;
            delete data;
        }
    }

}

#endif //PANORAMA_LOOPER_H
