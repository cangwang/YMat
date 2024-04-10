//
// Created by Cangwang on 2024/4/1.
//

#pragma once

#include <memory>
#include <bean/layerinfo.h>

using namespace std;
namespace ymat {
    class Layer {
    public:
        virtual void setLayerInfo(shared_ptr<SimpleLayerInfo> layer) = 0;

        virtual void renderFrame() = 0;

        virtual void release() = 0;

    protected:
        shared_ptr<SimpleLayerInfo> layer;
        int frameIndex;
        int width;
        int height;
    };
}
