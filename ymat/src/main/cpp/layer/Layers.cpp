//
// Created by Cangwang on 2024/5/9.
//

#include "Layers.h"

#include <utils/shaderutil.h>
#include <utils/TrackMatteType.h>
#include <utils/blendmode.h>

#define LOG_TAG "Layers"

Layers::Layers(shared_ptr<AbstractDrawer> drawer): drawer(drawer) {

}

Layers::~Layers() {
    destroy();
}

void Layers::setParentWidthHeight(float width, float height) {
    this->parentWidth = width;
    this->parentHeight = height;
}

void Layers::setMaskLayer(shared_ptr<Layers> layer, int trackMatteType) {
    this->maskLayer = layer;
    this->maskLayer->init();
    this->trackMatteType = trackMatteType;
    initMaskProgram();
    initImageProgram();
}

void Layers::setBlendMode(int blendMode) {
    this->blendMode = blendMode;
    if (blendMode > 0) {
        initBlendProgram();
    }
}


void Layers::init() {
    drawer->init();
}

void Layers::render(shared_ptr<Matrix> parentMatrix, shared_ptr<FrameInfo> frameInfo, bool fromMask) {
    if (drawer->shapeContent || (drawer->info && frameInfo->frameId >= drawer->info->inFrame
                                 && frameInfo->frameId <= drawer->info->outFrame)) {

        // 算矩阵
        auto localMatrix = drawer->getMatrix(frameInfo->frameId);
        if (!localMatrix) {
            YMLOGV("localMatrix null");
            return;
        }
        float opacity = drawer->getOpacity(frameInfo->frameId, frameInfo->opacity);
        if (drawMaskBlend(localMatrix, opacity, frameInfo, parentMatrix)) return;

        shared_ptr<Matrix> matrix = make_shared<Matrix>();
        Matrix::multiply(*parentMatrix, *localMatrix, *matrix);
        // 矩阵设置
//    float values[16] = {0};
//    localMatrix->getValue(values);
//    YMLOGV("matrix %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f",
//           values[0], values[1], values[2], values[3], values[4],values[5], values[6], values[7],
//           values[8], values[9], values[10],values[11], values[12],values[13], values[14], values[15]);

//    YMLOGV("render frameInfo texture %d", frameInfo->framebuffer->texture);

        frameInfo->framebuffer->bind();
        drawer->draw(matrix, frameInfo);
        frameInfo->framebuffer->unbind();
    }


//    if (maskLayer) {
//        maskLayer->render(parentMatrix, frameInfo);
//    }
}

void Layers::destroy() {
    if (maskLayer) {
        maskLayer->destroy();
    }
    if (drawer) {
        drawer->destroy();
    }
    glDeleteProgram(mShaderProgram);
    glDeleteProgram(shaderProgram);
}

