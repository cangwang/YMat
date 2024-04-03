//
// Created by Cangwang on 2024/4/1.
//

#pragma once
#include <list>
#include <bean/targetcomp.h>
#include <bean/comp.h>
#include <bean/source.h>
#include <vector>

using namespace std;
namespace ymat {
    class YMInfo {
    public:
        vector<float> bgColor;
        int width;
        int height;
        int duration;
        int frameRate;
        shared_ptr<TargetComp> targetComp;
        shared_ptr<list<Comp>> comps;
        shared_ptr<ImgSource> imgSource;
        shared_ptr<VideoSource> videoSource;
    };
}
