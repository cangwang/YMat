#pragma once
#include <cmath>
#include <cstring>

using namespace std;
namespace ymat
{
    #define PI 3.1415926
    typedef float vec3f[3];

    static float degToRad(float d)
    {
        return d * PI / 180;
    }

    static void normalize(vec3f& v)
    {
        float len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        if (len > 0.00001)
        {
            v[0] = v[0] / len;
            v[1] = v[1] / len;
            v[2] = v[2] / len;
        }
    }

    static void subtractVec3(vec3f a, vec3f b, vec3f &out)
    {
        out[0] = a[0] - b[0];
        out[1] = a[1] - b[1];
        out[2] = a[2] - b[2];
    }

    static void cross(vec3f a, vec3f b, vec3f &out)
    {
        out[0] = a[1] * b[2] - a[2] * b[1];
        out[1] = a[2] * b[0] - a[0] * b[2];
        out[2] = a[0] * b[1] - a[1] * b[0];
    }

    class Matrix
    {
    public:
        Matrix();

        Matrix(const Matrix &m);

        // 默认矩阵
        static void identity(Matrix& m);

        // 透视矩阵
        static void perspective(Matrix& m, float fieldOfViewInRadians, float aspect, float near, float far);

        // 平移矩阵
        static void translation(Matrix& m, float tx, float ty, float tz);

        static void lookAt(Matrix& m, vec3f cameraPosition, vec3f target, vec3f up);

        // 摄像机透视矩阵
        static void perspectiveCamera(Matrix& m, float width, float height, float fieldOfViewDeg);

        // 矩阵相乘
        static void multiply(const Matrix& a, const Matrix& b, Matrix& dst);

        //Android使用的变换矩阵
        static void orthoM(Matrix& m, float left, float right, float bottom, float top, float nearVal, float farVal);

        static void orthoM(Matrix& m, float left, float right, float bottom, float top);

        static void set(Matrix& m, float scaleX, float scaleY,  float translateX, float translateY);

        static void setFillMode(Matrix& m, int fillMode, float width, float height, float surfaceWidth, float surfaceHeight);

        void xRotate(float angleInRadians);

        void yRotate(float angleInRadians);

        void zRotate(float angleInRadians);

        void scale(float sx, float sy, float sz);

        void inverse();

        float get(int index) const
        {
            return values[index];
        }

        void set(int index, float value)
        {
            values[index] = value;
        }

        void getValue(float buffer[16]) const
        {
            memcpy(buffer, values, 16 * sizeof(float));
        }

        float* getValues() {
            return values;
        }

        Matrix &operator=(const Matrix &m);

        float operator[](int index) const
        {
            return values[index];
        }

        float &operator[](int index)
        {
            return values[index];
        }

        float values[16];
    };
} // namespace libdraw