bool Layers::drawMaskBlend(shared_ptr<Matrix> localMatrix, float opacity,
                           shared_ptr<FrameInfo> frameInfo, shared_ptr<Matrix> parentMatrix) {
    if (blendMode == 0 && !maskLayer) return false;
    YMLOGV("drawMaskBlend blendMode %d", blendMode);

    parentWidth = frameInfo->width;
    parentHeight = frameInfo->height;
    auto parentFrameBuffer = frameInfo->framebuffer;

    shared_ptr<Matrix> parentCamera = make_shared<Matrix>();
    Matrix::perspectiveCamera(*parentCamera, parentWidth, parentHeight, 40);

    shared_ptr<Matrix> matrix = make_shared<Matrix>();
    Matrix::multiply(*parentCamera, *localMatrix, *matrix);
    if (!srcFrameInfo) {
        srcFrameInfo = make_shared<FrameInfo>();
        srcFrameInfo->frameId = frameInfo->frameId;
        srcFrameInfo->width = frameInfo->width;
        srcFrameInfo->height = frameInfo->height;
        srcFrameInfo->opacity = frameInfo->opacity;
        auto srcFrameBuffer = make_shared<FrameBuffer>();
        srcFrameBuffer->init(parentWidth, parentHeight);
        srcFrameInfo->framebuffer = srcFrameBuffer;
    } else {
        srcFrameInfo->frameId = frameInfo->frameId;
        srcFrameInfo->opacity = frameInfo->opacity;
        srcFrameInfo->framebuffer->clear();
    }
    srcFrameInfo->framebuffer->bind();
    if (drawer->info) {
        YMLOGV("drawMaskBlend srcFrame %s", drawer->info->name.c_str());
    }
    drawer->draw(matrix, srcFrameInfo);
    srcFrameInfo->framebuffer->unbind();

    auto maskResultFrameBuffer = make_shared<FrameBuffer>();
    maskResultFrameBuffer->init(parentWidth, parentHeight);
    if (maskLayer) {
        if (!maskFrameInfo) {
            auto maskFrameBuffer = make_shared<FrameBuffer>();
            maskFrameBuffer->init(parentWidth, parentHeight);
            maskFrameBuffer->bind();
            maskFrameInfo = make_shared<FrameInfo>();
            maskFrameInfo->frameId = frameInfo->frameId;
            maskFrameInfo->width = parentWidth;
            maskFrameInfo->height = parentHeight;
            maskFrameInfo->opacity = 1.0;
            maskFrameInfo->framebuffer = maskFrameBuffer;
        } else {
            maskFrameInfo->frameId = frameInfo->frameId;
            maskFrameInfo->framebuffer->clear();
        }
        if (maskLayer->drawer->info) {
            YMLOGV("drawMaskBlend maskLayer %s", maskLayer->drawer->info->name.c_str());
        }
        maskLayer->render(parentMatrix, maskFrameInfo);
        maskFrameInfo->framebuffer->unbind();

        maskResultFrameBuffer->bind();
//        parentFrameBuffer->bind();

        // 着色器
        glUseProgram(mShaderProgram);
        // 顶点
        glEnableVertexAttribArray(mPositionLocation);
        glVertexAttribPointer(mPositionLocation, 2, GL_FLOAT, GL_FALSE, 0, textureVertices);
        //原纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcFrameInfo->framebuffer->texture);
        // 纹理坐标
        glUniform1i(muTextureLocation, 0);
        //遮罩纹理
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, maskFrameInfo->framebuffer->texture);
        // 纹理坐标
        glUniform1i(muMaskTextureLocation, 1);

        glEnableVertexAttribArray(mTextureLocation);
        glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, GL_FALSE, 0, texture);
        //启动混合
        glEnable(GL_BLEND);
        //基于alpha通道的半透明混合函数
//        void glBlendFuncSeparate(GLenum srcRGB,
//             GLenum dstRGB,
//             GLenum srcAlpha,
//             GLenum dstAlpha);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        //绘制
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //关闭混合
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

//        parentFrameBuffer->unbind();
        maskResultFrameBuffer->unbind();
    }

    if (blendMode) {
//        YMLOGV("drawMaskBlend dstTexture %d，parentFrameTexture %d", dstTexture, parentFrameBuffer->texture);
        auto parentFm = make_shared<FrameBuffer>();
        parentFm->init(frameInfo->width, frameInfo->height);
        parentFm->bind();
//        parentFrameBuffer->bind();

        // 着色器
        glUseProgram(shaderProgram);
        // 顶点
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, textureVertices);
        //原纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcFrameInfo->framebuffer->texture);
        // 纹理坐标
        glUniform1i(uTextureLocation, 0);
        //遮罩纹理
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, parentFrameBuffer->texture);
        // 纹理坐标
        glUniform1i(uDstTextureLocation, 1);

        glEnableVertexAttribArray(textureLocation);
        glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, texture);
        //启动混合
        glEnable(GL_BLEND);
        //基于alpha通道的半透明混合函数
