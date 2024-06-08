//
// Created by Cangwang on 2024/5/9.
//

#include "EllipseDrawer.h"

EllipseDrawer::EllipseDrawer(shared_ptr<ShapeContent> shapeContent) {
    this->shapeContent = shapeContent;
}

void EllipseDrawer::init() {
    auto ellipseInfo = shapeContent->elements->ellipseInfo;
    if (ellipseInfo) {
        int width = ellipseInfo->size[0];
        int height = ellipseInfo->size[1];
        int offX = ellipseInfo->position[0];
        int offY = ellipseInfo->position[1];
        setAnchorOffXY(width * 0.5 - offX, height * 0.5 - offY);
        setOffXY(offX, offY);
        setShapeContent(shapeContent, width, height);
    }
    ElementDrawer::init();
}

int EllipseDrawer::drawShape(shared_ptr<FrameInfo> frameInfo, tgfx::Canvas *canvas) {
    if (!shapeContent ||! shapeContent->elements|| !shapeContent->elements->ellipseInfo) {
        return -1;
    }
    tgfx::Rect rect;
    auto ellipseInfo = shapeContent->elements->ellipseInfo;
    int roundness = 0;
    if (ellipseInfo) {
        if (!ellipseInfo->position.empty()) {
            if (!ellipseInfo->size.empty()) {
                rect.setXYWH(ellipseInfo->position[0], ellipseInfo->position[1],
                             ellipseInfo->size[0], ellipseInfo->size[1]);
            } else {
                rect.setWH(ellipseInfo->size[0], ellipseInfo->size[1]);
            }
        }
    }

    auto strokePaint = getStrokePaint(frameInfo->frameId, shapeContent->elements);
    auto fillPaint = getFillPaint(frameInfo->frameId, shapeContent->elements);
    if (fillPaint) {
        canvas->drawOval(rect, *fillPaint);
    }
    if (strokePaint) {
        canvas->drawOval(rect, *strokePaint);
    }

    return 0;
}

void EllipseDrawer::destroy() {
    ElementDrawer::destroy();
}
