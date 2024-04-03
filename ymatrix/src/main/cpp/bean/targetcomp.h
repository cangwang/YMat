//
// Created by Cangwang on 2024/4/1.
//

#pragma once
#include <bean/layerinfo.h>

using namespace std;
namespace ymat {
    class TargetComp {
    public:
        shared_ptr<list<shared_ptr<LayerInfo>>> layers;
    };
}
