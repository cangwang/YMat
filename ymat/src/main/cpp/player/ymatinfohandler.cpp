//
// Created by zhangbaolei on 2024/4/18.
//

#include "ymatinfohandler.h"
#include <utils/ymlog.h>

#define LOG_TAG "YMatInfoHandler"

void
YMatInfoHandler::setVideoInfo(jobject &player, int id, int vectorId, int inFrame, int outFrame) {
    callVoidMethod(player, "setVideoInfo", "(IIII)V", id, vectorId, inFrame, outFrame);
}

void YMatInfoHandler::setPlayerInfo(jobject &player, int width, int height, int duration,
                                    int frameRate) {
    callVoidMethod(player, "setPlayerInfo", "(IIII)V", width, height, duration, frameRate);
}


void YMatInfoHandler::release() {
    javaVM = nullptr;
}

void
YMatInfoHandler::setText2Image(jobject &player, int id, int width, int height,
                               shared_ptr<TexDocAttr> textDocAttr) {
    YMLOGV("setText2Image:%d", id);
    JNIEnv *env;
    bool isNeedDetach = false;
    if (!attachCurrentThread(env, isNeedDetach)) {
        return;
    }
    jclass clsTextDocAttr = env->FindClass("com/yy/ymat/bean/TexDocAttr");
    jmethodID initMethodId = env->GetMethodID(clsTextDocAttr, "<init>",
                                              "(Ljava/lang/String;[FLjava/lang/String;Ljava/lang/String;ILjava/lang/String;ZZFIIIII)V");
    jstring text = env->NewStringUTF(textDocAttr->text.c_str());
    jsize color_size = static_cast<jsize>(textDocAttr->textColor.size());
    jfloatArray color_arr = env->NewFloatArray(color_size);
    env->SetFloatArrayRegion(color_arr, 0, color_size, textDocAttr->textColor.data());
    jstring font = env->NewStringUTF(textDocAttr->font.c_str());
    jstring fontFamily = env->NewStringUTF(textDocAttr->fontFamily.c_str());
    jstring fontStyle = env->NewStringUTF(textDocAttr->fontStyle.c_str());

    jobject textDoc = env->NewObject(clsTextDocAttr, initMethodId, text, color_arr, font,
                                     fontFamily, textDocAttr->fontSize, fontStyle,
                                     textDocAttr->fauxBold, textDocAttr->fauxItalic,
                                     textDocAttr->lineSpacing, textDocAttr->wordSpacing,
                                     textDocAttr->textAligment, textDocAttr->orientation, width,
                                     height);
    jclass clazz_listener = env->GetObjectClass(player);
    jmethodID methodId = env->GetMethodID(clazz_listener, "setText2Image",
                                          "(ILcom/yy/ymat/bean/TexDocAttr;)V");
    env->CallVoidMethod(player, methodId, id, textDoc);

    env->DeleteLocalRef(clsTextDocAttr);
    env->DeleteLocalRef(text);
    env->DeleteLocalRef(color_arr);
    env->DeleteLocalRef(font);
    env->DeleteLocalRef(fontFamily);
    env->DeleteLocalRef(fontStyle);
    env->DeleteLocalRef(textDoc);
    env->DeleteLocalRef(clazz_listener);
    if (isNeedDetach) {
        javaVM->DetachCurrentThread();
    }
}

void
YMatInfoHandler::callVoidMethod(jobject &player, const char *methodName, const char *sign, ...) {
    JNIEnv *env;
    bool isNeedDetach = false;
    if (!attachCurrentThread(env, isNeedDetach)) {
        return;
    }
    jclass clazz_listener = env->GetObjectClass(player);
    jmethodID methodId = env->GetMethodID(clazz_listener, methodName, sign);
    va_list ap;
    va_start(ap, sign);
    env->CallVoidMethodV(player, methodId, ap);
    va_end(ap);
    if (isNeedDetach) {
        javaVM->DetachCurrentThread();
    }
}

bool YMatInfoHandler::attachCurrentThread(JNIEnv *&env, bool &isNeedDetach) {
    if (!javaVM) {
        return false;
    }
    isNeedDetach = false;
    jint res = javaVM->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (res != JNI_OK) {
        res = javaVM->AttachCurrentThread(&env, nullptr);
        if (JNI_OK != res) {
            return false;
        }
        isNeedDetach = true;
    }
    return true;
}

