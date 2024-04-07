//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <vector>

using namespace std;
namespace ymat {
    class RectInfo {
    public:
        ~RectInfo() {
            size.clear();
            position.clear();
        }
        int direction;
        vector<float> size;
        vector<float> position;
        int roundness;
    };

    class StrokeInfo {
    public:
        ~StrokeInfo() {
            color.clear();
        }
        int blendMode;
        vector<float> color;
        int opacity;
        int width;
        int lineCap;
        int lineJoin;
        int miterLimit;
    };

    class FillInfo {
    public:
        ~FillInfo() {
            color.clear();
        }
        int blendMode;
        vector<float> color;
        int opacity;
    };

    class EllipseInfo {
    public:
        ~EllipseInfo() {
            size.clear();
            position.clear();
        }
        int direction;
        vector<float> size;
        vector<float> position;
    };

    class Element {
    public:
        shared_ptr<RectInfo> rectInfo;
        shared_ptr<StrokeInfo> strokeInfo;
        shared_ptr<FillInfo> fillInfo;
        shared_ptr<EllipseInfo> ellipseInfo;
    };
}
