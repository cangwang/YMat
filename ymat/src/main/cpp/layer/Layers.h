//
// Created by Cangwang on 2024/5/9.
//

#pragma once
#include <memory>
#include <layer/AbstractDrawer.h>

using namespace std;
namespace ymat {
    class Layers {
    public:
        Layers(shared_ptr<AbstractDrawer> drawer);
        ~Layers();
        void init();
        void render(shared_ptr<Matrix>, shared_ptr<FrameInfo> frameInfo,  bool fromShape = false);
        void destroy();
        void setMaskLayer(shared_ptr<Layers> layer, int trackMatteType);
        void setBlendMode(int blendMode);
        void setParentWidthHeight(float width, float height);
        shared_ptr<AbstractDrawer> drawer;
    private:

        float parentWidth = 0;
        float parentHeight = 0;

        shared_ptr<Layers> maskLayer;

        GLuint mShaderProgram;

        //顶点位置
        GLint mPositionLocation;
        //纹理位置
        GLint mTextureLocation;
        //纹理
        GLint muTextureLocation;
        //遮罩纹理
        GLint muMaskTextureLocation;

        GLuint shaderProgram;

        //顶点位置
        GLint positionLocation;
        //纹理位置
        GLint textureLocation;
        //纹理
        GLint uTextureLocation;
        //遮罩纹理
        GLint uMaskTextureLocation;
        //混合纹理
        GLint uDstTextureLocation;
        //矩阵
        GLint matrixLocation;
        //是否有透明区域
        GLint isAlpha;
        //整体透明度
        GLint opacity;

        int blendMode = 0;
        int trackMatteType = 0;

        void initMaskProgram();
        void initBlendProgram();
        bool drawMaskBlend(shared_ptr<Matrix> localMatrix, float opacity, shared_ptr<FrameInfo> frameInfo, shared_ptr<Matrix> parentMatrix);

        void initImageProgram();

        void updateImageFramebufferVertices(float width, float height);

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

        float texturecoord[8] = {
                0, 0, 1, 0, 0, 1, 1, 1
        };

        float vertices[8] = {
                1.0f,-1.0f, // 右下
                1.0f,1.0f, // 右上
                -1.0f,-1.0f, // 左下
                -1.0f,1.0f // 左上
        };

        shared_ptr<FrameInfo> srcFrameInfo;
        shared_ptr<FrameInfo> maskFrameInfo;
    };
}
