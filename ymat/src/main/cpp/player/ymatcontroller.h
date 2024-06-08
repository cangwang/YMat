//
// Created by Cangwang on 2024/4/8.
//

#pragma once

#include <utils/ymlog.h>
#include <memory>
#include <map>
#include <player/ymatplayer.h>
#include <android/bitmap.h>
#include <player/ymatinfohandler.h>

using namespace std;
namespace ymat {
    class YMatController {
    public:
        YMatController();

        ~YMatController();

        int createPlayer(bool useTGFX);

        void addYMat(int playerId, JavaVM *javaVm, const char *json, jobject& player);

        void initRender(int playerId, ANativeWindow *window);

        bool initLayer(int playerId);

        void updateViewPoint(int playerId, int width, int height);

        bool readyRender(int playerId, int frameIndex);

        void renderFrame(int playerId, int frameIndex);

        void renderClearFrame(int playerId);

        void renderSwapBuffers(int playerId, int frameIndex);

        void releaseTexture(int playerId);

        void releaseVideoTextureId(int playerId, int videoId, int vectorId,
                                   int videoTextureId);

        void destroyRender(int playerId);

        void destroyLayer(int playerId);

        int initVideoTexture(int playerId, int videoId, int vectorId);

        int initImageTexture(int playerId, int id, unsigned char *bitmap, AndroidBitmapInfo *bitmapInfo);

        void updateTextureSize(int playerId, int resId, int width, int height);

        void updateBitmapTextureInfo(int playerId, int resId, int frameIndex, unsigned char *bitmap, AndroidBitmapInfo *bitmapInfo);

        void showImage(int playerId,int textureId);

        void setParams(int playerId, int fillMode);

    private:
        map<int, shared_ptr<YMatPlayer>> playerMap;
        int id = 0;
        map<int, shared_ptr<YMatInfoHandler>> yMatInfoHandlerMap;
    };
}
