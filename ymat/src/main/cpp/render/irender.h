//
// Created by Cangwang on 2024/4/10.
//

#pragma once
#include <GLES3/gl3.h>

using namespace std;
namespace ymat {
    class IRender {
        virtual void initRender() = 0;
        virtual void renderFrame() = 0;
        virtual void clearFrame() = 0;
        virtual void destroyRender() = 0;
        virtual void updateViewPort(int width, int height){};
        virtual GLuint getExternalTexture() = 0;
        virtual void releaseTexture() = 0;
        virtual void swapBuffers() = 0;
    };
}