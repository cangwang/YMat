//
// Created by Cangwang on 2024/5/9.
//

#include "VectorDrawer.h"
#define LOG_TAG "VectorDrawer"

VectorDrawer::~VectorDrawer() {
    destroy();
}

void VectorDrawer::init() {
    initImageProgram();
}

void VectorDrawer::draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) {
    YMLOGV("drawShape name %s", info->name.c_str());
    if (transform) {
        float opacity = transform->getOpacity(frameInfo->frameId);  //如果存在透明度需要framebuffer来做调整

        if (!viewMatrix) {
            viewMatrix = make_shared<Matrix>();
        }

        if (viewFrameInfo->width != frameInfo->width || viewFrameInfo->height != frameInfo->height) {
            Matrix::perspectiveCamera(*viewMatrix, frameInfo->width, frameInfo->height, 40);
        }

        viewFrameInfo->frames = frameInfo->frames;
        viewFrameInfo->frameId = frameInfo->frameId;
        viewFrameInfo->width = frameInfo->width;
        viewFrameInfo->height = frameInfo->height;
        viewFrameInfo->opacity = opacity;

        if (!viewFrameInfo->framebuffer) {
            auto viewFrameBuffer = make_shared<FrameBuffer>();
//          viewFrameBuffer->init(frameInfo->width, frameInfo->height);
            viewFrameBuffer->init(width, height);
            viewFrameInfo->framebuffer = viewFrameBuffer;
        } else if (viewFrameInfo->framebuffer->w != width || viewFrameInfo->framebuffer->h != height) {
            auto viewFrameBuffer = make_shared<FrameBuffer>();
//              viewFrameBuffer->init(frameInfo->width, frameInfo->height);
            viewFrameBuffer->init(width, height);
            viewFrameInfo->framebuffer->destroy();
            viewFrameInfo->framebuffer = viewFrameBuffer;
        } else {
            viewFrameInfo->framebuffer->clear();
        }

        for (int i = subLayers.size() - 1; i >= 0; i--) { //AE层级倒序
            subLayers[i]->render(viewMatrix, viewFrameInfo);
        }

//        for (int i = 0; i < subLayers.size(); i++) { //AE层级倒序
//            subLayers[i]->render(viewMatrix, viewFrameInfo);
//        }
//        if (subLayers.size() > 4) {
//            subLayers[4]->render(viewMatrix, viewFrameInfo);
//            subLayers[3]->render(viewMatrix, viewFrameInfo);
//            subLayers[2]->render(viewMatrix, viewFrameInfo);
//            subLayers[0]->render(viewMatrix, viewFrameInfo);
//        }

        YMLOGV("opacity %f use viewFrameBuffer", opacity);

        auto parentFramebuffer = frameInfo->framebuffer;

        parentFramebuffer->bind();

        updateImageFramebufferVertices(frameInfo->width, frameInfo->height);

        simpleDraw(matrix, viewFrameInfo->framebuffer->texture, false, opacity);

        parentFramebuffer->unbind();

//        for (int i = subLayers.size() - 1; i >= 0; i--) { //AE层级倒序
//            subLayers[i]->render(matrix, frameInfo);
//        }
    }
}

void VectorDrawer::setLayers(vector<shared_ptr<Comp>> comps, vector<shared_ptr<SimpleLayerInfo>> infos) {
    destroy();
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
                subLayers.push_back(layer);
            }
        }
    }
}

shared_ptr<Layers> VectorDrawer::setLayer(vector<shared_ptr<Comp>> comps, int id, string type, bool isTrackMatte, bool findTrack) {
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
//                layer->setComp(comp);
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

shared_ptr<SimpleLayerInfo> VectorDrawer::findMaskInfo(
        vector<shared_ptr<SimpleLayerInfo>> infos,
        int trackMatteLayer) {
    for (shared_ptr<SimpleLayerInfo> i: infos) {
        if (i->id == trackMatteLayer) {
            return i;
        }
    }
    return nullptr;
}

void VectorDrawer::destroy() {
    if (!subLayers.empty()) {
        for (shared_ptr<Layers> layer : subLayers) {
            layer->destroy();
        }
        subLayers.clear();
    }
    if (viewFrameInfo && viewFrameInfo->framebuffer) {
        viewFrameInfo->framebuffer->destroy();
    }
    glDeleteProgram(shaderProgram);
}


void VectorDrawer::initImageProgram() {
    shaderProgram = ShaderUtil::createProgram(TextureLoadUtil::SIMPLE_VERTEX_SHADER, TextureLoadUtil::IMAGE_FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "u_texture");
    positionLocation = glGetAttribLocation(shaderProgram, "a_position");
    textureLocation = glGetAttribLocation(shaderProgram, "a_texcoord");
    matrixLocation = glGetUniformLocation(shaderProgram,"u_matrix");
    isAlpha = glGetUniformLocation(shaderProgram, "u_isAlpha");
    opacity = glGetUniformLocation(shaderProgram, "u_opacity");
}
