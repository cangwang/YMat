//
// Created by Cangwang on 2024/4/1.
//

#pragma once

#include "layer.h"

#define LOG_TAG "VectorLayer"
#define YMLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

/*
 * 预处理组合
 */
namespace ymatrix {
    class VectorLayer: public Layer {

    };
}

