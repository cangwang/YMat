//
// Created by Cangwang on 2024/4/1.
//

#pragma once
#include <string>
#include <bean/transform.h>
#include <bean/textdocattr.h>
#include <bean/shapecontent.h>
#include <list>

using namespace std;
namespace ymat {

    class LayerInfo {
    public:
        virtual void init() = 0;
    };

    class SimpleLayerInfo: public LayerInfo {
    public:
        void init() {};
        string type; //text->TextLayer文字 precomposition->VectorLayer预处理 image->ImageLayer图片 preComposition->vector组合
        int id;
        string name;
        string content;
        shared_ptr<Transform> transform;
        bool isTrackMatte; //是否蒙版
        int trackMatteLayer; //isTrackMatte true时存在， 蒙版对应图层layer id
        int trackMatteType; // 1透明， 2透明反色值， 3明亮度， 4明亮度饭色值
        int width;
        int height;
        int inFrame;
        int outFrame;
        bool isAlpha;  //透明
    };

    class TextLayerInfo: public SimpleLayerInfo {
    public:
        shared_ptr<TexDocAttr> textDocAttr;
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
        int fillMode;  // 0 默认是等比缩放留空, 1 等比铺满裁剪, 2 拉伸铺满
        void init(){}
    };

    class PreCompositionLayerInfo : public SimpleLayerInfo {
    public:
        void init(){}
    };
}