void YMatInfoHandler::handleData(jobject &player, const shared_ptr<YMInfo> &info) {
    YMLOGV("handleData: %d", info->targetComp->layers.size());

    setPlayerInfo(player, info->width, info->height, info->duration, info->frameRate);
    //遍历targetLayer
    handleData(player, info->comps, info->targetComp->layers, -1);
}

void YMatInfoHandler::handleData(jobject &player, vector<shared_ptr<Comp>> comps,
                                 vector<shared_ptr<SimpleLayerInfo>> layers, int vectorId) {
    for (const auto &layerInfo: layers) {
        YMLOGV("handleData: %s", layerInfo->type.c_str());
        if (layerInfo->type == "text") {
            auto textLayerInfo = dynamic_pointer_cast<TextLayerInfo>(
                    layerInfo);
            setText2Image(player, layerInfo->id, layerInfo->width, layerInfo->height,
                          textLayerInfo->textDocAttr);
        } else if (layerInfo->type == "video") {
            setVideoInfo(player, layerInfo->id, vectorId, layerInfo->inFrame, layerInfo->outFrame);
        } else if (layerInfo->type == "precomposition") {
            for (const auto &c: comps) {
                if (c->id == layerInfo->id) {
                    if (c->type == "video") {
                        setVideoInfo(player, layerInfo->id, vectorId, layerInfo->inFrame,
                                     layerInfo->outFrame);
                    } else if (c->type == "vector") {  //循环索引
                        handleData(player, comps, c->layers, c->id);
                    }
                }
            }
        } else if (layerInfo->type == "ShapeLayer") {
            auto shapeLayerInfo = dynamic_pointer_cast<ShapeLayerInfo>(
                    layerInfo);
            for (auto shapeContent: shapeLayerInfo->shapeContents) {
                YMLOGV("shapeBitmap id:%d", shapeLayerInfo->id);
                if (shapeContent->type == PATH) {
                    shapeBitmap(player, shapeLayerInfo->id, shapeLayerInfo->content.c_str(),
                                shapeLayerInfo->width,
                                shapeLayerInfo->height, shapeLayerInfo->inFrame,
                                shapeLayerInfo->outFrame);
                } else if (shapeContent->type == RECT) {
                    auto rectInfo = shapeContent->elements->rectInfo;
                    if (rectInfo) {
                        int width = rectInfo->size[0];
                        int height = rectInfo->size[1];
                        shapeBitmap(player, shapeLayerInfo->id, shapeLayerInfo->content.c_str(),
                                    width,
                                    height,shapeLayerInfo->inFrame,
                                    shapeLayerInfo->outFrame);
                    }
                } else if (shapeContent->type == ELLIPSE) {
                    auto ellipseInfo = shapeContent->elements->ellipseInfo;
                    if (ellipseInfo) {
                        int width = ellipseInfo->size[0];
                        int height = ellipseInfo->size[1];
                        shapeBitmap(player, shapeLayerInfo->id, shapeLayerInfo->content.c_str(),
                                    width,
                                    height,shapeLayerInfo->inFrame,
                                    shapeLayerInfo->outFrame);
                    }
                }
            }
        }
    }
}


YMatInfoHandler::~YMatInfoHandler() {
    javaVM = nullptr;
}

void YMatInfoHandler::shapeBitmap(jobject &player, int id, const char *content,
                                  int width, int height, int inFrame, int outFrame) {
    JNIEnv *env;
    bool isNeedDetach = false;
    if (!attachCurrentThread(env, isNeedDetach)) {
        return;
    }
    jclass clazz_player = env->GetObjectClass(player);
    jmethodID methodId = env->GetMethodID(clazz_player, "shapeBitmap",
                                          "(ILjava/lang/String;IIII)V");
    jstring jcontent = env->NewStringUTF(content);
    env->CallVoidMethod(player, methodId, id,
                        jcontent,
                        width, height, inFrame, outFrame);
    env->DeleteLocalRef(jcontent);
    env->DeleteLocalRef(clazz_player);
    if (isNeedDetach) {
        javaVM->DetachCurrentThread();
    }
}
