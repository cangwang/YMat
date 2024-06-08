//
// Created by asus on 2024/5/19.
//

#include "PathCpuDrawer.h"

PathCpuDrawer::~PathCpuDrawer() {

}

PathCpuDrawer::PathCpuDrawer(shared_ptr<ShapeContent> shapeContent) {
    this->shapeContent = shapeContent;
}

void PathCpuDrawer::init() {
    float cx = width * 0.5;
    float cy = height * 0.5;
    setAnchorOffXY(cx, cy);

    //更新transform
    setShapeContentOnly(shapeContent);

    ElementCpuDrawer::init();
}

void PathCpuDrawer::draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) {
    ElementCpuDrawer::draw(matrix, frameInfo);
    glFlush();
    glFinish();
}
