//
// Created by zengjiale on 2022/4/18.
//
#pragma once
#include <memory>
#include <bean/pointrect.h>

using namespace std;
namespace ymat {
    class VertexUtil {
    public:
        static float *create(int width, int height, shared_ptr <PointRect> rect, float *array);

        static float switchX(float x);

        static float switchY(float y);
    };
}
