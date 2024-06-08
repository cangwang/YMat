//
// Created by Cangwang on 2024/4/1.
//

#include "rendercontroller.h"
#define LOG_TAG "YMat-RenderController"

ymat::RenderController::RenderController(bool useTGFX):
    eglCore(make_shared<EGLCore>()),
    camera(make_shared<Matrix>()),
    useTGFX(useTGFX) {
}

ymat::RenderController::~RenderController() {
    targetLayers.clear();
    bgColor.clear();
}

GLuint ymat::RenderController::initRender(ANativeWindow *window) {
    if (eglCore && window){
        eglCore->start(window);
    }

    if (window) {
        updateViewPoint(ANativeWindow_getWidth(window), ANativeWindow_getHeight(window));
    } else {
        YMLOGE("window is null");
    }

    initImageProgram();
    return 0;
}

void RenderController::updateViewPoint(int width, int height) {
    this->surfaceWidth = (float) width;
    this->surfaceHeight = (float)height;
}

void RenderController::setCamera(int width, int height) {
    this->width = (float)width;
    this->height = (float)height;

    frameInfo = make_shared<FrameInfo>();
    frameInfo->width = width;
    frameInfo->height = height;
    frameInfo->opacity = 1.0;
    frameInfo->framebuffer = make_shared<FrameBuffer>();
    //framebuffer默认为camera大小
    frameInfo->framebuffer->init(width, height);

    // 用资源宽高， 创建camera matrix，40是AE默认
    Matrix::perspectiveCamera(*camera, (float) width, (float) height, 40);

    //计算正交投影矩阵，修正变形
    orthoMmatrix = make_shared<Matrix>();
    setFillMode(FillMode::CenterFit);
}

void RenderController::setLayers(vector<shared_ptr<Comp>> comps, vector<shared_ptr<SimpleLayerInfo>> infos) {
    destroyLayer();
    if (!infos.empty()) {
        for (const shared_ptr<SimpleLayerInfo>& i : infos) {
            auto layer = setLayer(comps, i->id, i->type, i->isTrackMatte, false);
            if (!i->isTrackMatte && i->trackMatteLayer > 0 && i->trackMatteType > 0) { //是否被遮罩
                auto matteInfo = findMaskInfo(infos, i->trackMatteLayer);
                auto matteLayer = setLayer(comps, matteInfo->id, matteInfo->type, matteInfo->isTrackMatte, true);
                matteLayer->drawer->setLayerInfo(matteInfo);
                matteLayer->drawer->setUseTGFX(useTGFX);
                layer->setMaskLayer(matteLayer, i->trackMatteType);
                matteLayer->drawer->setMatrixCache(matteInfo->inFrame, matteInfo->outFrame);
            }
            if (layer && layer->drawer) {
                layer->setBlendMode(i->blendMode);
                layer->drawer->setLayerInfo(i);
//                layer->setParentWidthHeight(width, height);
                layer->init();
                layer->drawer->setMatrixCache(i->inFrame, i->outFrame);
                targetLayers.push_back(layer);
            }
        }
    }
}

