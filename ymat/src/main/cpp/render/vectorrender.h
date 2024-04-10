//
// Created by Cangwang on 2024/4/10.
//

#pragma once

#include <render/render.h>
#include <layer/layer.h>

#define LOG_TAG "VectorRender"
#define YMLOGV(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

using namespace std;
namespace ymat {
    class VectorRender: public Render {
    public:
        VectorRender();

        ~VectorRender();

        void initRender();

        void setLayerInfo(SimpleLayerInfo info);

        void draw();

        void initRecordFrameBuffer(int width, int height);

        void destroyRecordFrameBuffer();
    private:
        list<shared_ptr<Layer>> layers;
    private:
        GLfloat textureVertices[8] = {
                -1.0f, -1.0f, // bot left
                -1.0f, 1.0f, // top left
                1.0f, -1.0f, // bot right
                1.0f, 1.0f, // top right
        };
        GLfloat texture[8] = {
                0.0f, 0.0f, // bot left
                0.0f, 1.0f, // top left
                1.0f, 0.0f, // bot right
                1.0f, 1.0f, // top right
        };
        //顶点位置
        GLint uTextureLocation;
        //纹理位置
        GLint positionLocation;
        //纹理位置
        GLint textureLocation;

        GLuint mFrameBuffer = -1;
        GLuint mFrameBufferTextures = -1;
        int mFrameWidth = -1;
        int mFrameHeight = -1;
    };
}
