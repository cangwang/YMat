//
// Created by Cangwang on 2024/4/10.
//
#pragma once

#include "render.h"

#define LOG_TAG "EvaRender"
#define YMLOGV(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

/**
 * 支持透明度的渲染器
 */
using namespace std;
namespace ymat {
    class EvaRender: Render {
    public:
        EvaRender();

        ~EvaRender();

        void setLayerInfo(SimpleLayerInfo info);

        void initRender();

        void draw();

    private:
        shared_ptr<GlFloatArray> vertexArray;
        shared_ptr<GlFloatArray> alphaArray;
        shared_ptr<GlFloatArray> rgbArray;

        //顶点位置
        GLint uTextureLocation;
        //rgb纹理位置
        GLint aPositionLocation;
        //alpha纹理位置
        GLint aTextureAlphaLocation;
        GLint aTextureRgbLocation;
    };
}