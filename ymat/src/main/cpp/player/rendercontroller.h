//
// Created by Cangwang on 2024/4/1.
//


#pragma once

#include "egl/eglcore.h"
#include <android/native_window_jni.h>
#include <layer/layer.h>
#include <memory>
#include <list>
#include <bean/YMatConfig.h>

#define LOG_TAG "RenderController"
#define YMLOGV(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
namespace ymat {
    class RenderController {
    public:
        RenderController();
        ~RenderController();
        GLuint initRender(ANativeWindow *window);
        void renderFrame();
        void renderSwapBuffers();
        void renderClearFrame();
        void destroy();
    private:
        list<shared_ptr<Layer>> layers;
        shared_ptr<EGLCore> eglCore;
        int width = 0;
        int height = 0;
    };
}

