//
// Created by Cangwang on 2024/4/1.
//

#include "rendercontroller.h"

ymat::RenderController::RenderController(): eglCore(make_shared<EGLCore>()) {

}

ymat::RenderController::~RenderController() {
    layers.clear();
}

GLuint ymat::RenderController::initRender(ANativeWindow *window) {
    if (eglCore && window){
        eglCore->start(window);
    }

    width = ANativeWindow_getWidth(window);
    height = ANativeWindow_getHeight(window);

    return 0;
}

void ymat::RenderController::renderFrame() {
    if (!layers.empty()) {
        for (shared_ptr<Layer> layer: layers) {
            layer->renderFrame();
        }
    }
}

void ymat::RenderController::renderSwapBuffers() {
    if (eglCore) {
        eglCore->swapBuffer();
    }
}

void ymat::RenderController::renderClearFrame() {

}

void ymat::RenderController::destroy() {
    if (!layers.empty()) {
        for (shared_ptr<Layer> layer: layers) {
            layer->release();
        }
    }
    if (eglCore) {
        eglCore->release();
    }
}
