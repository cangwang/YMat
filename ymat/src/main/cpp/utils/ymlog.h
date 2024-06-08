//
// Created by asus on 2024/4/14.
//

#pragma once

#include <jni.h>
#include <string>
#include <cstdarg> // 用于va_list, va_start, va_end等宏
#include <mutex>

#define YMLOGV(...)  ymat::YMLog::get()->i(LOG_TAG, __VA_ARGS__)
#define YMLOGE(...)  ymat::YMLog::get()->e(LOG_TAG, __VA_ARGS__)
#define YMLOGD(...)  ymat::YMLog::get()->d(LOG_TAG, __VA_ARGS__)

using namespace std;
namespace ymat {
    class YMLog {
    public:
        ~YMLog();

        void setJavaVM(JavaVM* _javaVm) {
            javaVM = _javaVm;
        };

        void setELog(jobject &_elog) {
            elog = _elog;
        };

        void i(string tagName, char* format, ...);

        void d(string tagName, char* format, ...);

        void e(string tagName, char* format, ...);

        static YMLog* get();

        mutex mtx;

    private:
        YMLog(): javaVM(nullptr){};
        int BUF_MAX_SIZE = 2048;
        JavaVM *javaVM;
        jobject elog = nullptr;

        void log(string level, string tagName, char* format, va_list args);

        void log(string level, string tagName, string msg);

        bool attachCurrentThread(JNIEnv *&env, bool &isNeedDetach);
    };
    static once_flag s_init_instance_flag;
    static ymat::YMLog* controller;
}

