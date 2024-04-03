//
// Created by Cangwang on 2024/4/3.
//
#pragma once

#include "yminfo.h"

#include <utils/parson.h>

using namespace std;
namespace ymat {
    class YMatConfig {
    public:
        YMatConfig();
        ~YMatConfig();

        static shared_ptr<YMInfo> parse(const char* json);
        static void setColor(vector<float>* color, JSON_Array* colorArray);
        static shared_ptr<TexDocAttr> getTextDocAttr(JSON_Object* textDocAttrObject);
        static shared_ptr<LayerInfo> getLayer(JSON_Object* layerObject);
        static shared_ptr<Transform> getTransform(JSON_Object* transformObject);
        static void setTransformInfo(shared_ptr<list<shared_ptr<TransformBean>>> transform, JSON_Array *transformArray);
        static void setValue(vector<float>* value, JSON_Array *valueArray);
    };
}