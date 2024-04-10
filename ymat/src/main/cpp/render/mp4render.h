//
// Created by Cangwang on 2024/4/10.
//

#pragma once

#include <render/irender.h>
#include <utils/glfloatarray.h>
#include <memory>

#define LOG_TAG "Mp4Render"
#define YMLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
namespace ymat {
    class Mp4Render: public IRender {
    public:
        Mp4Render();

        ~Mp4Render();

        void initRender();

        void renderFrame();

        void clearFrame();

        void destroyRender();

        GLuint getExternalTexture();

        void releaseTexture();

        void swapBuffers();

        void updateViewPort(int width, int height);

        void draw();

    private:
        shared_ptr<GlFloatArray> vertexArray;
        shared_ptr<GlFloatArray> rgbaArray;

        GLuint shaderProgram;
        //shader
        GLuint textureId;
        //顶点位置
        GLint uTextureLocation;
        //纹理位置
        GLint positionLocation;
        //纹理位置
        GLint textureLocation;

        int surfaceWidth = 0;
        int surfaceHeight = 0;
        bool surfaceSizeChanged = false;
    };
}

