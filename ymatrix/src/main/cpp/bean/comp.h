//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <string>
#include <bean/layerinfo.h>
#include <vector>

using namespace std;
namespace ymat {
    class Comp {
    public:
        int id;
        string type;
        shared_ptr<list<LayerInfo>> layers;
        vector<float> bgColor;
        int width;
        int height;
    };

    class VideoComp: public Comp {
        string content;
        string name;
        int duration;
        int frameRate;
    };
}
