//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <string>
#include <vector>

using namespace std;
namespace ymat {
    class TransformBean {
    public:
        int inFrame;
        vector<float> value;
        int timeFunc = 0;
    };
    class TransformInfo {
    public:
        shared_ptr<list<shared_ptr<TransformBean>>> transformList;
    };

    class ShaderTransformInfo {
        string type; //anchorPoint position scale opacity rotationX rotationY rotationZ orientation
        int inFrame;
        vector<float> value;
    };
}
