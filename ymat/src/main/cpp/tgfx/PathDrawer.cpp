//
// Created by Cangwang on 2024/5/9.
//

#include "PathDrawer.h"

#define LOG_TAG "PathDrawer"

PathDrawer::~PathDrawer() {
    destroy();
}

PathDrawer::PathDrawer(shared_ptr<ShapeContent> shapeContent) {
    this->shapeContent = shapeContent;
}

void PathDrawer::init() {
    points = shapeContent->elements->shapeInfo->points;
    if (points.empty()) {
        return;
    }

    float cx = width * 0.5;
    float cy = height * 0.5;
    setAnchorOffXY(cx, cy);

    //调整点偏移
    for (int i = 0; i < points.size(); ++i) {
        points[i][0] = cx + points[i][0];
        points[i][1] = cy + points[i][1];
    }
    //更新transform
    setShapeContentOnly(shapeContent);

    ElementDrawer::init();
}


int PathDrawer::drawShape(shared_ptr<FrameInfo> frameInfo, tgfx::Canvas *canvas) {
    YMLOGV("drawShape");
    if (shapeContent) {
        tgfx::Path path;
        int currentPoint = 0;
        for (const int &action: shapeContent->elements->shapeInfo->actions) {
            if (action == 0) {
                auto movePoint = points[currentPoint++];
                path.moveTo(movePoint[0], movePoint[1]);
            } else if (action == 1) {
                auto linePoint = points[currentPoint++];
                path.lineTo(linePoint[0], linePoint[1]);
            } else if (action == 2) {
                auto control1 = points[currentPoint++];
                auto control2 = points[currentPoint++];
                path.cubicTo(control1[0], control1[1], control2[0], control2[1],
                             points[currentPoint][0], points[currentPoint][1]);
            } else if (action == 3) {
                path.close();
            }
        }
        auto strokeInfo = shapeContent->elements->strokeInfo;
        if (strokeInfo && strokeInfo->dashesInfo) {
            setForceRefresh(true);
            size_t size = strokeInfo->dashesInfo->dash.size();
            auto dashList = new float[size];
            for (size_t i = 0; i < size; ++i) {
                dashList[i] = static_cast<float>(strokeInfo->dashesInfo->dash[i]);
            }
            auto offset = strokeInfo->dashesInfo->offset->getValue(frameInfo->frameId);
            auto dashEffect = tgfx::PathEffect::MakeDash(dashList, size, *offset);
            delete[] dashList;
            dashEffect->applyTo(&path);
        } else {
            setForceRefresh(false);
        }
        auto strokePaint = getStrokePaint(frameInfo->frameId, shapeContent->elements);
        auto fillPaint = getFillPaint(frameInfo->frameId, shapeContent->elements);
        if (fillPaint) {
            canvas->drawPath(path, *fillPaint);
        }
        if (strokeInfo) {
            canvas->drawPath(path, *strokePaint);
        }
    }
    return 0;
}

void PathDrawer::destroy() {
    points.clear();
    ElementDrawer::destroy();
}
