#include "matrix.h"

namespace ymat
{
    Matrix::Matrix()
    {
        memset(values, 0, sizeof(float));
        values[0 * 4 + 0] = 1;
        values[0 * 4 + 1] = 0;
        values[0 * 4 + 2] = 0;
        values[0 * 4 + 3] = 0;
        values[1 * 4 + 0] = 0;
        values[1 * 4 + 1] = 1;
        values[1 * 4 + 2] = 0;
        values[1 * 4 + 3] = 0;
        values[2 * 4 + 0] = 0;
        values[2 * 4 + 1] = 0;
        values[2 * 4 + 2] = 1;
        values[2 * 4 + 3] = 0;
        values[3 * 4 + 0] = 0;
        values[3 * 4 + 1] = 0;
        values[3 * 4 + 2] = 0;
        values[3 * 4 + 3] = 1;
    }

    Matrix::Matrix(const Matrix &m)
    {
        m.getValue(values);
    }

    void Matrix::identity(Matrix& m)
    {
        m[0 * 4 + 0] = 1;
        m[0 * 4 + 1] = 0;
        m[0 * 4 + 2] = 0;
        m[0 * 4 + 3] = 0;
        m[1 * 4 + 0] = 0;
        m[1 * 4 + 1] = 1;
        m[1 * 4 + 2] = 0;
        m[1 * 4 + 3] = 0;
        m[2 * 4 + 0] = 0;
        m[2 * 4 + 1] = 0;
        m[2 * 4 + 2] = 1;
        m[2 * 4 + 3] = 0;
        m[3 * 4 + 0] = 0;
        m[3 * 4 + 1] = 0;
        m[3 * 4 + 2] = 0;
        m[3 * 4 + 3] = 1;
    }

    void Matrix::perspective(Matrix& m, float fieldOfViewInRadians, float aspect, float near, float far)
    {
        float f = std::tan(PI * 0.5 - 0.5 * fieldOfViewInRadians);
        float rangeInv = 1.0 / (near - far);

        m[0] = f / aspect;
        m[1] = 0;
        m[2] = 0;
        m[3] = 0;
        m[4] = 0;
        m[5] = f;
        m[6] = 0;
        m[7] = 0;
        m[8] = 0;
        m[9] = 0;
        m[10] = (near + far) * rangeInv;
        m[11] = -1;
        m[12] = 0;
        m[13] = 0;
        m[14] = near * far * rangeInv * 2;
        m[15] = 0;
    }

    void Matrix::translation(Matrix& m, float tx, float ty, float tz)
    {
        m[0] = 1;
        m[1] = 0;
        m[2] = 0;
        m[3] = 0;
        m[4] = 0;
        m[5] = 1;
        m[6] = 0;
        m[7] = 0;
        m[8] = 0;
        m[9] = 0;
        m[10] = 1;
        m[11] = 0;
        m[12] = tx;
        m[13] = ty;
        m[14] = tz;
        m[15] = 1;
    }

    void Matrix::lookAt(Matrix& m, vec3f cameraPosition, vec3f target, vec3f up)
    {
        float zAxis[3] = {0.0f};
        float xAxis[3] = {0.0f};
        float yAxis[3] = {0.0f};

        subtractVec3(cameraPosition, target, zAxis);
        normalize(zAxis);
        cross(up, zAxis, xAxis);
        normalize(xAxis);
        cross(zAxis, xAxis, yAxis);
        normalize(yAxis);

        m[0] = xAxis[0];
        m[1] = xAxis[1];
        m[2] = xAxis[2];
        m[3] = 0;
        m[4] = yAxis[0];
        m[5] = yAxis[1];
        m[6] = yAxis[2];
        m[7] = 0;
        m[8] = zAxis[0];
        m[9] = zAxis[1];
        m[10] = zAxis[2];
        m[11] = 0;
        m[12] = cameraPosition[0];
        m[13] = cameraPosition[1];
        m[14] = cameraPosition[2];
        m[15] = 1;
    }

