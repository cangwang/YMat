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
#include <utils/ymlog.h>

#define LOG_TAG "YMat"
#define YMLOGV(...)  ymat::YMLog::get()->i(LOG_TAG, __VA_ARGS__)
#define YMLOGE(...)  ymat::YMLog::get()->e(LOG_TAG, __VA_ARGS__)

#define YMat(sig) Java_com_yy_ymat_utils_YMatJniUtils_##sig

using namespace std;
mutex mtx;
shared_ptr<YMatController> yMatController;
jobject elog;

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

JNIEXPORT void JNICALL YMat(renderFrame)(
        JNIEnv *env,
        jobject instance, jint playerId) {

}

JNIEXPORT void JNICALL YMat(setLog)(
        JNIEnv *env,
        jobject instance, jobject log) {
    YMLOGV("nativeSetListener");

    JavaVM *javaVm;
    if (env->GetJavaVM(&javaVm) != JNI_OK) {
        return;
    }
    if (elog) {
        env->DeleteGlobalRef(elog);
    }
    elog = env->NewGlobalRef(log);

    YMLog::get()->setJavaVM(javaVm);
    YMLog::get()->setELog(elog);
}

}