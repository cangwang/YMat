//
// Created by asus on 2024/5/19.
//

#pragma once
#include <memory>
#include <layer/ElementCpuDrawer.h>

namespace ymat {
    class EllipseCpuDrawer : public ElementCpuDrawer {
    public:
        ~EllipseCpuDrawer();
        EllipseCpuDrawer(shared_ptr<ShapeContent> shapeContent);
        void init() override;
    };
}