    void Matrix::perspectiveCamera(Matrix& dst, float width, float height, float fieldOfViewDeg)
    {
        // 透视矩阵
        float fieldOfViewRadians = degToRad(fieldOfViewDeg);
        float aspect = width / height;
        float zNear = 1;
        float zFar = 20000;
        Matrix projectionMatrix;
        Matrix::perspective(projectionMatrix, fieldOfViewRadians, aspect, zNear, zFar);

        // 相机坐标矩阵
        float zFlat = (height / tan(fieldOfViewRadians * 0.5)) * 0.5;
        float cameraPosition[3] = {width * 0.5f, -height * 0.5f, zFlat};
        float target[3] = {width * 0.5f, -height * 0.5f, 0};
        float up[3] = {0, 1, 0};
        Matrix cameraMatrix;
        Matrix::lookAt(cameraMatrix, cameraPosition, target, up);
        cameraMatrix.inverse();

        Matrix::multiply(projectionMatrix, cameraMatrix, dst);
    }

    void Matrix::xRotate(float angleInRadians)
    {
        float v10 = values[4];
        float v11 = values[5];
        float v12 = values[6];
        float v13 = values[7];
        float v20 = values[8];
        float v21 = values[9];
        float v22 = values[10];
        float v23 = values[11];
        float c = cos(angleInRadians);
        float s = sin(angleInRadians);

        values[4] = c * v10 + s * v20;
        values[5] = c * v11 + s * v21;
        values[6] = c * v12 + s * v22;
        values[7] = c * v13 + s * v23;
        values[8] = c * v20 - s * v10;
        values[9] = c * v21 - s * v11;
        values[10] = c * v22 - s * v12;
        values[11] = c * v23 - s * v13;
    }

    void Matrix::yRotate(float angleInRadians)
    {
        float v00 = values[0 * 4 + 0];
        float v01 = values[0 * 4 + 1];
        float v02 = values[0 * 4 + 2];
        float v03 = values[0 * 4 + 3];
        float v20 = values[2 * 4 + 0];
        float v21 = values[2 * 4 + 1];
        float v22 = values[2 * 4 + 2];
        float v23 = values[2 * 4 + 3];
        float c = cos(angleInRadians);
        float s = sin(angleInRadians);

        values[0] = c * v00 - s * v20;
        values[1] = c * v01 - s * v21;
        values[2] = c * v02 - s * v22;
        values[3] = c * v03 - s * v23;
        values[8] = c * v20 + s * v00;
        values[9] = c * v21 + s * v01;
        values[10] = c * v22 + s * v02;
        values[11] = c * v23 + s * v03;
    }

    void Matrix::zRotate(float angleInRadians)
    {
        float v00 = values[0 * 4 + 0];
        float v01 = values[0 * 4 + 1];
        float v02 = values[0 * 4 + 2];
        float v03 = values[0 * 4 + 3];
        float v10 = values[1 * 4 + 0];
        float v11 = values[1 * 4 + 1];
        float v12 = values[1 * 4 + 2];
        float v13 = values[1 * 4 + 3];
        float c = cos(angleInRadians);
        float s = sin(angleInRadians);

        values[0] = c * v00 + s * v10;
        values[1] = c * v01 + s * v11;
        values[2] = c * v02 + s * v12;
        values[3] = c * v03 + s * v13;
        values[4] = c * v10 - s * v00;
        values[5] = c * v11 - s * v01;
        values[6] = c * v12 - s * v02;
        values[7] = c * v13 - s * v03;
    }

    void Matrix::scale(float sx, float sy, float sz)
    {
        values[0] = sx * values[0 * 4 + 0];
        values[1] = sx * values[0 * 4 + 1];
        values[2] = sx * values[0 * 4 + 2];
        values[3] = sx * values[0 * 4 + 3];
        values[4] = sy * values[1 * 4 + 0];
        values[5] = sy * values[1 * 4 + 1];
        values[6] = sy * values[1 * 4 + 2];
        values[7] = sy * values[1 * 4 + 3];
        values[8] = sz * values[2 * 4 + 0];
        values[9] = sz * values[2 * 4 + 1];
        values[10] = sz * values[2 * 4 + 2];
        values[11] = sz * values[2 * 4 + 3];
    }

