//
// Created by Cangwang on 2024/4/1.
//

#define LOG_TAG "YMat"
#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/bitmap.h>
#include <android/native_window_jni.h>
#include <bean/yminfo.h>
#include <bean/YMatConfig.h>
#include <mutex>
#include <player/ymatcontroller.h>
#include <utils/ymlog.h>

#define YMat(sig) Java_com_yy_ymat_utils_YMatJniUtils_##sig

using namespace std;
mutex mtx;
shared_ptr<YMatController> yMatController;
jobject elog;

extern "C" {

JNIEXPORT jint JNICALL YMat(createPlayer)(
        JNIEnv *env,
        jobject instance, jboolean useTGFX) {
    lock_guard<mutex> auto_lock(mtx);
    if (!yMatController) {
        yMatController = make_shared<YMatController>();
    }
    return yMatController->createPlayer(useTGFX);
}

JNIEXPORT void JNICALL YMat(setYMatConfig)(
        JNIEnv *env,
        jobject instance, jint playerId, jstring json, jobject player) {
    lock_guard<mutex> auto_lock(mtx);
    if (!yMatController) {
        yMatController = make_shared<YMatController>();
    }
    const char *cJson = env->GetStringUTFChars(json, JNI_FALSE);
    JavaVM *javaVm;
    if (env->GetJavaVM(&javaVm) != JNI_OK) {
        return;
    }
    yMatController->addYMat(playerId, javaVm,cJson, player);
    env->ReleaseStringUTFChars(json, cJson);
    cJson = nullptr;
}

JNIEXPORT void JNICALL YMat(initRender)(
        JNIEnv *env,
        jobject instance, jint playerId, jobject surface) {
    lock_guard<mutex> auto_lock(mtx);
    ANativeWindow *window = ANativeWindow_fromSurface(env,surface);
    if (window == nullptr) {
        YMLOGE("window is nullptr");
        return;
    }
    if (!yMatController) {
        yMatController = make_shared<YMatController>();
    }
    yMatController->initRender(playerId, window);
}

JNIEXPORT jboolean JNICALL YMat(initLayer)(
        JNIEnv *env,
        jobject instance, jint playerId) {
    lock_guard<mutex> auto_lock(mtx);
    if (yMatController) {
        return yMatController->initLayer(playerId);
    }
    return false;
}

JNIEXPORT jint JNICALL YMat(initVideoTexture)(
        JNIEnv *env,
        jobject instance, jint playerId, jint videoId, jint vectorId) {
    lock_guard<mutex> auto_lock(mtx);
    if (yMatController) {
        return yMatController->initVideoTexture(playerId, videoId, vectorId);
    }
    return -1;
}

JNIEXPORT void JNICALL YMat(renderClearFrame)(
        JNIEnv *env,
        jobject instance, jint playerId) {
    if (yMatController) {
        yMatController->renderClearFrame(playerId);
    }
}

JNIEXPORT void JNICALL YMat(updateViewPoint)(
        JNIEnv *env,
        jobject instance, jint playerId, jint width, jint height) {
    if (yMatController) {
        yMatController->updateViewPoint(playerId, width, height);
    }
}

JNIEXPORT jboolean JNICALL YMat(readyRender)(
        JNIEnv *env,
        jobject instance, jint playerId, jint frameIndex) {
    if (yMatController) {
        return yMatController->readyRender(playerId, frameIndex);
    }
    return false;
}

JNIEXPORT void JNICALL YMat(renderFrame)(
        JNIEnv *env,
        jobject instance, jint playerId, jint frameIndex) {
    if (yMatController) {
        yMatController->renderFrame(playerId, frameIndex);
    }
}

JNIEXPORT void JNICALL YMat(renderSwapBuffer)(
        JNIEnv *env,
        jobject instance, jint playerId, jint frameIndex) {
    if (yMatController) {
        yMatController->renderSwapBuffers(playerId, frameIndex);
    }
}

JNIEXPORT void JNICALL YMat(releaseTexture)(
        JNIEnv *env,
        jobject instance, jint playerId) {
    if (yMatController) {
        yMatController->releaseTexture(playerId);
    }
}

JNIEXPORT void JNICALL YMat(releaseVideoTexture)(
        JNIEnv *env,
        jobject instance, jint playerId, jint videoId, jint vectorId, jint videoTextureId) {
    if (yMatController) {
        yMatController->releaseVideoTextureId(playerId, videoId, vectorId, videoTextureId);
    }
}

JNIEXPORT void JNICALL YMat(destroyLayer)(
        JNIEnv *env,
        jobject instance, jint playerId) {
    if (yMatController) {
        yMatController->destroyLayer(playerId);
    }
}

JNIEXPORT void JNICALL YMat(destroyRender)(
        JNIEnv *env,
        jobject instance, jint playerId) {
    if (yMatController) {
        yMatController->destroyRender(playerId);
    }
}

JNIEXPORT jint JNICALL YMat(initImageTexture)(
        JNIEnv *env, jobject instance, jint playerId, jint id, jobject bitmap) {
    if (playerId == -1) {
        YMLOGE("setBgBitmap controller not init");
        return -1;
    }

    if (bitmap == NULL) {
        return -1;
    }

    AndroidBitmapInfo bitmapInfo;
    memset(&bitmapInfo, 0, sizeof(bitmapInfo));
    int result = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        AndroidBitmap_unlockPixels(env, bitmap);
        return -1;
    }

    unsigned char *pixels;
    result = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixels));
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        AndroidBitmap_unlockPixels(env, bitmap);
        return -1;
    }

    int imageId = yMatController->initImageTexture(playerId, id, pixels, &bitmapInfo);

    AndroidBitmap_unlockPixels(env, bitmap);

    return imageId;
}

