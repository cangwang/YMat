//
// Created by Cangwang on 2024/4/3.
//

#include "YMatConfig.h"


#define LOG_TAG "YMatConfig"
#define YMLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define YMLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

YMatConfig::YMatConfig() {

}

YMatConfig::~YMatConfig() {

}

shared_ptr<YMInfo> YMatConfig::parse(const char *json) {
    if (json == nullptr) return nullptr;

    JSON_Value *root_value = json_parse_string(json);
    // 获取根类型 object/array
    JSON_Value_Type jsonValueType = json_value_get_type(root_value);
    if (jsonValueType == JSONObject) {
        auto config = make_shared<YMInfo>();

        JSON_Object *jsonObject = json_value_get_object(root_value);

        //解析
//        JSON_Array* bgColor = json_object_get_array(jsonObject, "bgColor");
//        if (bgColor) {
//            auto bgColorSize = json_array_get_count(bgColor);
//            for (int i = 0; i < bgColorSize; ++i) {
//                config->bgColor.push_back(json_array_get_number(bgColor, i));
//            }
//        }
        setColor(&config->bgColor, json_object_get_array(jsonObject, "bgColor"));

        config->width = (int) json_object_get_number(jsonObject, "width");

        config->height = (int) json_object_get_number(jsonObject, "height");

        config->duration = (int) json_object_get_number(jsonObject, "duration");

        config->frameRate = (int) json_object_get_number(jsonObject, "frameRate");

        //解析targetComp
        JSON_Object *targetCompObject = json_object_get_object(jsonObject, "targetComp");

        if (targetCompObject != nullptr) {
            auto targetComp = make_shared<TargetComp>();
            JSON_Object* layersObject = json_object_get_object(targetCompObject, "layers");
            JSON_Array* layers = json_object_get_array(layersObject,"layers");
            if (layers) {
                auto layersSize = json_array_get_count(layers);
                for (int i = 0; i < layersSize; ++i) {
                    JSON_Object* layerObject= json_array_get_object(layers, i);
                    if (layerObject) {
                        targetComp->layers->push_back(getLayer(layerObject));
                    }
                }
            }
        }
    }
    return nullptr;
}


void YMatConfig::setColor(vector<float>* color, JSON_Array *colorArray) {
    if (colorArray) {
        auto bgColorSize = json_array_get_count(colorArray);
        for (int i = 0; i < bgColorSize; ++i) {
            color->push_back((float)json_array_get_number(colorArray, i));
        }
    }
}

shared_ptr<TexDocAttr> YMatConfig::getTextDocAttr(JSON_Object *textDocAttrObject) {
    auto textDocAttr = make_shared<TexDocAttr>();
    textDocAttr->text = json_object_get_string(textDocAttrObject, "text");
    setColor(&textDocAttr->textColor, json_object_get_array(textDocAttrObject, "textColor"));
    textDocAttr->font = json_object_get_string(textDocAttrObject, "font");
    textDocAttr->fontFamily = json_object_get_string(textDocAttrObject, "fontFamily");
    textDocAttr->fontSize = (int)json_object_get_number(textDocAttrObject, "fontSize");
    textDocAttr->fontStyle = json_object_get_string(textDocAttrObject, "fontStyle");
    textDocAttr->fauxBold = (int)json_object_get_boolean(textDocAttrObject, "fauxBold");
    textDocAttr->fauxItalic = (int)json_object_get_boolean(textDocAttrObject, "fauxItalic");
    textDocAttr->lineSpacing = (float)json_object_get_number(textDocAttrObject, "lineSpacing");
    textDocAttr->wordSpacing = (int)json_object_get_number(textDocAttrObject, "wordSpacing");
    textDocAttr->textAligment = (int)json_object_get_number(textDocAttrObject, "textAligment");
    textDocAttr->orientation = (int)json_object_get_number(textDocAttrObject, "orientation");
    return textDocAttr;
}

shared_ptr<LayerInfo> YMatConfig::getLayer(JSON_Object *layerObject) {
    auto layerInfo = make_shared<LayerInfo>();
    layerInfo->type = json_object_get_string(layerObject, "type");
    layerInfo->id = (int) json_object_get_number(layerObject, "id");
    layerInfo->name = json_object_get_string(layerObject, "name");
    JSON_Object* textDocAttrObject = json_object_get_object(layerObject, "textDocAttr");
    if (textDocAttrObject) {
        layerInfo->textDocAttr = getTextDocAttr(textDocAttrObject);
    }
    layerInfo->content = json_object_get_string(layerObject, "content");
    JSON_Object* transformObject = json_object_get_object(layerObject, "transform");
    if (transformObject) {

    }
    return layerInfo;
}

shared_ptr<Transform> YMatConfig::getTransform(JSON_Object *transformObject) {
    auto transform = make_shared<Transform>();
    if (json_object_has_value(transformObject, "anchorPoint")) {
        setTransformInfo(transform->anchorPoint->transformList, json_object_get_array(transformObject, "anchorPoint"));
    }
    return transform;
}

void YMatConfig::setTransformInfo(shared_ptr<list<shared_ptr<TransformBean>>> transform, JSON_Array *transformArray) {
    if (transformArray) {
        auto transformSize = json_array_get_count(transformArray);
        for (int i = 0; i < transformSize; ++i) {
            JSON_Object* transformBeanObject = json_array_get_object(transformArray, i);
            if (transformBeanObject) {
                auto transformBean = make_shared<TransformBean>();
                transformBean->inFrame = (int) json_object_get_number(transformBeanObject, "inFrame");
                setValue(&transformBean->value, json_object_get_array(transformBeanObject, "value"));
                if (json_object_has_value(transformBeanObject, "timeFunc")) {
                    transformBean->timeFunc = (int) json_object_get_number(transformBeanObject,
                                                                           "timeFunc");
                }
                transform->push_back(transformBean);
            }
        }
    }
}

void YMatConfig::setValue(vector<float>* value, JSON_Array *valueArray) {
    if (valueArray) {
        auto bgColorSize = json_array_get_count(valueArray);
        for (int i = 0; i < bgColorSize; ++i) {
            value->push_back((float)json_array_get_number(valueArray, i));
        }
    }
}

