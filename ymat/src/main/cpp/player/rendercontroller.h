//
// Created by Cangwang on 2024/4/1.
//


#pragma once

#include <egl/eglcore.h>
#include <android/native_window_jni.h>
#include <layer/Layers.h>
#include <memory>
#include <bean/YMatConfig.h>
#include <utils/ymlog.h>
#include <layer/ImageDrawer.h>
#include <layer/VideoDrawer.h>
#include <layer/VectorDrawer.h>
#if USE_TGFX
#include "src/main/cpp/tgfx/TextDrawer.h"
#endif
#include <layer/ShapeDrawer.h>
#include <layer/TextCpuDrawer.h>
#include <utils/textureloadutil.h>

using namespace std;
namespace ymat {
    class RenderController {
    public:
        RenderController(bool useTGFX);
        ~RenderController();
        GLuint initRender(ANativeWindow *window);
        void updateViewPoint(int width, int height);
        void setCamera(int width, int height);
        bool readyRender(int frameIndex);
        void renderFrame(int frameIndex);
        void renderSwapBuffers(int frameIndex);
        void renderClearFrame();
        void releaseTexture();
        void releaseVideoTextureId(int playerId, int videoId, int vectorId,
                              int videoTextureId);
        void destroyRender();
        void destroyLayer();
        void setLayers(vector<shared_ptr<Comp>>, vector<shared_ptr<SimpleLayerInfo>> infos);
        shared_ptr<Layers> setLayer(vector<shared_ptr<Comp>> comps, int id, string type, bool isTrackMatte, bool findTrack);
        void setBgColor(vector<float> bgColor);
        shared_ptr<SimpleLayerInfo> findMaskInfo(vector<shared_ptr<SimpleLayerInfo>> infos, int trackMatteLayer);
        void setFillMode(int fillMode);
        void initImageProgram();

        void setParams(int playerId, int fillMode);

        void setFillCoord(float texW, float texH,
                     float dstW, float dstH,
                     bool isAlpha, int fillMode);

    private:
        vector<shared_ptr<Layers>> targetLayers;

        shared_ptr<FrameInfo> frameInfo;

        shared_ptr<EGLCore> eglCore;
        float width = 0.0f;
        float height = 0.0f;
        float surfaceWidth = 0.0f;
        float surfaceHeight = 0.0f;
        shared_ptr<Matrix> camera;
        vector<float> bgColor;

        GLuint shaderProgram;

        //顶点位置
        GLint positionLocation;
        //纹理位置
        GLint textureLocation;
        //纹理
        GLint uTextureLocation;
        //矩阵
        GLint matrixLocation;

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
        shared_ptr<Matrix> orthoMmatrix;

        int fillMode = 0;  //0全屏, 1缩放居中,2实际比例缩放居中,3centerCrop

        bool useTGFX = false;
    };
}

