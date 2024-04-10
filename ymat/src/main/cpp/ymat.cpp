//
// Created by Cangwang on 2024/4/1.
//
#pragma once

#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <bean/yminfo.h>
#include <bean/YMatConfig.h>
#include <mutex>
#include <player/ymatcontroller.h>

#define LOG_TAG "YMat"
#define YMLOGV(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#define YMat(sig) Java_com_yy_ymat_utils_YMatUtils_##sig

using namespace std;
mutex mtx;
shared_ptr<YMatController> yMatController;

extern "C" {

JNIEXPORT jint JNICALL YMat(setYMatConfig)(
        JNIEnv *env,
        jobject instance, jstring json) {
    lock_guard<mutex> auto_lock(mtx);
    if (!yMatController) {
        yMatController = make_shared<YMatController>();
    }

    const char *cJson = env->GetStringUTFChars(json, JNI_FALSE);
    int id = yMatController->addYMat(cJson);
    env->ReleaseStringUTFChars(json, cJson);
    return id;
}

}