//
// Created by Cangwang on 2024/4/8.
//

#pragma once

#include <utils/ymlog.h>
#include <memory>
#include <player/rendercontroller.h>
#include <bean/yminfo.h>
#include <android/bitmap.h>
#include <utils/textureloadutil.h>

using namespace std;
namespace ymat {
    class YMatPlayer {
    public:
        YMatPlayer(bool useTGFX);
        ~YMatPlayer();
        shared_ptr<YMInfo> parse(const char* json);
        void initRender(int playerId, ANativeWindow *window);
        bool initLayer(int playerId);
        void updateViewPoint(int playerId, int width, int height);
        bool readyRender(int playerId, int frameIndex);
        void renderFrame(int playerId, int frameIndex);
        void renderClearFrame(int playerId);
        void renderSwapBuffers(int playerId, int frameIndex);
        void releaseTexture(int playerId);
        void releaseVideoTextureId(int playerId, int videoId, int vectorId, int videoTextureId);
        void destroyRender(int playerId);
        void destroyLayer(int playerId);
        unsigned int initImageTexture(int playerId, int id, unsigned char *bitmap, AndroidBitmapInfo *bitmapInfo);
        unsigned int initVideoTexture(int playerId, int id, int vectorId);

        void updateTextureSize(int playerId, int resId, int width, int height);

        void updateBitmapTextureInfo(int playerId, int resId, int frameIndex, unsigned char *bitmap, AndroidBitmapInfo *bitmapInfo);

        void setParams(int playerId, int fillMode);

    private:
        int playerId;
        shared_ptr<YMInfo> ymatInfo;
        shared_ptr<RenderController> renderController;
        map<int, shared_ptr<TextureInfo>> textureMap;
        bool useTGFX = false;
    };
}