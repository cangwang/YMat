//
// Created by Cangwang on 2024/5/9.
//

#include "ImageDrawer.h"
#include <utils/shaderutil.h>

#define LOG_TAG "ImageDrawer"

ImageDrawer::ImageDrawer() {

}

ImageDrawer::~ImageDrawer() {

}

void ImageDrawer::init() {
    if (info->isAlpha) {
        shaderProgram = ShaderUtil::createProgram(TextureLoadUtil::SIMPLE_VERTEX_SHADER,
                                                  TextureLoadUtil::ALPHA_IMAGE_FRAGMENT_SHADER);
        isAlpha = glGetUniformLocation(shaderProgram, "u_isAlpha");
    } else {
        shaderProgram = ShaderUtil::createProgram(TextureLoadUtil::SIMPLE_VERTEX_SHADER,
                                                  TextureLoadUtil::NORMAL_IMAGE_FRAGMENT_SHADER);
    }
    uTextureLocation = glGetUniformLocation(shaderProgram, "u_texture");
    positionLocation = glGetAttribLocation(shaderProgram, "a_position");
    textureLocation = glGetAttribLocation(shaderProgram, "a_texcoord");
    matrixLocation = glGetUniformLocation(shaderProgram,"u_matrix");
    opacity = glGetUniformLocation(shaderProgram, "u_opacity");
    if (info) {
        if (info->textureInfo) {
            getFillCoord(info->textureInfo->width, info->textureInfo->height,
                         width, height, info->isAlpha,
                         info->fillMode);
        } else {
            YMLOGE("info name %s textureInfo is null", info->name.c_str());
        }
    }
}

/**
 * 处理纹理拉伸
 * @param texW
 * @param texH
 * @param dstW
 * @param dstH
 * @param isAlpha
 * @param fillMode
 */
void ImageDrawer::getFillCoord(float texW, float texH,
                               float dstW, float dstH,
                               bool isAlpha, int fillMode) {
    float lx = 0;
    float ly = 0;
    float rx = isAlpha ? 0.5 : 1.0;
    float ry = 1.0;
    sw = 1.0;
    sh = 1.0;

    float srcW = isAlpha ? texW * 0.5f : texW;
    float srcH = texH;
    if (fillMode == FillMode::AspectFill) {
        float srcWhr = srcW / srcH;
        float dstWhr = dstW / dstH;
        bool isLead = dstWhr < srcWhr;
        float tw = isLead ? dstH * srcWhr : dstW;
        float th = isLead ? dstH : dstW / srcWhr;
        lx = (dstW - tw) * 0.5f;
        ly = (dstH - th) * 0.5f;
        lx = -lx / texW;
        ly = -ly / th;
        rx = rx - lx;
        ry = ry - ly;
    } else if (fillMode == FillMode::FitXY) {
        // 默认拉伸
    } else {
        float srcWhr = srcW / srcH;
        float dstWhr = dstW / dstH;
        bool isLead = dstWhr < srcWhr;
        float tw = isLead ? dstW : dstH * srcWhr;
        float th = isLead ? dstW / srcWhr : dstH;
        sw = tw / dstW;
        sh = th / dstH;
    }
    traslateM = make_shared<Matrix>();
    Matrix::translation(*traslateM, width * (1 - sw) * 0.5f, - height * (1 - sh) * 0.5f, 1);

    updateTextureCoord(lx, ly, rx, ry);
}

void ImageDrawer::draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) {
    YMLOGV("draw name %s", info->name.c_str());
    if (info && info->textureInfo && info->textureInfo->textureId != 0) {
        bool isAlpha = true;
        if (info) {
            isAlpha = info->isAlpha;
        }
        Matrix::multiply(*matrix, *traslateM, *matrix);
        matrix->scale(sw, sh, 1.0);
        simpleDraw(matrix, info->textureInfo->textureId, isAlpha, transform->getOpacity(frameInfo->frameId));
    }
}

void ImageDrawer::destroy() {
    if (info && info->textureInfo) {
        glDeleteTextures(1, &info->textureInfo->textureId);
    }
    glDeleteProgram(shaderProgram);
}
