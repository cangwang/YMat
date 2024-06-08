//
// Created by Cangwang on 2024/5/9.
//

#pragma once

#include <tgfx/ElementDrawer.h>

using namespace std;
namespace ymat {
    class EllipseDrawer: public ElementDrawer {
    public:
        EllipseDrawer(shared_ptr<ShapeContent> shapeContent);
        void init() override;
        int drawShape(shared_ptr<FrameInfo> frameInfo, tgfx::Canvas *canvas) override;
        void destroy() override;
    };
}
