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
        ~YMInfo() {
            bgColor.clear();
            comps.clear();
            imgSource.clear();
            videoSource.clear();
        }
        vector<float> bgColor;
        int width;
        int height;
        int duration;
        int frameRate;
        shared_ptr<TargetComp> targetComp;
        list<shared_ptr<Comp>> comps;
        list<shared_ptr<ImgSource>> imgSource;
        list<shared_ptr<VideoSource>> videoSource;
    };
}