//        void glBlendFuncSeparate(GLenum srcRGB,
//             GLenum dstRGB,
//             GLenum srcAlpha,
//             GLenum dstAlpha);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        //绘制
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //关闭混合
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
//        parentFrameBuffer->unbind();
        parentFm->unbind();
        frameInfo->framebuffer->destroy();
        frameInfo->framebuffer = parentFm;
    } else {
        updateImageFramebufferVertices(parentWidth, parentHeight);
        parentFrameBuffer->bind();
        // 着色器
        glUseProgram(shaderProgram);
        // 顶点
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        //原纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, maskResultFrameBuffer->texture);
        // 纹理坐标
        glUniform1i(uTextureLocation, 0);

        glEnableVertexAttribArray(textureLocation);
        glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, texturecoord);
        float values[16] = {0};
        parentMatrix->getValue(values);
        glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, values);
        glUniform1i(isAlpha, 0);
        glUniform1f(this->opacity, opacity);

        //启动混合
        glEnable(GL_BLEND);
        //基于alpha通道的半透明混合函数
//        void glBlendFuncSeparate(GLenum srcRGB,
//             GLenum dstRGB,
//             GLenum srcAlpha,
//             GLenum dstAlpha);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        //绘制
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //关闭混合
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        parentFrameBuffer->unbind();
    }

    maskResultFrameBuffer->destroy();

    return true;
}

void Layers::updateImageFramebufferVertices(float width, float height) {
    const float x1 = 0;
    const float y1 = -height;
    const float x2 = width;
    const float y2 = 0;
    vertices[0] = x1;
    vertices[1] = y1;
    vertices[2] = x2;
    vertices[3] = y1;
    vertices[4] = x1;
    vertices[5] = y2;
    vertices[6] = x2;
    vertices[7] = y2;
}

void Layers::initMaskProgram() {
    char VERTEX_SHADER[] = R"(#version 300 es
        in vec2 a_position;
        in vec2 a_texcoord;
        out vec2 v_texcoord;
        uniform mat4 mvpMatrix;
        void main() {
            gl_Position = vec4(a_position, 0, 1.0);
            v_texcoord = a_texcoord;
        }
    )";

    if (trackMatteType == TrackMatteType::ALPHA) {
        char FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;
            uniform sampler2D u_maskTexture;

            out vec4 fragColor;

            void main(void) {
                vec4 texColor = texture(u_texture, v_texcoord);
                vec4 maskColor = texture(u_maskTexture, v_texcoord);

                float alpha = maskColor.a;
                fragColor = texColor * alpha;
            }
        )";
        mShaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    } else if (trackMatteType == TrackMatteType::ALPHA_INVERTED){
        char FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;
            uniform sampler2D u_maskTexture;

            out vec4 fragColor;

            void main(void) {
                vec4 texColor = texture(u_texture, v_texcoord);
                vec4 maskColor = texture(u_maskTexture, v_texcoord);

                float alpha = maskColor.a;
                fragColor = texColor * (1.0 - alpha);
            }
        )";
        mShaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    } else if (trackMatteType == TrackMatteType::LUMA) {
        char FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;
            uniform sampler2D u_maskTexture;

            out vec4 fragColor;

            void main(void) {
                vec4 texColor = texture(u_texture, v_texcoord);
                vec4 maskColor = texture(u_maskTexture, v_texcoord);

                float brightness = maskColor.r * 0.3 + maskColor.g * 0.6 + maskColor.b * 0.1;
                fragColor = texColor * brightness;
            }
        )";
        mShaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    } else if (trackMatteType == TrackMatteType::LUMA_INVERTED){
        char FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;
            uniform sampler2D u_maskTexture;

            out vec4 fragColor;

            void main(void) {
                vec4 texColor = texture(u_texture, v_texcoord);
                vec4 maskColor = texture(u_maskTexture, v_texcoord);

                float brightness = maskColor.r * 0.3 + maskColor.g * 0.6 + maskColor.b * 0.1;
                fragColor = texColor * (1.0 - brightness);
            }
        )";
        mShaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    } else {
        YMLOGE("trackMatteType is null");
        return;
    }

    muTextureLocation = glGetUniformLocation(mShaderProgram, "u_texture");
    muMaskTextureLocation = glGetUniformLocation(mShaderProgram, "u_maskTexture");
    mPositionLocation = glGetAttribLocation(mShaderProgram, "a_position");
    mTextureLocation = glGetAttribLocation(mShaderProgram, "a_texcoord");
}

