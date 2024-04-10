//
// Created by Cangwang on 2024/4/1.
//

#pragma once

#include "layer.h"

#define LOG_TAG "VideoLayer"
#define YMLOGV(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

namespace ymatrix {
    class VideoLayer: public Layer {
    public:
        VideoLayer();
        ~VideoLayer();
        void setLayerInfo(shared_ptr<SimpleLayerInfo> layer) override;
        void renderFrame() override;
        void release() override;
    };
}