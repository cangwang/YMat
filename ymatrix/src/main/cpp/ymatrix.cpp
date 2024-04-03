//
// Created by Cangwang on 2024/4/1.
//
#pragma once

#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <bean/yminfo.h>

#define LOG_TAG "YMatrix"
#define YMLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#define YMat(sig) Java_com_yy_ymatrix_utils_YMatrixUtils_##sig

using namespace std;
extern "C" {

JNIEXPORT jint JNICALL YMat(getExternalTexture)(
        JNIEnv *env,
        jobject instance) {
    return 1;
}

JNIEXPORT void JNICALL YMat(setYMatConfig)(
        JNIEnv *env,
        jobject instance, jstring json) {
    const char *cJson = env->GetStringUTFChars(json, JNI_FALSE);

    env->ReleaseStringUTFChars(json, cJson);
}

}