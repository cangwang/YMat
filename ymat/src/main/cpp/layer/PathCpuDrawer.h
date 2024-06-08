//
// Created by asus on 2024/5/19.
//

#pragma once
#include <memory>
#include <layer/ElementCpuDrawer.h>

namespace ymat {
    class PathCpuDrawer: public ElementCpuDrawer {
    public:
        ~PathCpuDrawer();
        PathCpuDrawer(shared_ptr<ShapeContent> shapeContent);
        void init() override;
        void draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) override;
    };
}
