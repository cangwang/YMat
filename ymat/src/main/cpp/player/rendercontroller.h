//
// Created by Cangwang on 2024/4/1.
//


#pragma once

#include "egl/eglcore.h"
#include <memory>

#define LOG_TAG "RenderController"
#define YMLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
namespace ymatrix {
    class RenderController {
    public:
        RenderController();
        ~RenderController();
        void renderFrame();
        void renderSwapBuffers();
        void renderClearFrame();
        void destroy();
    private:
        shared_ptr<EGLCore> eglCore;
        int width = 0;
        int height = 0;
    };
}