shared_ptr<Layers> RenderController::setLayer(vector<shared_ptr<Comp>> comps, int id, string type, bool isTrackMatte, bool findTrack) {
    if (isTrackMatte && !findTrack) {
        YMLOGV("layerId %d type %s, is mask", id, type.c_str());
        return nullptr;
    }
    YMLOGV("setLayers layer %s", type.c_str());
    shared_ptr<Layers> layer;
    shared_ptr<AbstractDrawer> drawer;
    if (type == getLayerType(LayerType::Text)) {
#if USE_TGFX
        if (!useTGFX) {
            drawer = make_shared<TextCpuDrawer>();
        } else {
            drawer = make_shared<TextDrawer>();
        }
#else
        drawer = make_shared<TextCpuDrawer>();
#endif
    } else if (type == getLayerType(LayerType::Image)) {
        drawer = make_shared<ImageDrawer>();
    } else if (type == getLayerType(LayerType::Video)) {
        drawer = make_shared<VideoDrawer>();
    } else if (type == getLayerType(LayerType::PreComposition)) {
        for (shared_ptr<Comp> comp : comps) {
            if (comp->id == id) {
                 layer = setLayer(comps, comp->id, comp->type, comp->isTrackMatte, findTrack);
                return layer;
            }
        }
    } else if (type == getLayerType(LayerType::Shape)) {
        if (!isTrackMatte || findTrack) {
            drawer = make_shared<ShapeDrawer>();
        } else {
            YMLOGE("setLayers %d is mask shape", id);
        }
//        drawer = make_shared<ShapeDrawer>();
    } else if (type == getLayerType(LayerType::Vector)) {
        drawer = make_shared<VectorDrawer>();
        for (shared_ptr<Comp> comp : comps) {
            if (comp->id == id) {
                drawer->setUseTGFX(useTGFX);
                dynamic_pointer_cast<VectorDrawer>(drawer)->setLayers(comps, comp->layers);
                break;
            }
        }
    } else {
        YMLOGE("setLayers %s error", type.c_str());
    }
    if (drawer) {
        drawer->setUseTGFX(useTGFX);
        layer = make_shared<Layers>(drawer);
    }
    return layer;
}

shared_ptr<SimpleLayerInfo> RenderController::findMaskInfo(
        vector<shared_ptr<SimpleLayerInfo>> infos,
        int trackMatteLayer) {
    for (shared_ptr<SimpleLayerInfo> i: infos) {
        if (i->id == trackMatteLayer) {
            return i;
        }
    }
    return nullptr;
}

void RenderController::setFillMode(int fillMode) {
    this->fillMode = fillMode;
    if (width > 0 && height > 0 && surfaceWidth > 0, surfaceHeight > 0) {
        setFillCoord(width, height, surfaceWidth, surfaceHeight, false, fillMode);
    }
//    Matrix::setFillMode(*orthoMmatrix, fillMode, width, height, surfaceWidth, surfaceHeight);
}

void RenderController::setBgColor(vector<float> bgColor) {
    this->bgColor = bgColor;
}

bool RenderController::readyRender(int frameIndex) {
    return true;
}

void ymat::RenderController::renderFrame(int frameIndex) {
    if (!targetLayers.empty()) {
        //切换视口为资源大小
        glViewport(0, 0, width, height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        frameInfo->framebuffer->clear();
        frameInfo->frameId = frameIndex;

//        targetLayers[3]->render(camera, frameInfo);
//        targetLayers[2]->render(camera, frameInfo);
//        targetLayers[1]->render(camera, frameInfo);
//        targetLayers[0]->render(camera, frameInfo);
        for (int i = targetLayers.size() - 1; i >= 0; i--) { //AE层级倒序
//            YMLOGV("renderFrame layer %d", i);
            try {
                targetLayers[i]->render(camera, frameInfo);
            } catch (std::exception &e) {
                YMLOGE("renderFrame %s", e.what());
            }
        }
        //切换视口为渲染窗口大小
        glViewport(0, 0, surfaceWidth, surfaceHeight);
        if (bgColor.size() == 3) {
            glClearColor(bgColor[0], bgColor[1], bgColor[2], 0);
        } else {
            glClearColor(0, 0, 0, 0);
        }
        glClear(GL_COLOR_BUFFER_BIT);

        // 着色器
        glUseProgram(shaderProgram);
        // 顶点
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, textureVertices);

        // 矩阵设置
        glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, orthoMmatrix->values);

        //原纹理
        glActiveTexture(GL_TEXTURE0);
        YMLOGV("renderFrame framebuffer texture %d", frameInfo->framebuffer->texture);
        glBindTexture(GL_TEXTURE_2D, frameInfo->framebuffer->texture);
        // 纹理坐标
        glUniform1i(uTextureLocation, 0);

        glEnableVertexAttribArray(textureLocation);
        glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, texture);
        //绘制
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void ymat::RenderController::renderSwapBuffers(int frameIndex) {
    if (eglCore) {
        eglCore->swapBuffer();
    }
}

