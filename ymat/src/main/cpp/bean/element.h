//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <vector>
#include "transforminfo.h"
using namespace std;
namespace ymat {
    class DashesInfo {
    public:
        ~DashesInfo() {
            dash.clear();
        }
        vector<int> dash;
        shared_ptr<TransformSimpleInfo> offset;
    };
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

    //https://developer.mozilla.org/zh-CN/docs/Web/API/CanvasRenderingContext2D/lineJoin 属性说明
    class StrokeInfo {
    public:
        ~StrokeInfo() {
            color.clear();
        }
        int blendMode;   //混合模式
        vector<float> color;
        int opacity;
        int width;
        int lineCap;  //线帽
        int lineJoin;  //设置 2 个长度不为 0 的相连部分（线段、圆弧、曲线）如何连接在一起的属性
        int miterLimit; //设置斜接面限制比例的属性
        shared_ptr<DashesInfo> dashesInfo;
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

    class ShapeInfo {
    public:
        ~ShapeInfo() {
            for (vector<float> p : points) {
                p.clear();
            }
            points.clear();
            actions.clear();
        };

        vector<vector<float>> points;
        vector<int> actions;
    };

    class Element {
    public:
        shared_ptr<RectInfo> rectInfo;
        shared_ptr<StrokeInfo> strokeInfo;
        shared_ptr<FillInfo> fillInfo;
        shared_ptr<EllipseInfo> ellipseInfo;
        shared_ptr<ShapeInfo> shapeInfo;
    };
}
