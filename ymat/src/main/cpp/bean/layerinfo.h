//
// Created by Cangwang on 2024/4/1.
//

#pragma once

#include <string>
#include <bean/transform.h>
#include <bean/textdocattr.h>
#include <bean/shapecontent.h>
#include <utils/matrix.h>
#include <bean/TextureInfo.h>
#include <utils/fillmode.h>

using namespace std;
namespace ymat {

    class LayerInfo {
    public:
        virtual void init() = 0;
    };

    class SimpleLayerInfo : public LayerInfo {
    public:
        ~SimpleLayerInfo() {
            type.clear();
            name.clear();
            content.clear();
            values.clear();
        }

        void init() {

        };
        string type; //text->TextLayer文字 precomposition->PrecompositionLayer预处理 image->ImageLayer图片
        int id;
        string name;
        string content;
        string contentStr;
        shared_ptr<Transform> transform;
        bool isTrackMatte; //是否蒙版 true遮罩层，false被遮罩层
        int trackMatteLayer; //isTrackMatte false时存在值， 蒙版对应图层layer id
        int trackMatteType; // 1透明， 2透明反色值， 3明亮度， 4明亮度饭色值
        int blendMode;  //混合模式
        int width;
        int height;
        int inFrame;
        int outFrame;
        bool isAlpha;  //透明
        int fillMode = FillMode::FitXY;  // 0 默认是等比缩放留空, 1 等比铺满裁剪, 2 拉伸铺满
        shared_ptr<TextureInfo> textureInfo; //纹理id
        vector<float> values;

        /**
         * 世界坐标转换
         * @param frameIndex
         * @param matrix
         */
        shared_ptr<Matrix> getMatrix(int frameIndex) {
            int index = frameIndex;
            if (frameIndex > outFrame) {
                index = outFrame;
            }
            shared_ptr<Matrix> m = make_shared<Matrix>();
            if (transform->position) {
                auto position = transform->position->getValue(index);
//                matrix = new Matrix();
                Matrix::translation(*m, position[0], -position[1], -position[2]);
            }

            if (transform->rotationX) {
                auto rotationX = transform->rotationX->getValue(index);
                if (rotationX) {
                    m->xRotate(degToRad(*rotationX));
                }
            }

            if (transform->rotationY) {
                auto rotationY = transform->rotationY->getValue(index);
                if (rotationY) {
                    m->yRotate(degToRad(360 - *rotationY));
                }
            }

            if (transform->rotationZ) {
                auto rotationZ = transform->rotationZ->getValue(index);
                if (rotationZ) {
                    m->zRotate(degToRad(360 - *rotationZ));
                }
            }

            if (transform->orientation) {
                auto orientation = transform->orientation->getValue(index);
                if (orientation.size() == 3) {
                    if (orientation[0]) {
                        m->xRotate(degToRad(orientation[0]));
                    }
                    if (orientation[1]) {
                        m->yRotate(degToRad(360 - orientation[1]));
                    }
                    if (orientation[2]) {
                        m->zRotate(degToRad(360 - orientation[2]));
                    }
                }
            }

            if (transform->scale) {
                auto scale = transform->scale->getValue(index);
                //不明白
                if (scale.size() == 3) {
                    float x = scale.size() > 0 ? scale[0] * 0.01 : 1.0f;
                    float y = scale.size() > 1 ? scale[1] * 0.01 : 1.0f;
                    float z = scale.size() > 2 ? scale[2] * 0.01 : 1.0f;
                    m->scale(x, y, z);
                }
            }

            if (transform->anchorPoint) {
                auto anchorPoint = transform->anchorPoint->getValue(index);
                shared_ptr<Matrix> moveOrighMatrix = make_shared<Matrix>();
                Matrix::translation(*moveOrighMatrix, -anchorPoint[0], anchorPoint[1], 0);
                Matrix::multiply(*m, *moveOrighMatrix, *m);
            }
            return m;
        }
    };

    class TextLayerInfo : public SimpleLayerInfo {
    public:
        shared_ptr<TexDocAttr> textDocAttr;

        void init() {}
    };

    class ShapeLayerInfo : public SimpleLayerInfo {
    public:
        ~ShapeLayerInfo() {
            shapeContents.clear();
        }

        vector<shared_ptr<ShapeContent>> shapeContents;

        void init() {}
    };

    class VideoLayerInfo : public SimpleLayerInfo {
    public:

        void init() {}
    };

    class ImageLayerInfo : public SimpleLayerInfo {
    public:
        void init() {}
    };

    class PreCompositionLayerInfo : public SimpleLayerInfo {
    public:
        void init() {}
    };
}

