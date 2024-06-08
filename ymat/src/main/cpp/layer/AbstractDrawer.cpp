//
// Created by Cangwang on 2024/5/9.
//

#include "AbstractDrawer.h"

#define LOG_TAG "AbstractDrawer"

AbstractDrawer::~AbstractDrawer() {
    matrixMap.clear();
    glDeleteProgram(shaderProgram);
}

/**
 * 相对父布局偏移
 * @param x
 * @param y
 */
void AbstractDrawer::setOffXY(int x, int y) {
    this->offXY[0] = x;
    this->offXY[1] = y;
}

/**
 * 描点偏移
 * @param x
 * @param y
 */
void AbstractDrawer::setAnchorOffXY(int x, int y) {
    this->anchorOffXY[0] = x;
    this->anchorOffXY[1] = y;
}

void AbstractDrawer::setLayerInfo(shared_ptr<SimpleLayerInfo> info) {
    this->info = info;
    this->transform = info->transform;
    updateWidthHeight(info->width, info->height);
}

void AbstractDrawer::updateWidthHeight(float width, float height) {
    this->width = width;
    this->height = height;
    const float x1 = 0;
    const float y1 = 0;
    const float x2 = width;
    const float y2 = -height;
    vertices[0] = x1;
    vertices[1] = y1;
    vertices[2] = x2;
    vertices[3] = y1;
    vertices[4] = x1;
    vertices[5] = y2;
    vertices[6] = x2;
    vertices[7] = y2;
}

void AbstractDrawer::updateImageFramebufferVertices(float width, float height) {
    this->width = width;
    this->height = height;
    const float x1 = 0;
    const float y1 = -height;
    const float x2 = width;
    const float y2 = 0;
    vertices[0] = x1;
    vertices[1] = y1;
    vertices[2] = x2;
    vertices[3] = y1;
    vertices[4] = x1;
    vertices[5] = y2;
    vertices[6] = x2;
    vertices[7] = y2;
}

/**
 * shape layer使用
 * @param shapeContent
 * @param width
 * @param height
 */
void AbstractDrawer::setShapeContent(shared_ptr<ShapeContent> shapeContent, float width, float height) {
    this->shapeContent = shapeContent;
    this->transform = shapeContent->transforms;
    updateWidthHeight(width, height);
}

/**
 * path layer使用
 * @param shapeContent
 */
void AbstractDrawer::setShapeContentOnly(shared_ptr<ShapeContent> shapeContent) {
    this->shapeContent = shapeContent;
    this->transform = shapeContent->transforms;
}

/**
 * 创建时放入缓存
 * @param inFrame
 * @param outFrame
 */
void AbstractDrawer::setMatrixCache(int inFrame, int outFrame) {
    //不知为何开启后会出现泄露
    if (useCache) {
        matrixMap.clear();
        for (int i = inFrame; i < outFrame; ++i) {
            matrixMap.insert(make_pair(i, getInFrameMatrix(i)));
        }
    }
}

shared_ptr<Matrix> AbstractDrawer::getMatrix(int frameId) {
    if (useCache) {
        if (matrixMap.find(frameId) != matrixMap.end()) {
            return matrixMap[frameId];
        } else {
            getInFrameMatrix(frameId);
        }
    } else {
        return getInFrameMatrix(frameId);
    }
}

