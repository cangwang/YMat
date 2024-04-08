//
// Created by Cangwang on 2024/4/8.
//

#pragma once

#define LOG_TAG "YMatPlayer"
#define YMLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#include <memory>
#include <map>
#include "ymatplayer.h"

using namespace std;
namespace ymatrix {
    class YMatController {
    public:
        map<int, shared_ptr<YMatPlayer>> playerMap;
    };
}
