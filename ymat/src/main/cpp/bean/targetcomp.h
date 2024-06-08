//
// Created by Cangwang on 2024/4/1.
//

#pragma once
#include <bean/layerinfo.h>

using namespace std;
namespace ymat {
    class TargetComp {
    public:
        ~TargetComp() {
            layers.clear();
        }
        vector<shared_ptr<SimpleLayerInfo>> layers;
    };
}
