//
// Created by Cangwang on 2024/5/9.
//

#pragma once
#include <map>
#include <bean/layerinfo.h>
#include <bean/FrameInfo.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <utils/shaderutil.h>
#include <utils/ymlog.h>
#include <utils/textureloadutil.h>
#include <utils/fillmode.h>

using namespace std;
namespace ymat {
    class AbstractDrawer {
    public:
        ~AbstractDrawer();

        void setOffXY(int x, int y);

        void setAnchorOffXY(int x, int y);

        void setLayerInfo(shared_ptr<SimpleLayerInfo> info);

        void setShapeContent(shared_ptr<ShapeContent> shapeContent, float width, float height);

        void setShapeContentOnly(shared_ptr<ShapeContent> shapeContent);

        void updateWidthHeight(float width, float height);

        void updateImageFramebufferVertices(float width, float height);

        void setMatrixCache(int inFrame, int outFrame);

        shared_ptr<Matrix> getMatrix(int frameIndex);

        shared_ptr<Matrix> getInFrameMatrix(int frameIndex);

        float getOpacity(int frameId, float opacity);

        virtual void init() = 0;

        void updateTextureCoord(bool isAlpha);

        void updateTextureCoord(float lx, float ly, float rx, float ry);

        virtual void draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) = 0;

        void simpleDraw(shared_ptr<Matrix> matrix, int textureId, bool alpha, float opacity, bool isVideo = false);

        void simpleDraw(shared_ptr<Matrix> matrix, int textureId);

        virtual void destroy() = 0;

        void setUseTGFX(bool useTGFX);

        void setUseCache(bool useCache);

        shared_ptr<SimpleLayerInfo> info;
        shared_ptr<ShapeContent> shapeContent;
    protected:

        float offXY[2] = {0, 0};
        float anchorOffXY[2] = {0, 0};
        GLuint shaderProgram;
        shared_ptr<Transform> transform;

        bool useTGFX = false;
        bool useCache = false;

        int width = 0;
        int height = 0;

        float texturecoord[8] = {
                0, 0, 1, 0, 0, 1, 1, 1
        };

        float vertices[8] = {
                1.0f,-1.0f, // 右下
                1.0f,1.0f, // 右上
                -1.0f,-1.0f, // 左下
                -1.0f,1.0f // 左上
        };

        //顶点位置
        GLint uTextureLocation;
        //纹理位置
        GLint positionLocation;
        //纹理位置
        GLint textureLocation;
        //矩阵
        GLint matrixLocation;
        //是否有透明区域
        GLint isAlpha;
        //整体透明度
        GLint opacity;

        map<int, shared_ptr<Matrix>> matrixMap;
    };
}

