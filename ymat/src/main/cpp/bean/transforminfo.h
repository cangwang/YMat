//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <string>
#include <vector>
#include <list>

using namespace std;
namespace ymat {

    class TransformSimpleBean {
    public:
        int inFrame;
        int value;
        int timeFunc = 0;
    };

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

    class TransformSimpleInfo {
    public:
        ~TransformSimpleInfo() {
            transformList.clear();
        }
        list<shared_ptr<TransformSimpleBean>> transformList;
    };
}