    void Matrix::inverse()
    {
        float m00 = values[0 * 4 + 0];
        float m01 = values[0 * 4 + 1];
        float m02 = values[0 * 4 + 2];
        float m03 = values[0 * 4 + 3];
        float m10 = values[1 * 4 + 0];
        float m11 = values[1 * 4 + 1];
        float m12 = values[1 * 4 + 2];
        float m13 = values[1 * 4 + 3];
        float m20 = values[2 * 4 + 0];
        float m21 = values[2 * 4 + 1];
        float m22 = values[2 * 4 + 2];
        float m23 = values[2 * 4 + 3];
        float m30 = values[3 * 4 + 0];
        float m31 = values[3 * 4 + 1];
        float m32 = values[3 * 4 + 2];
        float m33 = values[3 * 4 + 3];
        float tmp_0 = m22 * m33;
        float tmp_1 = m32 * m23;
        float tmp_2 = m12 * m33;
        float tmp_3 = m32 * m13;
        float tmp_4 = m12 * m23;
        float tmp_5 = m22 * m13;
        float tmp_6 = m02 * m33;
        float tmp_7 = m32 * m03;
        float tmp_8 = m02 * m23;
        float tmp_9 = m22 * m03;
        float tmp_10 = m02 * m13;
        float tmp_11 = m12 * m03;
        float tmp_12 = m20 * m31;
        float tmp_13 = m30 * m21;
        float tmp_14 = m10 * m31;
        float tmp_15 = m30 * m11;
        float tmp_16 = m10 * m21;
        float tmp_17 = m20 * m11;
        float tmp_18 = m00 * m31;
        float tmp_19 = m30 * m01;
        float tmp_20 = m00 * m21;
        float tmp_21 = m20 * m01;
        float tmp_22 = m00 * m11;
        float tmp_23 = m10 * m01;

        float t0 = tmp_0 * m11 + tmp_3 * m21 + tmp_4 * m31 - (tmp_1 * m11 + tmp_2 * m21 + tmp_5 * m31);
        float t1 = tmp_1 * m01 + tmp_6 * m21 + tmp_9 * m31 - (tmp_0 * m01 + tmp_7 * m21 + tmp_8 * m31);
        float t2 = tmp_2 * m01 + tmp_7 * m11 + tmp_10 * m31 - (tmp_3 * m01 + tmp_6 * m11 + tmp_11 * m31);
        float t3 = tmp_5 * m01 + tmp_8 * m11 + tmp_11 * m21 - (tmp_4 * m01 + tmp_9 * m11 + tmp_10 * m21);

        float d = 1.0 / (m00 * t0 + m10 * t1 + m20 * t2 + m30 * t3);

        values[0] = d * t0;
        values[1] = d * t1;
        values[2] = d * t2;
        values[3] = d * t3;
        values[4] = d * (tmp_1 * m10 + tmp_2 * m20 + tmp_5 * m30 - (tmp_0 * m10 + tmp_3 * m20 + tmp_4 * m30));
        values[5] = d * (tmp_0 * m00 + tmp_7 * m20 + tmp_8 * m30 - (tmp_1 * m00 + tmp_6 * m20 + tmp_9 * m30));
        values[6] = d * (tmp_3 * m00 + tmp_6 * m10 + tmp_11 * m30 - (tmp_2 * m00 + tmp_7 * m10 + tmp_10 * m30));
        values[7] = d * (tmp_4 * m00 + tmp_9 * m10 + tmp_10 * m20 - (tmp_5 * m00 + tmp_8 * m10 + tmp_11 * m20));
        values[8] = d * (tmp_12 * m13 + tmp_15 * m23 + tmp_16 * m33 - (tmp_13 * m13 + tmp_14 * m23 + tmp_17 * m33));
        values[9] = d * (tmp_13 * m03 + tmp_18 * m23 + tmp_21 * m33 - (tmp_12 * m03 + tmp_19 * m23 + tmp_20 * m33));
        values[10] = d * (tmp_14 * m03 + tmp_19 * m13 + tmp_22 * m33 - (tmp_15 * m03 + tmp_18 * m13 + tmp_23 * m33));
        values[11] = d * (tmp_17 * m03 + tmp_20 * m13 + tmp_23 * m23 - (tmp_16 * m03 + tmp_21 * m13 + tmp_22 * m23));
        values[12] = d * (tmp_14 * m22 + tmp_17 * m32 + tmp_13 * m12 - (tmp_16 * m32 + tmp_12 * m12 + tmp_15 * m22));
        values[13] = d * (tmp_20 * m32 + tmp_12 * m02 + tmp_19 * m22 - (tmp_18 * m22 + tmp_21 * m32 + tmp_13 * m02));
        values[14] = d * (tmp_18 * m12 + tmp_23 * m32 + tmp_15 * m02 - (tmp_22 * m32 + tmp_14 * m02 + tmp_19 * m12));
        values[15] = d * (tmp_22 * m22 + tmp_16 * m02 + tmp_21 * m12 - (tmp_20 * m12 + tmp_23 * m22 + tmp_17 * m02));
    }

