//
// Created by Cangwang on 2024/5/9.
//

#pragma once
#include <memory>
#include <GLES3/gl3.h>
#include <utils/ymlog.h>

using namespace std;
namespace ymat {
    class FrameBuffer {
    public:
        FrameBuffer();
        ~FrameBuffer();
        GLuint init(float w, float h);
        void bind();
        void unbind();
        void viewport(float w, float h, bool noClr);
        GLuint reset(float width, float height);
        void clear();
        void destroy();

        GLuint texture = 0;
        GLuint buffer = 0;
        float w = 0;
        float h = 0;
    };
}