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
        ~Comp() {
            layers.clear();
            bgColor.clear();
        }
        int id = 0;
        string type;
        vector<shared_ptr<SimpleLayerInfo>> layers;
        vector<float> bgColor;
        int width = 0;
        int height = 0;
        string name;
        string content;
        bool isTrackMatte;
        int trackMatteLayer = 0; //isTrackMatte false时存在值， 蒙版对应图层layer id
        int trackMatteType = 0; // 1透明， 2透明反色值， 3明亮度， 4明亮度饭色值
        int blendMode = 0;

//        int textureId = -1;
        int duration = 0;
        int frameRate = 0;
    };
}
