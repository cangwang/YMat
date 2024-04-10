//
// Created by Cangwang on 2024/4/10.
//

#pragma once

#include <render/irender.h>
#include <utils/glfloatarray.h>
#include <memory>
#include <render/render.h>

#define LOG_TAG "Mp4Render"
#define YMLOGV(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
namespace ymat {
    class Mp4Render: public Render {
    public:
        Mp4Render();

        ~Mp4Render();

        void initRender();

        void setLayerInfo(SimpleLayerInfo info);

        void draw();

    private:
        shared_ptr<GlFloatArray> vertexArray;
        shared_ptr<GlFloatArray> rgbaArray;
        //顶点位置
        GLint uTextureLocation;
        //纹理位置
        GLint positionLocation;
        //纹理位置
        GLint textureLocation;
    };
}
