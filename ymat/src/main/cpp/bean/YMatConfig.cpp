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

        if (targetCompObject) {
            config->targetComp = make_shared<TargetComp>();
            JSON_Array* layers = json_object_get_array(targetCompObject,"layers");
            if (layers) {
                auto layersSize = json_array_get_count(layers);
                for (int i = 0; i < layersSize; ++i) {
                    JSON_Object* layerObject= json_array_get_object(layers, i);
                    if (layerObject) {
                        config->targetComp->layers.push_back(getLayer(layerObject));
                    }
                }
            }
        }
        targetCompObject = nullptr;

        //解析Comp
        JSON_Array *compsArrayObject = json_object_get_array(jsonObject, "comps");
        if (compsArrayObject) {
            auto compsCount = json_array_get_count(compsArrayObject);
            for (auto i = 0; i < compsCount; ++i) {
                JSON_Object* compObj = json_array_get_object(compsArrayObject, i);
                auto comp = make_shared<Comp>();
                comp->type = json_object_get_string(compObj, "type");
                comp->id = (int)json_object_get_number(compObj, "id");

                JSON_Array *layerArrayObject = json_object_get_array(compObj, "layers");
                auto layersCount = json_array_get_count(layerArrayObject);
                for (auto i = 0; i < layersCount; ++i) {
                    comp->layers.push_back(getLayer(json_array_get_object(layerArrayObject, i)));
                }
                setColor(&comp->bgColor, json_object_get_array(compObj, "bgColor"));
                comp->width = (int)json_object_get_number(compObj, "width");
                comp->height = (int)json_object_get_number(compObj, "height");
                if (json_object_has_value(compObj, "frameRate")) {
                    comp->frameRate = (int)json_object_get_number(compObj, "frameRate");
                }
                if (json_object_has_value(compObj, "name")) {
                    comp->name = json_object_get_string(compObj, "name");
                }
                if (json_object_has_value(compObj, "content")) {
                    comp->content = json_object_get_string(compObj, "content");
                }
                config->comps.push_back(comp);
            }
        }
        compsArrayObject = nullptr;
        //解析imgSource
        JSON_Array *imgArrayObject = json_object_get_array(jsonObject, "imgSource");
        if (imgArrayObject) {
            auto imgCount = json_array_get_count(imgArrayObject);
            for (auto i = 0; i < imgCount; ++i) {
                JSON_Object* imgObj = json_array_get_object(imgArrayObject, i);
                auto iSource = make_shared<ImgSource>();
                iSource->srcPath = json_object_get_string(imgObj, "srcPath");
                iSource->desPath = json_object_get_string(imgObj, "desPath");
                iSource->path = json_object_get_string(imgObj, "path");
                config->imgSource.push_back(iSource);
            }
        }
        imgArrayObject = nullptr;
        //解析videoSource
        JSON_Array *videoArrayObject = json_object_get_array(jsonObject, "videoSource");
        if (videoArrayObject) {
            auto videoCount = json_array_get_count(videoArrayObject);
            for (auto i = 0; i < videoCount; ++i) {
                JSON_Object* videoObj = json_array_get_object(videoArrayObject, i);
                auto vSource = make_shared<VideoSource>();
                if (json_object_has_value(videoObj, "srcPath")) {
                    vSource->srcPath = json_object_get_string(videoObj, "srcPath");
                }
                vSource->content = json_object_get_string(videoObj, "content");
                vSource->name = json_object_get_string(videoObj, "name");
                config->videoSource.push_back(vSource);
            }
        }
        videoArrayObject = nullptr;
        return config;
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

