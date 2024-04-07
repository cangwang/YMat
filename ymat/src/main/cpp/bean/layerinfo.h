//
// Created by Cangwang on 2024/4/1.
//

#pragma once
#include <string>
#include <bean/transform.h>
#include <bean/textdocattr.h>
#include <bean/shapecontent.h>

using namespace std;
namespace ymat {

    class Layer {
    public:
        virtual void init() = 0;
    };

    class LayerInfo: public Layer {
    public:
        void init() {};
        string type; //text->TextLayer文字 precomposition->VectorLayer预处理 image->ImageLayer图片 preComposition->vector组合
        int id;
        string name;
        shared_ptr<TexDocAttr> textDocAttr;
        string content;
        shared_ptr<Transform> transform;
    };

    class TextLayerInfo: public LayerInfo {
    public:
        void init(){}
    };

    class SimpleLayerInfo: public LayerInfo {
    public:
        bool isTrackMatte;
        int width;
        int height;
        int inFrame;
        int outFrame;
        void init(){}
    };

    class ShadeLayerInfo: public SimpleLayerInfo {
    public:
        ~ShadeLayerInfo() {
            shapeContents.clear();
        }
        list<shared_ptr<ShapeContent>> shapeContents;
        void init(){}
    };

    class VideoLayerInfo: public SimpleLayerInfo {
    public:
        void init(){}
    };

    class ImageLayerInfo: public SimpleLayerInfo {
    public:
        int fillMode;
        void init(){}
    };

    class PreCompositionLayerInfo : public SimpleLayerInfo {
    public:
        void init(){}
    };
}
