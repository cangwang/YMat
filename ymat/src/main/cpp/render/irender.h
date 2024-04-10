//
// Created by Cangwang on 2024/4/10.
//

#pragma once
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <utils/vertexutil.h>
#include <utils/texcoordsutil.h>
#include <utils/shaderutil.h>
#include <utils/glfloatarray.h>
#include <bean/layerinfo.h>

using namespace std;
namespace ymat {
    class IRender {
        virtual void initRender() = 0;
        virtual void renderFrame() = 0;
        virtual void clearFrame() = 0;
        virtual void destroyRender() = 0;
        virtual void setLayerInfo(SimpleLayerInfo info) = 0;
        virtual void updateViewPort(int width, int height){};
        virtual GLuint getExternalTexture() = 0;
        virtual void releaseTexture() = 0;
    };
}