//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <bean/element.h>
#include <bean/transform.h>

using namespace std;
using namespace ymat;
namespace ymat {
    class ShaderContent {
        string type; //Rect
        string name;
        int blendMode;
        shared_ptr<Element> elements;
        shared_ptr<Transform> transforms;
    };
}
