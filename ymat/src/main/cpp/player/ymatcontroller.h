//
// Created by Cangwang on 2024/4/8.
//

#pragma once

#define LOG_TAG "YMatPlayer"
#define YMLOGV(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#include <memory>
#include <map>
#include "ymatplayer.h"

using namespace std;
namespace ymat {
    class YMatController {
    public:
        YMatController();
        ~YMatController();

        int addYMat(const char* json);
    private:
        map<int, shared_ptr<YMatPlayer>> playerMap;
        int id = 0;
    };
}
