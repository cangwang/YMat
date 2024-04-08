//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <bean/transforminfo.h>

using namespace std;
namespace ymat {
    class BaseTransform {
    public:
        shared_ptr<TransformInfo> anchorPoint;
        shared_ptr<TransformInfo> position;
        shared_ptr<TransformInfo> scale;
        shared_ptr<TransformInfo> orientation;
        shared_ptr<TransformSimpleInfo> opacity;
        shared_ptr<TransformSimpleInfo> rotationX;
        shared_ptr<TransformSimpleInfo> rotationY;
        shared_ptr<TransformSimpleInfo> rotationZ;
    };

    class Transform : public BaseTransform {
    public:
    };

    class ShapeTransform : public BaseTransform {
    };
}
