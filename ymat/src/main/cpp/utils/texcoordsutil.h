//
// Created by zengjiale on 2022/4/18.
//
#pragma once
#include <bean/pointrect.h>
#include <memory>

using namespace std;
namespace ymat {
    class TexCoordsUtil {
    public:

        static float *create(int width, int height, shared_ptr<PointRect> rect, float *array);

        static float *rotate90(float *array);
    };
}
