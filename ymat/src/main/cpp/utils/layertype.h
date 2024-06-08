//
// Created by Cangwang on 2024/4/16.
//

#pragma once
#include <map>
#include <string>

using namespace std;
namespace ymat {
    enum LayerType {
        Text,
        Image,
        Shape,
        PreComposition,
        Video,
        Vector
    };

    // 使用map来存储LayerType到字符串的映射
    const map<LayerType, string> layerTypeToString = {
            {Text, "text"},
            {Image, "image"},
            {Shape, "ShapeLayer"},
            {PreComposition, "precomposition"},
            {Video, "video"},
            {Vector, "vector"},
    };

    static string getLayerType(LayerType type) {
        auto iter = layerTypeToString.find(type);
        if (iter != layerTypeToString.end()) {
            return iter->second;
        } else {
            return "Unknown";
        }
    }
}


