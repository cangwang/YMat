//
// Created by Cangwang on 2024/5/20.
//

#pragma once

#include <layer/AbstractDrawer.h>

namespace ymat {
    class ElementCpuDrawer: public AbstractDrawer {
    public:
        ~ElementCpuDrawer();

        void init() override;

        void draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) override;

        void destroy() override;

    protected:
        //顶点位置
        GLint uTextureLocation;
        //纹理位置
        GLint positionLocation;
        //纹理位置
        GLint textureLocation;
        //矩阵
        GLint matrixLocation;

        shared_ptr<Matrix> matrix;

        shared_ptr<ShapeContent> shapeContent;
    };
}

