//
// Created by asus on 2024/5/19.
//

#pragma once
#include <memory>
#include <layer/ElementCpuDrawer.h>

namespace ymat {
    class RectCpuDrawer : public ElementCpuDrawer {
    public:
        ~RectCpuDrawer();
        RectCpuDrawer(shared_ptr<ShapeContent> shapeContent);
        void init() override;
    };
}