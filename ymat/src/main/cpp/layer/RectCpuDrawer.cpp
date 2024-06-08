//
// Created by asus on 2024/5/19.
//

#include "RectCpuDrawer.h"

RectCpuDrawer::~RectCpuDrawer() {

}

RectCpuDrawer::RectCpuDrawer(shared_ptr<ShapeContent> shapeContent) {
    this->shapeContent = shapeContent;
}

void RectCpuDrawer::init() {
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
    ElementCpuDrawer::init();
}
