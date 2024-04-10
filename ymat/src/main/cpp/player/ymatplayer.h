//
// Created by Cangwang on 2024/4/8.
//

#pragma once

#define LOG_TAG "YMatPlayer"
#define YMLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#include <memory>
#include <list>
#include <player/rendercontroller.h>
#include <layer/layer.h>
#include "bean/yminfo.h"

using namespace std;
namespace ymat {
    class YMatPlayer {
    public:
        YMatPlayer();
        ~YMatPlayer();
        void parse(const char* json);
    private:
        shared_ptr<YMInfo> ymInfo;
        shared_ptr<RenderController> renderController;
    };
}