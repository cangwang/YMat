//
// Created by Cangwang on 2024/4/10.
//

#pragma once

#include <render/irender.h>

using namespace std;
namespace ymat {
    class Render: public IRender {
    public:
        Render();

        ~Render();

        void initRender() = 0;

        void renderFrame();

        void clearFrame();

        virtual void setLayerInfo(SimpleLayerInfo info) = 0;

        void destroyRender();

        GLuint getExternalTexture();

        void releaseTexture();

        void updateViewPort(int width, int height);

        virtual void draw() = 0;

    protected:
        shared_ptr<GlFloatArray> vertexArray;
        shared_ptr<GlFloatArray> rgbaArray;

        GLuint shaderProgram;
        //shader
        GLuint textureId;

        int surfaceWidth = 0;
        int surfaceHeight = 0;
        bool surfaceSizeChanged = false;
    };
}
