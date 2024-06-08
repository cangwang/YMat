//
// Created by Cangwang on 2024/5/20.
//

#include "ElementCpuDrawer.h"
#define LOG_TAG "ElementCpuDrawer"

ElementCpuDrawer::~ElementCpuDrawer() {

}

void ElementCpuDrawer::init() {
    char VERTEX_SHADER[] = R"(#version 300 es
        in vec4 vPosition;
        in vec2 vTexCoordinate;
        out vec2 v_TexCoordinate;
        uniform mat4 mvpMatrix;
        void main() {
            v_TexCoordinate = vTexCoordinate;
            gl_Position = mvpMatrix * vPosition;
        }
    )";
    char FRAGMENT_SHADER[] = R"(#version 300 es
        precision mediump float;
        uniform sampler2D shapeTexture;
        in vec2 v_TexCoordinate;
        out vec4 fragColor;
        void main() {
//            fragColor = vec4(1.0, 0.0, 0.0, 1.0);
            fragColor = texture(shapeTexture, v_TexCoordinate);
        }
    )";
    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "shapeTexture");
    positionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    textureLocation = glGetAttribLocation(shaderProgram, "vTexCoordinate");
    matrixLocation = glGetUniformLocation(shaderProgram,"mvpMatrix");
}

void ElementCpuDrawer::draw(shared_ptr<Matrix> matrix, shared_ptr<FrameInfo> frameInfo) {
    YMLOGV("draw name %s", shapeContent->name.c_str());
    if (shapeContent && shapeContent->textureInfo && shapeContent->textureInfo->textureId != 0) {
        // 着色器
        glUseProgram(shaderProgram);
        // 顶点
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        // 矩阵设置
        glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, matrix->values);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shapeContent->textureInfo->textureId);
        // 纹理坐标
        glUniform1i(uTextureLocation, 0);
        glEnableVertexAttribArray(textureLocation);
        glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, texturecoord);
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
    }
}

void ElementCpuDrawer::destroy() {
    if (shapeContent && shapeContent->textureInfo && shapeContent->textureInfo != 0) {
        glDeleteTextures(1, &shapeContent->textureInfo->textureId);
    }
    glDeleteProgram(shaderProgram);
}