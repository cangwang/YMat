//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <string>
#include <vector>
#include <utils/transformutil.h>

using namespace std;
namespace ymat {

    class TransformSimpleBean {
    public:
        int inFrame;
        float value;
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
        vector<shared_ptr<TransformBean>> transformList;

        vector<float> getValue(int frameIndex) {
            vector<float> v;
            if (transformList.empty()) return v;
            if (frameIndex < transformList.front()->inFrame) return v;
            unsigned int l = transformList.size();
            unsigned int idx = 0;
            for (unsigned int i = 0; i < l; i++) {
                if (transformList[i]->inFrame == frameIndex) return transformList[i]->value;
                if (transformList[i]->inFrame > frameIndex) break;
                idx = i;
            }

            if (idx == l - 1) {
                return transformList[idx]->value;
            }
            //计算
            auto lhs = transformList[idx];
            auto rhs = transformList[idx + 1];

            float fas;
            switch (transformList[idx]->timeFunc) {
                case 0:
                    fas = TransformUtil::linear(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
                case 1:
                    fas = TransformUtil::easeInQuad(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
                case 2:
                    fas = TransformUtil::easeOutQuad(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
                case 3:
                    fas = TransformUtil::easeInOutQuad(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
                default:
                    fas = TransformUtil::linear(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
            }
            for (unsigned int i = 0; i < lhs->value.size(); ++i) {
                float value = lhs->value[i] + (rhs->value[i] - lhs->value[i]) * fas;
                v.push_back(value);
            }
            return v;
        }
    };

    class TransformSimpleInfo {
    public:
        ~TransformSimpleInfo() {
            transformList.clear();
        }
        vector<shared_ptr<TransformSimpleBean>> transformList;

        shared_ptr<float> getValue(int frameIndex) {
            if (transformList.empty()) return nullptr;
            if (frameIndex < transformList.front()->inFrame) return nullptr;
            unsigned int l = transformList.size();
            unsigned int idx = 0;
            for (unsigned int i = 0; i < l; i++) {
                if (transformList[i]->inFrame == frameIndex) return make_shared<float>(transformList[i]->value);
                if (transformList[i]->inFrame > frameIndex) break;
                idx = i;
            }

            if (idx == l - 1) {
                return make_shared<float>(transformList[idx]->value);
            }
            //计算
            auto lhs = transformList[idx];
            auto rhs = transformList[idx + 1];

            float fas;
            switch (transformList[idx]->timeFunc) {
                case 0:
                    fas = TransformUtil::linear(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
                case 1:
                    fas = TransformUtil::easeInQuad(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
                case 2:
                    fas = TransformUtil::easeOutQuad(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
                case 3:
                    fas = TransformUtil::easeInOutQuad(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
                default:
                    fas = TransformUtil::linear(frameIndex, lhs->inFrame, rhs->inFrame);
                    break;
            }

            float v = lhs->value + (rhs->value - lhs->value) * fas;

            return make_shared<float>(v);
        }
    };
}