    void Matrix::multiply(const Matrix& a, const Matrix& b, Matrix& dst)
    {
        float b00 = b[0 * 4 + 0];
        float b01 = b[0 * 4 + 1];
        float b02 = b[0 * 4 + 2];
        float b03 = b[0 * 4 + 3];
        float b10 = b[1 * 4 + 0];
        float b11 = b[1 * 4 + 1];
        float b12 = b[1 * 4 + 2];
        float b13 = b[1 * 4 + 3];
        float b20 = b[2 * 4 + 0];
        float b21 = b[2 * 4 + 1];
        float b22 = b[2 * 4 + 2];
        float b23 = b[2 * 4 + 3];
        float b30 = b[3 * 4 + 0];
        float b31 = b[3 * 4 + 1];
        float b32 = b[3 * 4 + 2];
        float b33 = b[3 * 4 + 3];
        float a00 = a[0 * 4 + 0];
        float a01 = a[0 * 4 + 1];
        float a02 = a[0 * 4 + 2];
        float a03 = a[0 * 4 + 3];
        float a10 = a[1 * 4 + 0];
        float a11 = a[1 * 4 + 1];
        float a12 = a[1 * 4 + 2];
        float a13 = a[1 * 4 + 3];
        float a20 = a[2 * 4 + 0];
        float a21 = a[2 * 4 + 1];
        float a22 = a[2 * 4 + 2];
        float a23 = a[2 * 4 + 3];
        float a30 = a[3 * 4 + 0];
        float a31 = a[3 * 4 + 1];
        float a32 = a[3 * 4 + 2];
        float a33 = a[3 * 4 + 3];
        dst[0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
        dst[1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
        dst[2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
        dst[3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
        dst[4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
        dst[5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
        dst[6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
        dst[7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
        dst[8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
        dst[9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
        dst[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
        dst[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
        dst[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
        dst[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
        dst[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
        dst[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;
    }

    /**
    * Computes an orthographic projection matrix.
    *
    * @param m 存放结果矩阵的数组
    * @param mOffset 结果矩阵的起始偏移值
    * @param left x轴最小范围
    * @param right x轴最大范围
    * @param bottom y轴最小范围
    * @param top y轴最大范围
    * @param near z轴最小范围
    * @param far z轴最大范围
    */
    void Matrix::orthoM(Matrix& m, float left, float right, float bottom, float top, float nearVal, float farVal) {
        // 创建一个4x4正交投影矩阵
        m[0] = 2.0f / (right - left);
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;

        m[4] = 0.0f;
        m[5] = 2.0f / (top - bottom);
        m[6] = 0.0f;
        m[7] = 0.0f;

        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = -2.0f / (farVal - nearVal);
        m[11] = 0.0f;

        m[12] = -(right + left) / (right - left);
        m[13] = -(top + bottom) / (top - bottom);
        m[14] = -(farVal + nearVal) / (farVal - nearVal);
        m[15] = 1.0f;
    }

    void Matrix::orthoM(Matrix& m, float left, float right, float bottom, float top) {
        // 创建一个4x4正交投影矩阵
        m[0] = 2.0f / (right - left);
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;

        m[4] = 0.0f;
        m[5] = 2.0f / (top - bottom);
        m[6] = 0.0f;
        m[7] = 0.0f;

        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;

        m[12] = -(right + left) / (right - left);
        m[13] = -(top + bottom) / (top - bottom);
        m[14] = 1.0f;
        m[15] = 1.0f;
    }

    void Matrix::set(Matrix& m, float scaleX, float scaleY, float translateX, float translateY) {
        // 创建一个4x4正交投影矩阵
        m[0] = scaleX;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;

        m[4] = 0.0f;
        m[5] = scaleY;
        m[6] = 0.0f;
        m[7] = 0.0f;

        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;

        m[12] = translateX;
        m[13] = translateY;
        m[14] = 0.0f;
        m[15] = 1.0f;
    }

    void Matrix::setFillMode(Matrix &orthoMmatrix, int fillMode, 
                             float width, float height,
                             float surfaceWidth,
                             float surfaceHeight) {
        if (fillMode == 0) {
            float aspectRatio = surfaceWidth > surfaceHeight ?
                                (float)surfaceWidth / (float)surfaceHeight : (float)surfaceHeight / (float)surfaceWidth;
            if (surfaceWidth > surfaceHeight){
                Matrix::orthoM(orthoMmatrix, -aspectRatio, aspectRatio, -1.0f, 1.0f);
            } else {
                Matrix::orthoM(orthoMmatrix,  -1.0f, 1.0f, -aspectRatio, aspectRatio);
            }
        } else if (fillMode == 1) {
            float widthRatio = surfaceWidth / width;
            float heightRatio = surfaceHeight / height;
            float ratio = 1.0f;
            if (widthRatio > heightRatio) {
                ratio = widthRatio;
            } else {
                ratio = heightRatio;
            }
            float scaleWidth = ratio * width;
            float scaleHeight = ratio * height;
            if (scaleWidth > surfaceWidth) {
                float translateX = (1.0f - scaleWidth/surfaceWidth) / 2.0f;
                float translateY = 0.0f;
                Matrix::set(orthoMmatrix, ratio, 1.0f, translateX, translateY);
            } else {
                float translateX = 0.0f;
                float translateY = (1.0f - scaleHeight/surfaceHeight) / 2.0f;
                Matrix::set(orthoMmatrix, 1.0f , ratio, translateX, translateY);
            }
        } else if (fillMode == 2) {
            orthoMmatrix = Matrix();
        } else if (fillMode == 3) {
            float widthRatio = surfaceWidth / width;
            float heightRatio = surfaceHeight / height;
            float ratio = 1.0f;
            if (widthRatio > heightRatio) {
                ratio = widthRatio;
            } else {
                ratio = heightRatio;
            }
            float scaleWidth = ratio * width;
            float scaleHeight = ratio * height;
            if (scaleWidth > surfaceWidth) {
                float translateX = (1.0f - scaleWidth/surfaceWidth) / 2.0f;
                float translateY = 0.0f;
                Matrix::set(orthoMmatrix, ratio, 1.0f, translateX, translateY);
            } else {
                float translateX = 0.0f;
                float translateY = (1.0f - scaleHeight/surfaceHeight) / 2.0f;
                Matrix::set(orthoMmatrix, 1.0f , ratio, translateX, translateY);
            }
        }
    }

    Matrix &Matrix::operator=(const Matrix &m)
    {
        if (this != &m)
        {
            m.getValue(values);
        }

        return *this;
    }
}