JNIEXPORT void JNICALL YMat(updateTextureSize)(
        JNIEnv *env, jobject instance, jint playerId, int resId, jint width, jint height) {
    if (yMatController) {
        yMatController->updateTextureSize(playerId, resId, width, height);
    }
}

JNIEXPORT void JNICALL YMat(updateBitmapTextureInfo)(
        JNIEnv *env, jobject instance, jint playerId, int resId, int frameIndex, jobject bitmap) {
    if (yMatController) {
        if (playerId == -1) {
            YMLOGE("updateBitmapTextureInfo controller not init");
            return;
        }

        if (bitmap == NULL) {
            YMLOGE("updateBitmapTextureInfo bitmap not init");
            return;
        }

        AndroidBitmapInfo bitmapInfo;
        memset(&bitmapInfo, 0, sizeof(bitmapInfo));
        int result = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
        if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
            AndroidBitmap_unlockPixels(env, bitmap);
            YMLOGE("updateBitmapTextureInfo bitmap not create success");
            return;
        }

        unsigned char *pixels;
        result = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixels));
        if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
            AndroidBitmap_unlockPixels(env, bitmap);
            YMLOGE("updateBitmapTextureInfo bitmap not init");
            return;
        }
        yMatController->updateBitmapTextureInfo(playerId, resId, frameIndex, pixels, &bitmapInfo);

        AndroidBitmap_unlockPixels(env, bitmap);
    }
}

JNIEXPORT void JNICALL YMat(setLog)(
        JNIEnv *env,
        jobject instance, jobject log) {
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
    YMLOGV("setLog");
}

JNIEXPORT void JNICALL YMat(showImage)(
        JNIEnv *env, jobject instance, jint playerId, jobject bitmap) {
    if (playerId == -1) {
        YMLOGE("setBgBitmap controller not init");
    }

    if (bitmap == NULL) {
        return;
    }

    AndroidBitmapInfo bitmapInfo;
    memset(&bitmapInfo, 0, sizeof(bitmapInfo));
    int result = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    }

    unsigned char *pixels;
    result = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixels));
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    }

    auto textureInfo = TextureLoadUtil::loadTexture(pixels, &bitmapInfo);

    if (yMatController) {
        yMatController->showImage(playerId, textureInfo->textureId);
    }

    AndroidBitmap_unlockPixels(env, bitmap);
}

JNIEXPORT void JNICALL YMat(setParams)(
        JNIEnv *env,
        jobject instance, jint playerId, jint fillMode) {
    if (yMatController) {
        yMatController->setParams(playerId, fillMode);
    }
}

}