//
// Created by Cangwang on 2024/5/9.
//

#pragma once

#include <layer/AbstractDrawer.h>
#include <layer/Layers.h>

using namespace std;
namespace ymat {
    class ShapeDrawer : public AbstractDrawer {
    public:
        void init() override;

        void draw(shared_ptr<Matrix> parentMatrix, shared_ptr<FrameInfo> frameInfo) override;

        void destroy() override;
    private:
        vector<shared_ptr<Layers>> subLayers;
    };
}
