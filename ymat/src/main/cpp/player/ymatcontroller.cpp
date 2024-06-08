//
// Created by Cangwang on 2024/4/8.
//

#include "ymatcontroller.h"
#define LOG_TAG "YMatController"

YMatController::YMatController() {

}

YMatController::~YMatController() {
    playerMap.clear();
    yMatInfoHandlerMap.clear();
}

int YMatController::createPlayer(bool useTGFX) {
    id++; //id+1；
    auto nativePlayer = make_shared<YMatPlayer>(useTGFX);
    playerMap.insert(make_pair(id, nativePlayer));
    return id;
}

void YMatController::addYMat(int playerId, JavaVM *javaVm, const char *json, jobject& player) {
    if (playerMap.find(playerId) != playerMap.end()) {
        auto ymInfo = playerMap[playerId]->parse(json);
        if (yMatInfoHandlerMap.find(playerId) == yMatInfoHandlerMap.end()) {
            yMatInfoHandlerMap[playerId] = make_shared<YMatInfoHandler>(javaVm);
        }
        if (ymInfo) {
            yMatInfoHandlerMap[playerId]->handleData(player, ymInfo);
        }
    } else {
        YMLOGE("addYMat not find playerId %d", playerId);
    }
}

void YMatController::initRender(int playerId, ANativeWindow *window) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->initRender(playerId, window);
    } else {
        YMLOGE("initRender not find playerId %d", playerId);
    }
}

bool YMatController::initLayer(int playerId) {
    if (playerMap.find(playerId) != playerMap.end()) {
        return playerMap[playerId]->initLayer(playerId);
    } else {
        YMLOGE("initLayer not find playerId %d", playerId);
        return false;
    }
}

void YMatController::updateViewPoint(int playerId, int width, int height) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->updateViewPoint(playerId, width, height);
    } else {
        YMLOGE("updateViewPoint not find playerId %d", playerId);
    }
}

int YMatController::initVideoTexture(int playerId, int videoId, int vectorId) {
    if (playerMap.find(playerId) != playerMap.end()) {
        return static_cast<int>(playerMap[playerId]->initVideoTexture(playerId, videoId, vectorId));
    } else {
        YMLOGE("initVideoTexture not find playerId %d", playerId);
        return TextureLoadUtil::NONE_TEXTURE;
    }
}

bool YMatController::readyRender(int playerId, int frameIndex) {
    if (playerMap.find(playerId) != playerMap.end()) {
        return playerMap[playerId]->readyRender(playerId, frameIndex);
    } else {
        YMLOGE("readyRender not find playerId %d", playerId);
        return false;
    }
}

void YMatController::renderFrame(int playerId, int frameIndex) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->renderFrame(playerId, frameIndex);
    } else {
        YMLOGE("renderFrame not find playerId %d", playerId);
    }
}

int YMatController::initImageTexture(int playerId, int id, unsigned char *bitmap,
                              AndroidBitmapInfo *bitmapInfo) {
    if (playerMap.find(playerId) != playerMap.end()) {
        return static_cast<int>(playerMap[playerId]->initImageTexture(playerId, id, bitmap,
                                                                      bitmapInfo));
    } else {
        YMLOGE("initImageTexture not find playerId %d", playerId);
        return TextureLoadUtil::NONE_TEXTURE;
    }
}

void YMatController::renderClearFrame(int playerId) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->renderClearFrame(playerId);
    } else {
        YMLOGE("renderClearFrame not find playerId %d", playerId);
    }
}

void YMatController::renderSwapBuffers(int playerId, int frameIndex) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->renderSwapBuffers(playerId, frameIndex);
    } else {
        YMLOGE("renderSwapBuffers not find playerId %d", playerId);
    }
}

void YMatController::releaseTexture(int playerId) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->releaseTexture(playerId);
    } else {
        YMLOGE("releaseTexture not find playerId %d", playerId);
    }
}

void YMatController::releaseVideoTextureId(int playerId, int videoId, int vectorId, int videoTextureId) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->releaseVideoTextureId(playerId, videoId, vectorId, videoTextureId);
    } else {
        YMLOGE("releaseVideoTextureId not find playerId %d", playerId);
    }
}

void YMatController::destroyLayer(int playerId) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->destroyLayer(playerId);
    } else {
        YMLOGE("destroyRender not find playerId %d", playerId);
    }
}

void YMatController::destroyRender(int playerId) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->destroyRender(playerId);
        playerMap.erase(playerId);  //移除对象
    } else {
        YMLOGE("destroyRender not find playerId %d", playerId);
    }
    if (yMatInfoHandlerMap.find(playerId) != yMatInfoHandlerMap.end()) {
        yMatInfoHandlerMap[playerId]->release();
        yMatInfoHandlerMap.erase(playerId);
    }
}

void YMatController::updateTextureSize(int playerId, int resId, int width, int height) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->updateTextureSize(playerId, resId, width, height);
    } else {
        YMLOGE("updateTextureInfo not find playerId %d", playerId);
    }
}

void YMatController::updateBitmapTextureInfo(int playerId, int resId, int frameIndex, unsigned char *bitmap, AndroidBitmapInfo *bitmapInfo) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->updateBitmapTextureInfo(playerId, resId, frameIndex, bitmap, bitmapInfo);
    } else {
        YMLOGE("updateBitmapTextureInfo not find playerId %d", playerId);
    }
}

void YMatController::showImage(int playerId, int textureId) {
    if (playerMap.find(playerId) != playerMap.end()) {
//        playerMap[playerId]->showImage(playerId, textureId);
    } else {
        YMLOGE("showImage not find playerId %d", playerId);
    }
}

void YMatController::setParams(int playerId, int fillMode) {
    if (playerMap.find(playerId) != playerMap.end()) {
        playerMap[playerId]->setParams(playerId, fillMode);
    } else {
        YMLOGE("showImage not find playerId %d", playerId);
    }
}
