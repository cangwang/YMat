//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <vector>

using namespace std;
namespace ymat {
    class Element {
        shared_ptr<list<Element>> elementInfo;
        list<ShaderTransformInfo> transform;
    };

    class ElementInfo {

    };
    class RectInfo : public ElementInfo {
        int direction;
        vector<float> size;
        vector<float> position;
        int roundness;
    };

    class StrokeInfo : public ElementInfo{
        int blendMode;
        vector<float> color;
        int opacity;
        int width;
        int lineCap;
        int lineJoin;
        int miterLimit;
    };

    class FillInfo : public ElementInfo{
        int blendMode;
        vector<float> color;
        int opacity;
    };

    class EllipseInfo : public ElementInfo{
        int direction;
        vector<float> size;
        vector<float> position;
    };
}
