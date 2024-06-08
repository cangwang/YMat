//
// Created by Cangwang on 2024/4/1.
//

#pragma once

#include <bean/targetcomp.h>
#include <bean/comp.h>
#include <vector>
#include <map>

using namespace std;
namespace ymat {
    class YMInfo {
    public:
        ~YMInfo() {
            bgColor.clear();
            comps.clear();
            layers.clear();
        }
        vector<float> bgColor;
        int width;
        int height;
        int duration;
        int frameRate;
        shared_ptr<TargetComp> targetComp;
        vector<shared_ptr<Comp>> comps;
        //用于记录全局layer信息
        map<int, shared_ptr<SimpleLayerInfo>> layers;
    };
}
