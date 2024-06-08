//
// Created by zengjiale on 2022/4/21.
//
#pragma once
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <android/bitmap.h>
#include <EGL/egl.h>
#include <utils/ymlog.h>
#include <bean/TextureInfo.h>

using namespace ymat;
namespace ymat {
    class TextureLoadUtil {
    public:
        static const int NONE_TEXTURE = -1;
        static shared_ptr<TextureInfo> loadTexture(unsigned char *bitmap, AndroidBitmapInfo *info);

        static void updateTexture(shared_ptr<TextureInfo> textureInfo, unsigned char *bitmap, AndroidBitmapInfo *info);

        static shared_ptr<TextureInfo> loadVideoTexture();

        static void releaseTexture(GLuint textureId);

        constexpr static const char SIMPLE_VERTEX_SHADER[] = R"(#version 300 es
             in vec4 a_position;  // 接受顶点坐标
            in vec2 a_texcoord;  // 接受纹理坐标

            uniform mat4 u_matrix;  // 顶点矩阵
            out vec2 v_texcoord;

            void main() {
                gl_Position = u_matrix * a_position;
                v_texcoord = a_texcoord;
            }
        )";

        constexpr static const char SIMPLE_FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;
            uniform sampler2D u_texture;
            in vec2 v_texcoord;
            out vec4 fragColor;
            void main () {
    //            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                fragColor = texture(u_texture, v_texcoord);
            }
        )";

        constexpr static const char IMAGE_FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;

            uniform float u_opacity;
            uniform int u_isAlpha;

            out vec4 fragColor;
            void main () {
    //            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                vec4 texColor = texture(u_texture, v_texcoord);
                texColor.a = texColor.a * u_opacity;

                float isAlpha = step(1.0, float(u_isAlpha)); // 0 或者 1
                float alpha = texture(u_texture, v_texcoord + vec2(0.5, 0.0)).r;
                texColor.a = texColor.a * (isAlpha * alpha + 1.0 - isAlpha);

                fragColor = texColor;
            }
        )";

        constexpr static const char ALPHA_IMAGE_FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;
            uniform sampler2D u_texture;
            in vec2 v_texcoord;
            out vec4 fragColor;
            uniform float u_opacity;
            void main () {
                vec4 texColor = texture(u_texture, v_texcoord);
                  float alpha = texture(u_texture, v_texcoord + vec2(0.5, 0.0)).r;
                  texColor.a = alpha * u_opacity;
                  fragColor = texColor;
            }
        )";

        constexpr static const char NORMAL_IMAGE_FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;
            uniform sampler2D u_texture;
            in vec2 v_texcoord;
            out vec4 fragColor;
            uniform float u_opacity;
            void main () {
                 vec4 texColor = texture(u_texture, v_texcoord);
                 texColor.a = texColor.a * u_opacity;
                 fragColor = texColor;
            }
        )";

        constexpr static const char VIDEO_FRAGMENT_SHADER[] = R"(#version 300 es
            #extension GL_OES_EGL_image_external_essl3 : require
            precision mediump float;
            uniform samplerExternalOES u_texture;
            in vec2 v_texcoord;
            out vec4 fragColor;
            uniform float u_opacity;
            uniform int u_isAlpha;
            void main () {
                vec4 texColor = texture(u_texture, v_texcoord);
                texColor.a = texColor.a * u_opacity;
                float isAlpha = step(1.0, float(u_isAlpha)); // 0 或者 1
                float alpha = texture(u_texture, v_texcoord + vec2(0.5, 0.0)).r;
                texColor.a = texColor.a * (isAlpha * alpha + 1.0 - isAlpha);
                fragColor = texColor;
            }
        )";

        constexpr static const char NORMAL_VIDEO_FRAGMENT_SHADER[] = R"(#version 300 es
            #extension GL_OES_EGL_image_external_essl3 : require
            precision mediump float;
            uniform samplerExternalOES u_texture;
            in vec2 v_texcoord;
            out vec4 fragColor;
            uniform float u_opacity;
            void main () {
                 vec4 texColor = texture(u_texture, v_texcoord);
                 texColor.a = texColor.a * u_opacity;
                 fragColor = texColor;
            }
        )";

        constexpr static const char ALPHA_VIDEO_FRAGMENT_SHADER[] = R"(#version 300 es
            #extension GL_OES_EGL_image_external_essl3 : require
            precision mediump float;
            uniform samplerExternalOES u_texture;
            in vec2 v_texcoord;
            out vec4 fragColor;
            uniform float u_opacity;
            void main () {
                  vec4 texColor = texture(u_texture, v_texcoord);
                  float alpha = texture(u_texture, v_texcoord + vec2(0.5, 0.0)).r;
                  texColor.a = alpha * u_opacity;

                  fragColor = texColor;
            }
        )";
    };
}