shared_ptr<SimpleLayerInfo> YMatConfig::getLayer(JSON_Object *layerObject) {
    string type = json_object_get_string(layerObject, "type");
    shared_ptr<SimpleLayerInfo> layerInfo;
    if (type == "text") {
        layerInfo = make_shared<TextLayerInfo>();
    } else if (type == "image") {
        layerInfo = make_shared<ImageLayerInfo>();
    } else if (type == "video") {
        layerInfo = make_shared<VideoLayerInfo>();
    } else if (type == "ShapeLayer") {
        layerInfo = make_shared<ShadeLayerInfo>();
    } else if(type == "precomposition") {
        layerInfo = make_shared<PreCompositionLayerInfo>();
    }
    if (layerInfo != nullptr) {
        layerInfo->type = type;
        if (json_object_has_value(layerObject, "id")) {
            layerInfo->id = (int) json_object_get_number(layerObject, "id");
        }
        if (json_object_has_value(layerObject, "name")) {
            layerInfo->name = json_object_get_string(layerObject, "name");
        }

        if (json_object_has_value(layerObject, "content")) {
            if (type == "ShapeLayer") {
                setShapeContent(&dynamic_pointer_cast<ShadeLayerInfo>(layerInfo)->shapeContents,
                                json_object_get_array(layerObject,"content"));
            } else {
                layerInfo->content = json_object_get_string(layerObject, "content");
            }
        }
        JSON_Object *transformObject = json_object_get_object(layerObject, "transform");
        if (transformObject) {
            layerInfo->transform = getTransform(transformObject);
        }

        layerInfo->isTrackMatte = json_object_get_boolean(layerObject,"isTrackMatte");
        layerInfo->width = (int) json_object_get_number(layerObject,"width");

        layerInfo->height = (int) json_object_get_number(layerObject,"height");
        layerInfo->inFrame = (int) json_object_get_number(layerObject,"inFrame");
        layerInfo->outFrame = (int) json_object_get_number(layerObject,"outFrame");

        if (type == "text") {
            JSON_Object *textDocAttrObject = json_object_get_object(layerObject, "textDocAttr");
            dynamic_pointer_cast<TextLayerInfo>(
                    layerInfo)->textDocAttr = getTextDocAttr(textDocAttrObject);
        }

        if (type == "image") {
            dynamic_pointer_cast<ImageLayerInfo>(
                    layerInfo)->fillMode = (int) json_object_get_number(layerObject,"fillMode");
        }
    }
    return layerInfo;
}

shared_ptr<Transform> YMatConfig::getTransform(JSON_Object *transformObject) {
    auto transform = make_shared<Transform>();
    if (json_object_has_value(transformObject, "anchorPoint")) {
        transform->anchorPoint = make_shared<TransformInfo>();
        setTransformInfo(&transform->anchorPoint->transformList, json_object_get_array(transformObject, "anchorPoint"));
    }
    if (json_object_has_value(transformObject, "position")) {
        transform->position = make_shared<TransformInfo>();
        setTransformInfo(&transform->position->transformList, json_object_get_array(transformObject, "position"));
    }
    if (json_object_has_value(transformObject, "scale")) {
        transform->scale = make_shared<TransformInfo>();
        setTransformInfo(&transform->scale->transformList, json_object_get_array(transformObject, "scale"));
    }
    if (json_object_has_value(transformObject, "orientation")) {
        transform->orientation = make_shared<TransformInfo>();
        setTransformInfo(&transform->orientation->transformList, json_object_get_array(transformObject, "orientation"));
    }
    if (json_object_has_value(transformObject, "opacity")) {
        transform->opacity = make_shared<TransformSimpleInfo>();
        setTransformInfo(&transform->opacity->transformList, json_object_get_array(transformObject, "opacity"));
    }
    if (json_object_has_value(transformObject, "rotationX")) {
        transform->rotationX = make_shared<TransformSimpleInfo>();
        setTransformInfo(&transform->rotationX->transformList, json_object_get_array(transformObject, "rotationX"));
    }
    if (json_object_has_value(transformObject, "rotationY")) {
        transform->rotationY = make_shared<TransformSimpleInfo>();
        setTransformInfo(&transform->rotationY->transformList, json_object_get_array(transformObject, "rotationY"));
    }
    if (json_object_has_value(transformObject, "rotationZ")) {
        transform->rotationZ = make_shared<TransformSimpleInfo>();
        setTransformInfo(&transform->rotationZ->transformList, json_object_get_array(transformObject, "rotationZ"));
    }
//    transform->isTrackMatte = json_object_get_boolean(transformObject, "isTrackMatte");
//    transform->width = (int)json_object_get_number(transformObject, "width");
//    transform->height = (int)json_object_get_number(transformObject, "height");
//    transform->inFrame = (int)json_object_get_number(transformObject, "inFrame");
//    transform->outFrame = (int)json_object_get_number(transformObject, "outFrame");
    return transform;
}

