//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <bean/element.h>
#include <bean/transform.h>
#include <bean/TextureInfo.h>

using namespace std;
using namespace ymat;
namespace ymat {
    const static string PATH = "Path";
    const static string RECT = "Rect";
    const static string ELLIPSE = "Ellipse";

    class ShapeContent {
    public:
        string type; //Rect
        string name;
        int blendMode;
        shared_ptr<Element> elements;
        shared_ptr<Transform> transforms;
        shared_ptr<TextureInfo> textureInfo;
    };
}
