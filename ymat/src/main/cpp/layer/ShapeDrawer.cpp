//
// Created by Cangwang on 2024/5/9.
//

#include "ShapeDrawer.h"
#if USE_TGFX
#include <tgfx/RectDrawer.h>
#include <tgfx/ElementDrawer.h>
#include <tgfx/PathDrawer.h>
#include <tgfx/EllipseDrawer.h>
#endif

#include <layer/RectCpuDrawer.h>
#include <layer/EllipseCpuDrawer.h>
#include <layer/PathCpuDrawer.h>

#define LOG_TAG "ShapeDrawer"

void ShapeDrawer::init() {
    auto shapeInfo = dynamic_pointer_cast<ShapeLayerInfo>(info);
    if (!shapeInfo->shapeContents.empty()) {
        for (int i = shapeInfo->shapeContents.size() - 1; i >=0 ; i--) {
            auto shapeContent = shapeInfo->shapeContents[i];
            shared_ptr<Layers> layer;
            if (!useTGFX) {
                shapeContent->textureInfo = info->textureInfo;
            }
            if (shapeContent->type == RECT) {
                shared_ptr<AbstractDrawer> rect;
#if USE_TGFX
                if (!useTGFX) {
                    rect = make_shared<RectCpuDrawer>(shapeContent);
                } else {
                    rect = make_shared<RectDrawer>(shapeContent);
                }
#else
                rect = make_shared<RectCpuDrawer>(shapeContent);
#endif
                layer = make_shared<Layers>(rect);
            } else if (shapeContent->type == ELLIPSE) {
                shared_ptr<AbstractDrawer> ellipse;
#if USE_TGFX
                if (!useTGFX) {
                    ellipse = make_shared<EllipseCpuDrawer>(shapeContent);
                } else{
                    ellipse = make_shared<EllipseDrawer>(shapeContent);
                }
#else
                ellipse = make_shared<EllipseCpuDrawer>(shapeContent);
#endif
                layer = make_shared<Layers>(ellipse);
            } else if (shapeContent->type == PATH) {
                shared_ptr<AbstractDrawer> path;
#if USE_TGFX
                if (!useTGFX) {
                    path = make_shared<PathCpuDrawer>(shapeContent);
                } else {
                    path = make_shared<PathDrawer>(shapeContent);
                }
#else
                path = make_shared<PathCpuDrawer>(shapeContent);
#endif
                path->setLayerInfo(info);
                layer = make_shared<Layers>(path);
            }
            if (layer) {
                layer->init();
                if (layer->drawer) {
                    layer->drawer->setMatrixCache(shapeInfo->inFrame, shapeInfo->outFrame);
                }
                subLayers.push_back(layer);
            }
        }
    }
}

void ShapeDrawer::draw(shared_ptr<Matrix> parentMatrix, shared_ptr<FrameInfo> frameInfo) {
    YMLOGV("drawShape name %s", info->name.c_str());
    for (int i = 0; i < subLayers.size(); ++i) {
        subLayers[i]->render(parentMatrix, frameInfo);
    }
    //切换回默认的帧缓冲, 这里肯定tgfx有绑定未释放的问题，要手动释放
//    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void ShapeDrawer::destroy() {
    for (int i = 0; i < subLayers.size(); ++i) {
        subLayers[i]->destroy();
    }
    subLayers.clear();
}