void YMatConfig::setTransformInfo(list<shared_ptr<TransformBean>>* transform, JSON_Array *transformArray) {
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

void YMatConfig::setTransformInfo(list<shared_ptr<TransformSimpleBean>>* transform, JSON_Array *transformArray) {
    if (transformArray) {
        auto transformSize = json_array_get_count(transformArray);
        for (int i = 0; i < transformSize; ++i) {
            JSON_Object* transformBeanObject = json_array_get_object(transformArray, i);
            if (transformBeanObject) {
                auto transformBean = make_shared<TransformSimpleBean>();
                transformBean->inFrame = (int) json_object_get_number(transformBeanObject, "inFrame");
                transformBean->value = (int) json_object_get_number(transformBeanObject, "value");
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

void YMatConfig::setShapeContent(list<shared_ptr<ShapeContent>>* shapeContentList, JSON_Array *shapeContentArray) {
    if (shapeContentArray != nullptr) {
        auto contentSize = json_array_get_count(shapeContentArray);
        for (int i = 0; i < contentSize; ++i) {
           JSON_Object* shaderContentObj = json_array_get_object(shapeContentArray, i);
           if (shaderContentObj) {
               auto content = make_shared<ShapeContent>();
               content->type = json_object_get_string(shaderContentObj, "type");
               content->name = json_object_get_string(shaderContentObj, "name");
               content->blendMode = (int)json_object_get_number(shaderContentObj, "blendMode");
               JSON_Object* elementObj = json_object_get_object(shaderContentObj, "elements");
               if (elementObj) {
                   content->elements = make_shared<Element>();
                   setElement(content->elements, elementObj);
               }
               JSON_Object* transformObj = json_object_get_object(shaderContentObj, "transform");
               if (transformObj) {
                   content->transforms = getTransform(transformObj);
               }
               shapeContentList->push_back(content);
           }
        }
    }
}

void YMatConfig::setElement(shared_ptr<Element> element, JSON_Object *elementObj) {
    if (elementObj) {
        JSON_Object* rectObj = json_object_get_object(elementObj, "rectInfo");
        if (rectObj) {
            element->rectInfo = make_shared<RectInfo>();
            element->rectInfo->direction = (int)json_object_get_number(rectObj, "direction");
            setValue(&element->rectInfo->size, json_object_get_array(rectObj, "size"));
            setValue(&element->rectInfo->position, json_object_get_array(rectObj, "position"));
            element->rectInfo->roundness = (int)json_object_get_number(rectObj, "roundness");
        }

        JSON_Object* strokeObj = json_object_get_object(elementObj, "strokeInfo");
        if (strokeObj) {
            element->strokeInfo = make_shared<StrokeInfo>();
            element->strokeInfo->blendMode = (int)json_object_get_number(strokeObj, "blendMode");
            setColor(&element->strokeInfo->color, json_object_get_array(strokeObj, "color"));
            element->strokeInfo->opacity = (int)json_object_get_number(strokeObj, "opacity");
            element->strokeInfo->width = (int)json_object_get_number(strokeObj, "width");
            element->strokeInfo->lineCap = (int)json_object_get_number(strokeObj, "lineCap");
            element->strokeInfo->lineJoin = (int)json_object_get_number(strokeObj, "lineJoin");
            element->strokeInfo->miterLimit = (int)json_object_get_number(strokeObj, "miterLimit");
        }

        JSON_Object* ellipseObj = json_object_get_object(elementObj, "ellipseInfo");
        if (ellipseObj) {
            element->ellipseInfo = make_shared<EllipseInfo>();
            element->ellipseInfo->direction = (int)json_object_get_number(ellipseObj, "direction");
            setValue(&element->ellipseInfo->size, json_object_get_array(ellipseObj, "size"));
            setValue(&element->ellipseInfo->position, json_object_get_array(ellipseObj, "position"));
        }

        JSON_Object* fillObj = json_object_get_object(elementObj, "fillInfo");
        if (fillObj) {
            element->fillInfo = make_shared<FillInfo>();
            element->fillInfo->blendMode = (int)json_object_get_number(fillObj, "blendMode");
            setColor(&element->fillInfo->color, json_object_get_array(fillObj, "color"));
            element->fillInfo->opacity = (int)json_object_get_number(fillObj, "opacity");
        }
    }
}
