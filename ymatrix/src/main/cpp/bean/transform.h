//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <bean/transforminfo.h>

using namespace std;
namespace ymat {
    class Transform {
    public:
        shared_ptr<TransformInfo> anchorPoint;
        shared_ptr<TransformInfo> position;
        shared_ptr<TransformInfo> scale;
        shared_ptr<TransformInfo> opacity;
        shared_ptr<TransformInfo> rotationX;
        shared_ptr<TransformInfo> rotationY;
        shared_ptr<TransformInfo> rotationZ;
        bool isTrackMatte;
        int width;
        int height;
        int inFrame;
        int outFrame;
    };
}
