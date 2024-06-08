//
// Created by Cangwang on 2024/5/9.
//

#pragma once
#include <layer/AbstractDrawer.h>

using namespace std;
namespace ymat {
    class ImageDrawer: public AbstractDrawer {
    public:
        ImageDrawer();
        ~ImageDrawer();
        void init() override;
        void draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) override;
        void destroy() override;
    private:
        void getFillCoord(float texW, float texH, float dstW, float dstH, bool isAlpha = false, int fillmode = FillMode::AspectFill);
        shared_ptr<Matrix> traslateM;
        float sw;
        float sh;
    };
}
