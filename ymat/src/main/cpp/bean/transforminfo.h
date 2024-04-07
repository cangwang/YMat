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
        ~TransformBean() {
            value.clear();
        }
        int inFrame;
        vector<float> value;
        int timeFunc = 0;
    };
    class TransformInfo {
    public:
        ~TransformInfo() {
            transformList.clear();
        }
        list<shared_ptr<TransformBean>> transformList;
    };

    class ShaderTransformInfo {
    public:
        string type; //anchorPoint position scale opacity rotationX rotationY rotationZ orientation
        int inFrame;
        vector<float> value;
    };
}