void ymat::RenderController::renderClearFrame() {
    if (eglCore) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        eglCore->swapBuffer();
    }
}

void ymat::RenderController::releaseTexture() {

}

void ymat::RenderController::releaseVideoTextureId(int playerId, int videoId, int vectorId,
                                                   int videoTextureId) {
//    if (!targetLayers.empty()) {
//        for (shared_ptr<Layers> layer: targetLayers) {
//            auto info = layer->getLayerInfo();
//            if (info->id == videoId) {
//                layer->release();
//            } else if (info->type == "precomposition") {
//                layer->releaseVideoTextureId(videoId, vectorId, videoTextureId);
//            }
//        }
//    }
}

void ymat::RenderController::destroyRender() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    if (eglCore) {
        eglCore->release();
    }
    glDeleteProgram(shaderProgram);
}

void ymat::RenderController::destroyLayer() {
    if (!targetLayers.empty()) {
        for (shared_ptr<Layers> layer: targetLayers) {
            layer->destroy();
        }
        targetLayers.clear();
    }
}

void RenderController::initImageProgram() {
    shaderProgram = ShaderUtil::createProgram(TextureLoadUtil::SIMPLE_VERTEX_SHADER, TextureLoadUtil::SIMPLE_FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "u_texture");
    positionLocation = glGetAttribLocation(shaderProgram, "a_position");
    textureLocation = glGetAttribLocation(shaderProgram, "a_texcoord");
    matrixLocation = glGetUniformLocation(shaderProgram,"u_matrix");
}

void RenderController::setParams(int playerId, int fillMode) {
    this->fillMode = fillMode;
    setFillMode(fillMode);
}

/**
 * 处理纹理拉伸
 * @param texW
 * @param texH
 * @param dstW
 * @param dstH
 * @param isAlpha
 * @param fillMode
 */
void RenderController::setFillCoord(float texW, float texH,
                               float dstW, float dstH,
                               bool isAlpha, int fillMode) {
    float lx = 0;
    float ly = 0;
    float rx = isAlpha ? 0.5 : 1.0;
    float ry = 1.0;
    float sw = 1.0;
    float sh = 1.0;

    float srcW = isAlpha ? texW * 0.5f : texW;
    float srcH = texH;
    if (fillMode == FillMode::AspectFill) {
        float srcWhr = srcW / srcH;
        float dstWhr = dstW / dstH;
        bool isLead = dstWhr < srcWhr;
        float tw = isLead ? dstH * srcWhr : dstW;
        float th = isLead ? dstH : dstW / srcWhr;
        lx = (dstW - tw) * 0.5f;
        ly = (dstH - th) * 0.5f;
        lx = -lx / texW;
        ly = -ly / th;
        rx = rx - lx;
        ry = ry - ly;
    } else if (fillMode == FillMode::FitXY) {
        // 默认拉伸
    } else {
        float srcWhr = srcW / srcH;
        float dstWhr = dstW / dstH;
        bool isLead = dstWhr < srcWhr;
        float tw = isLead ? dstW : dstH * srcWhr;
        float th = isLead ? dstW / srcWhr : dstH;
        sw = tw / dstW;
        sh = th / dstH;
    }
    float x1 = -sw;
    float y1 = -sh;
    float x2 = sw;
    float y2 = sh;

    textureVertices[0] = x1;
    textureVertices[1] = y1;
    textureVertices[2] = x2;
    textureVertices[3] = y1;
    textureVertices[4] = x1;
    textureVertices[5] = y2;
    textureVertices[6] = x2;
    textureVertices[7] = y2;

    texture[0] = lx;
    texture[1] = ly;
    texture[2] = rx;
    texture[3] = ly;
    texture[4] = lx;
    texture[5] = ry;
    texture[6] = rx;
    texture[7] = ry;
}
