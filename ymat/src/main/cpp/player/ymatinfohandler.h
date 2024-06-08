//
// Created by zhangbaolei on 2024/4/18.
//

#pragma once

#include <jni.h>
#include <string>
#include <bean/yminfo.h>

class YMatInfoHandler {
public:
    YMatInfoHandler(JavaVM *javaVM_) : javaVM(javaVM_) {};

    ~YMatInfoHandler();

    void handleData(jobject &player, const shared_ptr<YMInfo> &info);

    void handleData(jobject &player, vector<shared_ptr<Comp>> comps,
                    vector<shared_ptr<SimpleLayerInfo>> layers, int vectorId);

    void shapeBitmap(jobject &player, int id, const char *element, int width,
                     int height, int inFrame, int outFrame);

    void release();

private:
    JavaVM *javaVM;

    void setPlayerInfo(jobject &player, int width, int height, int duration, int frameRate);

    void setVideoInfo(jobject &player, int id, int vectorId, int inFrame, int outFrame);


    void setText2Image(jobject &player, int id, int width, int height,
                       shared_ptr<TexDocAttr> textDocAttr);

    void callVoidMethod(jobject &player, const char *methodName, const char *sign, ...);

    bool attachCurrentThread(JNIEnv *&env, bool &isNeedDetach);

};
