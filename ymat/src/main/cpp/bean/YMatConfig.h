//
// Created by Cangwang on 2024/4/3.
//
#pragma once

#include "yminfo.h"

/*
 * 使用parson是c库因为真的非常轻量
 */
#include <utils/parson.h>
#include <utils/ymlog.h>

using namespace std;
namespace ymat {
    class YMatConfig {
    public:
        YMatConfig();
        ~YMatConfig();

        static void parse(const char* json,const shared_ptr<YMInfo>& config);
        static void setColor(vector<float>* color, JSON_Array* colorArray);
        static shared_ptr<TexDocAttr> getTextDocAttr(JSON_Object* textDocAttrObject);
        static shared_ptr<SimpleLayerInfo> getLayer(JSON_Object* layerObject);
        static shared_ptr<Transform> getTransform(JSON_Object* transformObject);
        static void setTransformInfo(vector<shared_ptr<TransformBean>>* transform, JSON_Array *transformArray);
        static void setTransformInfo(vector<shared_ptr<TransformSimpleBean>>* transform, JSON_Array *transformArray);
        static void setValue(vector<float>* value, JSON_Array *valueArray);
        static void setValue(vector<int>* value, JSON_Array *valueArray);
        static void setValue(vector<vector<float>>* value, JSON_Array *valueArray);

        static void setShapeContent(vector<shared_ptr<ShapeContent>>* shapeContentList, JSON_Array *shapeContent);
        static void setElement(shared_ptr<Element> element, JSON_Object *elementObj);
    };
}