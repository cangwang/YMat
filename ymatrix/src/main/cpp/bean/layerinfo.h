//
// Created by Cangwang on 2024/4/1.
//

#pragma once
#include <string>
#include <bean/transform.h>
#include <bean/textdocattr.h>
#include <bean/shadercontent.h>

using namespace std;
namespace ymat {
    class LayerInfo {
    public:
        string type; //text->TextLayer文字 precomposition->VectorLayer预处理 image->ImageLayer图片 preComposition->vector组合
        int id;
        string name;
        shared_ptr<TexDocAttr> textDocAttr;
        string content;
        shared_ptr<Transform> transform;
    };

    class ShaderInfo: public LayerInfo {
    public:
        list<ShaderContent> shaderContents;
        bool isTrackMatte;
        int width;
        int height;
        int inFrame;
        int outFrame;
    };

    class VideoInfo: public ShaderInfo {

    };

    class ImageInfo: public ShaderInfo {
        int fillMode;
    };

    class PreCompositionInfo : public ShaderInfo {

    };
}

