//
// Created by Cangwang on 2024/4/8.
//

#include "ymatplayer.h"
#define LOG_TAG "YMatPlayer"

ymat::YMatPlayer::YMatPlayer(bool useTGFX): useTGFX(useTGFX) {

}

ymat::YMatPlayer::~YMatPlayer() {
    textureMap.clear();
}

shared_ptr<YMInfo> ymat::YMatPlayer::parse(const char *json) {
    if (!renderController) {
        renderController = make_shared<RenderController>(useTGFX);
    }
    if (json) {
        ymatInfo = make_shared<YMInfo>();
        //解析ymat文件
        YMatConfig::parse(json, ymatInfo);
        return ymatInfo;
    }
    return nullptr;
}

void YMatPlayer::initRender(int playerId, ANativeWindow *window) {
    this->playerId = playerId;
    if (!renderController) {
        renderController = make_shared<RenderController>(useTGFX);
    }
    if (renderController) {
        //初始化native egl和surface
        renderController->initRender(window);
    }
}

/**
 * 等待video和image textureId初始化完后，再去初始化layer
 */
bool YMatPlayer::initLayer(int playerId) {
    if (ymatInfo) {
        YMLOGV("initLayer");
        if (renderController) {
            //初始化矩阵
            renderController->setCamera(ymatInfo->width, ymatInfo->height);
//        //设置组合容器
//        renderController->setComps(ymatInfo->comps);
            //设置运行容器
            renderController->setBgColor(ymatInfo->bgColor);
            //初始化渲染层
            renderController->setLayers(ymatInfo->comps, ymatInfo->targetComp->layers);
            return true;
        }
    } else {
        YMLOGE("initLayer yMatInfo is null");
    }
    return false;
}

void YMatPlayer::updateViewPoint(int playerId, int width, int height) {
    if (renderController) {
        renderController->updateViewPoint(width, height);
    }
}

bool YMatPlayer::readyRender(int playerId, int frameIndex) {
    if (renderController) {
        return renderController->readyRender(frameIndex);
    }
    return false;
}


void YMatPlayer::renderFrame(int playerId, int frameIndex) {
    if (renderController) {
        renderController->renderFrame(frameIndex);
    }
}

void YMatPlayer::renderClearFrame(int playerId) {
    if (renderController) {
        renderController->renderClearFrame();
    }
}

void YMatPlayer::renderSwapBuffers(int playerId, int frameIndex) {
    if (renderController) {
        renderController->renderSwapBuffers(frameIndex);
    }
}

void YMatPlayer::releaseTexture(int playerId) {
    if (renderController) {
        renderController->releaseTexture();
    }
}

void YMatPlayer::releaseVideoTextureId(int playerId, int videoId, int vectorId,
                                       int videoTextureId) {
    TextureLoadUtil::releaseTexture(videoTextureId);
    if (renderController) {
        renderController->releaseVideoTextureId(playerId, videoId, vectorId, videoTextureId);
    }
}

void YMatPlayer::destroyRender(int playerId) {
    if (renderController) {
        renderController->destroyRender();
    }
}

void YMatPlayer::destroyLayer(int playerId) {
    if (renderController) {
        renderController->destroyLayer();
    }
//    if (ymatInfo) {
//        ymatInfo->layers.clear();
//    }
    textureMap.clear();
}

unsigned int YMatPlayer::initImageTexture(int playerId, int imageId, unsigned char *bitmap,
                                          AndroidBitmapInfo *bitmapInfo) {
    if (textureMap.find(imageId) != textureMap.end()) {
        return textureMap[imageId]->textureId;
    }
    //创建图片纹理
    auto textureInfo = TextureLoadUtil::loadTexture(bitmap, bitmapInfo);
    textureInfo->resId = imageId;
    textureMap.insert(make_pair(textureInfo->resId, textureInfo));
    if (ymatInfo) { //写入textureId
        if (ymatInfo->layers.find(imageId) != ymatInfo->layers.end()) {
            ymatInfo->layers[imageId]->textureInfo = textureInfo;
        }
    }
    return textureInfo->textureId;
}

unsigned int YMatPlayer::initVideoTexture(int playerId, int videoId, int vectorId) {
    //创建视频纹理
    auto textureInfo = TextureLoadUtil::loadVideoTexture();
    textureInfo->resId = videoId;
    textureMap.insert(make_pair(textureInfo->resId, textureInfo));
    if (ymatInfo) { //写入textureId
        if (ymatInfo->layers.find(videoId) != ymatInfo->layers.end()) {
            ymatInfo->layers[videoId]->textureInfo = textureInfo;
        }
    }
    return textureInfo->textureId;
}

/**
 * 用于更新video纹理尺寸
 * @param playerId
 * @param resId
 * @param width
 * @param height
 */
void YMatPlayer::updateTextureSize(int playerId, int resId, int width, int height) {
    if (textureMap.find(resId) != textureMap.end()) {
        auto info= textureMap[resId];
        info->width = width;
        info->height = height;
    } else {
        YMLOGE("updateTextureSize not find playerId %d", resId);
    }
}

void YMatPlayer::updateBitmapTextureInfo(int playerId, int resId, int frameIndex, unsigned char *bitmap,
                                         AndroidBitmapInfo *bitmapInfo) {
    if (textureMap.find(resId) != textureMap.end()) {
        auto textureInfo = textureMap[resId];
        //更新图片纹理
        TextureLoadUtil::updateTexture(textureInfo, bitmap, bitmapInfo);
        if (ymatInfo) { //写入textureId
            if (ymatInfo->layers.find(resId) != ymatInfo->layers.end()) {
                ymatInfo->layers[resId]->textureInfo = textureInfo;
            }
        }
        textureInfo->forceUpdate = textureInfo->frameIndex != frameIndex;
        textureInfo->frameIndex = frameIndex;
    } else {
        YMLOGE("updateBitmapTextureInfo not find playerId %d", resId);
    }
}

void YMatPlayer::setParams(int playerId, int fillMode) {
    if (renderController) {
        renderController->setParams(playerId, fillMode);
    }
}

