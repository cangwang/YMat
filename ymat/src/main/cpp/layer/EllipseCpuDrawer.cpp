//
// Created by asus on 2024/5/19.
//

#include "EllipseCpuDrawer.h"

EllipseCpuDrawer::~EllipseCpuDrawer() {

}

EllipseCpuDrawer::EllipseCpuDrawer(shared_ptr<ShapeContent> shapeContent) {
    this->shapeContent = shapeContent;
}

void EllipseCpuDrawer::init() {
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
    ElementCpuDrawer::init();
}