shared_ptr<Matrix> AbstractDrawer::getInFrameMatrix(int index) {
    if (transform != nullptr) {
//        int index = frameIndex;
//        if (frameIndex > info->outFrame) {
//            index = info->outFrame;
//        }
        shared_ptr<Matrix> m = make_shared<Matrix>();
        auto position = transform->getPosition(index, offXY[0], offXY[1]);
        if (!position.empty()) {
            Matrix::translation(*m, position[0], -position[1], -position[2]);
        }

        auto rotationX = transform->getRotationX(index);
        if (rotationX && *rotationX != 0) {
            m->xRotate(degToRad(*rotationX));
        }

        auto rotationY = transform->getRotationY(index);
        if (rotationY && *rotationY != 0) {
            m->yRotate(degToRad(360 - *rotationY));
        }

        auto rotationZ = transform->getRotationZ(index);
        if (rotationZ && *rotationZ != 0) {
            m->zRotate(degToRad(360 - *rotationZ));
        }

        auto orientation = transform->getOrientation(index);
        if (!orientation.empty()) {
            m->xRotate(degToRad(orientation[0]));
            m->yRotate(degToRad(360 - orientation[1]));
            m->zRotate(degToRad(360 - orientation[2]));
        }

        auto scale = transform->getScale(index);
        if (scale.size() == 3) {
            float x = scale.size() > 0 ? scale[0] * 0.01f : 1.0f;
            float y = scale.size() > 1 ? scale[1] * 0.01f : 1.0f;
            float z = scale.size() > 2 ? scale[2] * 0.01f : 1.0f;
            m->scale(x, y, z);
        }

        auto anchorPoint = transform->getAnchorPoint(index, anchorOffXY[0], anchorOffXY[1]);
        if (anchorPoint.size() == 3) {
            shared_ptr<Matrix> moveOrighMatrix = make_shared<Matrix>();
            Matrix::translation(*moveOrighMatrix, -anchorPoint[0], anchorPoint[1], 0);
            Matrix::multiply(*m, *moveOrighMatrix, *m);
        }
        return m;
    } else {
        YMLOGE("transform is null");
    }
    return nullptr;
}

float AbstractDrawer::getOpacity(int frameId, float opacity) {
    if (transform != nullptr) {
        return transform->getOpacity(frameId);
    }
    return 1.0f;
}


void AbstractDrawer::updateTextureCoord(bool isAlpha) {
    texturecoord[0] = 0;
    texturecoord[1] = 0;
    texturecoord[2] = isAlpha ? 0.5f : 1;
    texturecoord[3] = 0;
    texturecoord[4] = 0;
    texturecoord[5] = 1;
    texturecoord[6] = isAlpha ? 0.5f : 1;
    texturecoord[7] = 1;
}

/**
 * 图片存在缩放
 * @param lx
 * @param ly
 * @param rx
 * @param ry
 */
void AbstractDrawer::updateTextureCoord(float lx, float ly, float rx, float ry) {
    texturecoord[0] = lx;
    texturecoord[1] = ly;
    texturecoord[2] = rx;
    texturecoord[3] = ly;
    texturecoord[4] = lx;
    texturecoord[5] = ry;
    texturecoord[6] = rx;
    texturecoord[7] = ry;
}

void AbstractDrawer::setUseTGFX(bool useTGFX) {
    this->useTGFX = useTGFX;
}

void AbstractDrawer::setUseCache(bool useCache) {
    this->useCache = useCache;
}

void AbstractDrawer::simpleDraw(shared_ptr<Matrix> matrix, int textureId, bool alpha, float opacity, bool isVideo) {
    // 着色器
    glUseProgram(shaderProgram);
    // 顶点
    glEnableVertexAttribArray(positionLocation);
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    // 矩阵设置
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, matrix->values);
    glActiveTexture(GL_TEXTURE0);
    if (isVideo) {
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, info->textureInfo->textureId);
    } else {
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
    // 纹理坐标
    glUniform1i(uTextureLocation, 0);
    glEnableVertexAttribArray(textureLocation);
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, texturecoord);

    if (isAlpha >= 0) {
        glUniform1i(isAlpha, alpha ? 1 : 0);
    }
    glUniform1f(this->opacity, opacity);

    //启动混合
    glEnable(GL_BLEND);
    //基于alpha通道的半透明混合函数
//        void glBlendFuncSeparate(GLenum srcRGB,
//             GLenum dstRGB,
//             GLenum srcAlpha,
//             GLenum dstAlpha);
//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //关闭混合
    glDisable(GL_BLEND);
}

void AbstractDrawer::simpleDraw(shared_ptr<Matrix> matrix, int textureId) {
    // 着色器
    glUseProgram(shaderProgram);
    // 顶点
    glEnableVertexAttribArray(positionLocation);
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    // 矩阵设置
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, matrix->values);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // 纹理坐标
    glUniform1i(uTextureLocation, 0);
    glEnableVertexAttribArray(textureLocation);
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, texturecoord);
    //启动混合
    glEnable(GL_BLEND);
    //基于alpha通道的半透明混合函数
//        void glBlendFuncSeparate(GLenum srcRGB,
//             GLenum dstRGB,
//             GLenum srcAlpha,
//             GLenum dstAlpha);
//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //关闭混合
    glDisable(GL_BLEND);
}
