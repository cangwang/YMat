//
// Created by Cangwang on 2024/5/9.
//

#include "ElementDrawer.h"
#define LOG_TAG "ElementDrawer"

ElementDrawer::~ElementDrawer() {
    releaseCanvas();
}

void ElementDrawer::init() {
    shaderProgram = ShaderUtil::createProgram(TextureLoadUtil::SIMPLE_VERTEX_SHADER, TextureLoadUtil::SIMPLE_FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "u_texture");
    positionLocation = glGetAttribLocation(shaderProgram, "a_position");
    textureLocation = glGetAttribLocation(shaderProgram, "a_texcoord");
    matrixLocation = glGetUniformLocation(shaderProgram,"u_matrix");

//    shapeTextureId = -1;
//    shared_ptr<FrameInfo> frame = make_shared<FrameInfo>();
//    frame->frameId = 0;
//    shapeTextureId = drawTGFX(frame);
    initCanvas();
}

void ElementDrawer::initCanvas() {
    //对比如果，矩阵值相同，就认为没有变化
    if (tgfxWindow == nullptr) {
        tgfxWindow = tgfx::EGLWindow::Current();
    }
    if (tgfxWindow == nullptr) {
        return;
    }
    device = tgfxWindow->getDevice();
    context = device->lockContext();
    if (context == nullptr) {
        return;
    }

//    if (!frameBuffer) {
//        frameBuffer = make_shared<FrameBuffer>();
//        frameBuffer->init(width, height);
//    }
//
//    tgfx::GLFrameBufferInfo glFrameBufferInfo;
//    glFrameBufferInfo.id = frameBuffer->buffer;
//    tgfx::BackendRenderTarget backendRenderTarget(glFrameBufferInfo, width, height);
//    tgfxSurface = tgfx::Surface::MakeFrom(context, backendRenderTarget,
//                                               tgfx::ImageOrigin::TopLeft);
    tgfxSurface = tgfx::Surface::Make(context, width, height);
    if (tgfxSurface == nullptr) {
        device->unlock();
        return;
    }
    device->unlock();
}

void ElementDrawer::releaseCanvas() {
    device->unlock();
    context = nullptr;
}

void ElementDrawer::draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) {
    frameInfo->framebuffer->unbind();
    if (forceRefresh || shapeTextureId == -1) { //是否没有创建纹理，或者强制刷新
        shapeTextureId = drawTGFX(frameInfo);
    }
    //因为绘制形状需要解绑，所以需要重新切换回原来的FrameBuffer
    frameInfo->framebuffer->bind();
    if (shapeTextureId != 0) {
        simpleDraw(matrix, shapeTextureId);
    }
    frameInfo->framebuffer->unbind();
}

int ElementDrawer::drawTGFX(shared_ptr<FrameInfo> frameInfo) {
    device->lockContext();
    auto canvas = tgfxSurface->getCanvas();
    canvas->clear();
    canvas->save();
    //实体类绘制
    drawShape(frameInfo, canvas);
    canvas->restore();
    tgfxSurface->flush();
//        context->submit();
//        tgfxWindow->present(context);
    tgfx::GLTextureInfo textureInfo;
    tgfxSurface->getBackendTexture().getGLTextureInfo(&textureInfo);
    device->unlock();
    //切换回默认的帧缓冲, 这里肯定tgfx有绑定未释放的问题，要手动释放
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    YMLOGV("textureId %d", textureInfo.id);
    return textureInfo.id;
}

void ElementDrawer::destroy() {
    if (info && info->textureInfo) {
        glDeleteTextures(1, &info->textureInfo->textureId);
    }
}


shared_ptr<tgfx::Paint> ElementDrawer::getStrokePaint(int frame, shared_ptr<Element> elements) {
    auto strokeInfo = elements->strokeInfo;
    shared_ptr<tgfx::Paint> paint = nullptr;
    if (strokeInfo) {
        paint = make_shared<tgfx::Paint>();
        paint->setStyle(tgfx::PaintStyle::Stroke);
        paint->setStrokeWidth(strokeInfo->width);
        if (strokeInfo->lineCap == 0) {
            paint->setLineCap(tgfx::LineCap::Butt);
        } else if (strokeInfo->lineCap == 1) {
            paint->setLineCap(tgfx::LineCap::Round);
        } else if (strokeInfo->lineCap == 2) {
            paint->setLineCap(tgfx::LineCap::Square);
        }
        if (strokeInfo->lineJoin == 0) {
            paint->setLineJoin(tgfx::LineJoin::Miter);
        } else if (strokeInfo->lineJoin == 1) {
            paint->setLineJoin(tgfx::LineJoin::Round);
        } else if (strokeInfo->lineJoin == 2) {
            paint->setLineJoin(tgfx::LineJoin::Bevel);
        }
        paint->setMiterLimit(strokeInfo->miterLimit);

//                paint.setBlendMode(strokeInfo->blendMode);
        if (!strokeInfo->color.empty()) {
            auto color = tgfx::Color{strokeInfo->color[0], strokeInfo->color[1],
                                     strokeInfo->color[2],
                                     strokeInfo->color[3]};
            paint->setColor(color);
        }
    }
    return paint;
}

shared_ptr<tgfx::Paint> ElementDrawer::getFillPaint(int frame, shared_ptr<Element> elements) {

    auto fillInfo = elements->fillInfo;
    shared_ptr<tgfx::Paint> paint = nullptr;
    if (fillInfo) {
        paint = make_shared<tgfx::Paint>();
        paint->setStyle(tgfx::PaintStyle::Fill);
        if (!fillInfo->color.empty()) {
            auto color = tgfx::Color{fillInfo->color[0], fillInfo->color[1],
                                     fillInfo->color[2],
                                     fillInfo->color[3]};
            paint->setColor(color);
        }
        paint->setAlpha(fillInfo->opacity * 0.01f);
    }
    return paint;
}

void ElementDrawer::setForceRefresh(bool forceRefresh) {
    this->forceRefresh = forceRefresh;
}
