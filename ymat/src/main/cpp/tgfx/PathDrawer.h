//
// Created by Cangwang on 2024/5/9.
//

#pragma once

#include <tgfx/ElementDrawer.h>
#include <include/tgfx/core/PathEffect.h>

using namespace std;
namespace ymat {
    class PathDrawer: public ElementDrawer {
    public:
        ~PathDrawer();
        PathDrawer(shared_ptr<ShapeContent> shapeContent);
        void init() override;
        int drawShape(shared_ptr<FrameInfo> frameInfo, tgfx::Canvas *canvas) override;
        void destroy() override;
    private:
        vector<vector<float>> points;
    };
}
