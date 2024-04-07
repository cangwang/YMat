//
// Created by Cangwang on 2024/4/3.
//
#pragma once

#include "yminfo.h"

/*
 * 使用parson是c库因为真的非常轻量
 */
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
        static shared_ptr<ShapeTransform> getShapeTransform(JSON_Object* transformObject);
        static void setTransformInfo(list<shared_ptr<TransformBean>> transform, JSON_Array *transformArray);
        static void setValue(vector<float>* value, JSON_Array *valueArray);

        static void setShapeContent(list<shared_ptr<ShapeContent>>* shapeContentList, JSON_Array *shapeContent);
        static void setElement(shared_ptr<Element> element, JSON_Object *elementObj);
    };
}