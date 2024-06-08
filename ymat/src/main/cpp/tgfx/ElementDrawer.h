//
// Created by Cangwang on 2024/5/9.
//

#pragma once

#include <layer/AbstractDrawer.h>
#include <include/tgfx/opengl/egl/EGLWindow.h>

namespace ymat {
    class ElementDrawer: public AbstractDrawer {
    public:
        ~ElementDrawer();

        void init() override;

        void draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) override;

        void destroy() override;

        virtual int drawShape(shared_ptr<FrameInfo> frameInfo, tgfx::Canvas *canvas) = 0;

        void setForceRefresh(bool forceRefresh);

        void initCanvas();

        void releaseCanvas();

        int shapeTextureId = -1;

    protected:
        int drawTGFX(shared_ptr<FrameInfo> frameInfo);

        std::shared_ptr<tgfx::EGLWindow> tgfxWindow = nullptr;

        shared_ptr<tgfx::Device> device;

        tgfx::Context* context;

        shared_ptr<tgfx::Surface> tgfxSurface;

        shared_ptr<ShapeContent> shapeContent;

        bool forceRefresh = false;

        shared_ptr<tgfx::Paint> getStrokePaint(int frame, shared_ptr<Element> elements);
        shared_ptr<tgfx::Paint> getFillPaint(int frame, shared_ptr<Element> elements);

        shared_ptr<FrameBuffer> frameBuffer;
    };
}

