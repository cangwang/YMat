//
// Created by Cangwang on 2024/5/9.
//

#include "RectDrawer.h"
#define LOG_TAG "RectDrawer"

RectDrawer::RectDrawer(shared_ptr<ShapeContent> shapeContent) {
    this->shapeContent = shapeContent;
}

void RectDrawer::init() {
    auto rectInfo = shapeContent->elements->rectInfo;
    if (rectInfo) {
        int width = rectInfo->size[0];
        int height = rectInfo->size[1];
        int offX = rectInfo->position[0];
        int offY = rectInfo->position[1];
        setAnchorOffXY(width * 0.5 - offX, height * 0.5 - offY);
        setOffXY(offX, offY);
        setShapeContent(shapeContent, width, height);
    }

    ElementDrawer::init();
}

int RectDrawer::drawShape(shared_ptr<FrameInfo> frameInfo, tgfx::Canvas *canvas) {
    YMLOGV("drawShape");
    if (!shapeContent ||! shapeContent->elements|| !shapeContent->elements->rectInfo) {
        return -1;
    }
    tgfx::Rect rect;
    auto rectInfo = shapeContent->elements->rectInfo;
    int roundness = 0;
    if (rectInfo) {
        if (!rectInfo->position.empty()) {
            if (!rectInfo->size.empty()) {
                rect.setXYWH(rectInfo->position[0], rectInfo->position[1],
                             rectInfo->size[0], rectInfo->size[1]);
            } else {
                rect.setWH(rectInfo->size[0], rectInfo->size[1]);
            }
        }
        roundness = rectInfo->roundness;
    }

    auto strokePaint = getStrokePaint(frameInfo->frameId, shapeContent->elements);
    auto fillPaint = getFillPaint(frameInfo->frameId, shapeContent->elements);
    if (roundness > 0) { //圆角矩形
        if (fillPaint) {
            canvas->drawRoundRect(rect, roundness, roundness, *fillPaint);
        }
        if (strokePaint) {
            canvas->drawRoundRect(rect, roundness, roundness, *strokePaint);
        }
    } else {
        if (fillPaint) {
            canvas->drawRect(rect, *fillPaint);
        }
        if (strokePaint) {
            canvas->drawRect(rect, *strokePaint);
        }
    }
    return 0;
}

void RectDrawer::destroy() {
    ElementDrawer::destroy();
}
