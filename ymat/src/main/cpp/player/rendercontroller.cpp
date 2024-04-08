//
// Created by Cangwang on 2024/4/1.
//

#include "rendercontroller.h"

ymatrix::RenderController::RenderController(): eglCore(nullptr) {

}

ymatrix::RenderController::~RenderController() {

}

void ymatrix::RenderController::renderFrame() {

}

void ymatrix::RenderController::renderSwapBuffers() {
    if (eglCore) {
        eglCore->swapBuffer();
    }
}

void ymatrix::RenderController::renderClearFrame() {

}

void ymatrix::RenderController::destroy() {

}
