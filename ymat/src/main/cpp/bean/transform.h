//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <bean/transforminfo.h>

using namespace std;
namespace ymat {
    class BaseTransform {
    public:
        shared_ptr<TransformInfo> anchorPoint = make_shared<TransformInfo>();
        shared_ptr<TransformInfo> position = make_shared<TransformInfo>();
        shared_ptr<TransformInfo> scale = make_shared<TransformInfo>();
        shared_ptr<TransformInfo> orientation = make_shared<TransformInfo>();
        shared_ptr<TransformSimpleInfo> opacity = make_shared<TransformSimpleInfo>();
        shared_ptr<TransformSimpleInfo> rotationX = make_shared<TransformSimpleInfo>();
        shared_ptr<TransformSimpleInfo> rotationY = make_shared<TransformSimpleInfo>();
        shared_ptr<TransformSimpleInfo> rotationZ = make_shared<TransformSimpleInfo>();

        vector<float> getAnchorPoint(int frameIndex, float offX, float offY) {
            vector<float> points;
            if (!anchorPoint) return points;

            auto value = anchorPoint->getValue(frameIndex);
            if (!value.empty()) {
                points.push_back(value[0] + offX);
                points.push_back(value[1] + offY);
                if (value.size() == 3) {
                    points.push_back(value[2]);
                } else {
                    points.push_back(0);
                }
            }

            return points;
        }

        vector<float> getPosition(int frameIndex, float offX, float offY) {
            vector<float> positions;
            if (!position) return positions;

            auto value = position->getValue(frameIndex);
            if (!value.empty()) {
                positions.push_back(value[0] + offX);
                positions.push_back(value[1] + offY);
                if (value.size() == 3) {
                    positions.push_back(value[2]);
                } else {
                    positions.push_back(0);
                }
            }

            return positions;
        }

        vector<float> getScale(int frameIndex) {
            return scale->getValue(frameIndex);
        }

        shared_ptr<float> getRotationX(int frameIndex) {
            return rotationX->getValue(frameIndex);
        }

        shared_ptr<float> getRotationY(int frameIndex) {
            return rotationY->getValue(frameIndex);
        }

        shared_ptr<float> getRotationZ(int frameIndex) {
            return rotationZ->getValue(frameIndex);
        }

        vector<float> getOrientation(int frameIndex) {
            return orientation->getValue(frameIndex);
        }

        float getOpacity(int frameIndex) {
            if (opacity) {
                auto op = opacity->getValue(frameIndex);
                if (op) {
                    return *op * 0.01f;
                }
            }
            return 1.0f;
        }
    };

    class Transform : public BaseTransform {
    public:
    };
}
