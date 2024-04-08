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
        list<shared_ptr<SimpleLayerInfo>> layers;
    };
}