void Layers::initBlendProgram() {
    char VERTEX_SHADER[] = R"(#version 300 es
        in vec4 a_position;
        in vec2 a_texcoord;
        out vec2 v_texcoord;
        void main() {
            gl_Position = a_position;
            v_texcoord = a_texcoord;
        }
    )";

    if (blendMode == BlendMode::ADD) {
        char FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;
            uniform sampler2D u_dstTexture;

            out vec4 fragColor;

            void main(void) {
                vec4 src = texture(u_texture, v_texcoord);
                vec4 dst = texture(u_dstTexture, v_texcoord);
                float noSrc = step(src.a, 0.001);
                fragColor = noSrc*dst + (1.0-noSrc)*vec4(dst.rgb + src.rgb, dst.a);
            }
        )";
        shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    } else if (blendMode == BlendMode::SCREEN) {
        char FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;
            uniform sampler2D u_dstTexture;

            out vec4 fragColor;

            void main(void) {
                vec4 src = texture(u_texture, v_texcoord);
                vec4 dst = texture(u_dstTexture, v_texcoord);
                float noSrc = step(src.a, 0.001);
                fragColor = noSrc*dst + (1.0-noSrc)*(1.0-(1.0-dst)*(1.0-src));
            }
        )";
        shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    } else if (blendMode == BlendMode::OVERLAY) {
        char FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;
            uniform sampler2D u_dstTexture;

            out vec4 fragColor;

            void main(void) {
                vec4 src = texture(u_texture, v_texcoord);
                vec4 dst = texture(u_dstTexture, v_texcoord);
                float noSrc = step(src.a, 0.001);
                vec4 flag = step(dst, vec4(0.5, 0.5, 0.5, 0.5));
                fragColor = noSrc*dst + (1.0-noSrc)*(flag*dst*src*2.0 + (1.0-flag)*(1.0-(1.0-dst)*(1.0-src)*2.0));
            }
        )";
        shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    } else if (blendMode == BlendMode::SOFT_LIGHT) {
        char FRAGMENT_SHADER[] = R"(#version 300 es
            precision mediump float;

            in vec2 v_texcoord;

            uniform sampler2D u_texture;
            uniform sampler2D u_dstTexture;

            out vec4 fragColor;

            void main(void) {
                vec4 src = texture(u_texture, v_texcoord);
                vec4 dst = texture(u_dstTexture, v_texcoord);
                float noSrc = step(src.a, 0.001);
                vec4 flag = step(src, vec4(0.5, 0.5, 0.5, 0.5));
                fragColor = noSrc*dst + (1.0-noSrc)*(flag*(dst*src*2.0+dst*dst*(1.0-src*2.0))+(1.0-flag)*(dst*(1.0-src)*2.0+sqrt(dst)*(2.0*src-1.0)));
            }
        )";
        shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    }
    uDstTextureLocation = glGetUniformLocation(shaderProgram, "u_dstTexture");
    uTextureLocation = glGetUniformLocation(shaderProgram, "u_texture");
    positionLocation = glGetAttribLocation(shaderProgram, "a_position");
    textureLocation = glGetAttribLocation(shaderProgram, "a_texcoord");
}

void Layers::initImageProgram() {
    shaderProgram = ShaderUtil::createProgram(TextureLoadUtil::SIMPLE_VERTEX_SHADER, TextureLoadUtil::IMAGE_FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "u_texture");
    positionLocation = glGetAttribLocation(shaderProgram, "a_position");
    textureLocation = glGetAttribLocation(shaderProgram, "a_texcoord");
    matrixLocation = glGetUniformLocation(shaderProgram,"u_matrix");
    isAlpha = glGetUniformLocation(shaderProgram, "u_isAlpha");
    opacity = glGetUniformLocation(shaderProgram, "u_opacity");
}