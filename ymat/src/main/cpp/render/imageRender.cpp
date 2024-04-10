//
// Created by Cangwang on 2024/4/10.
//

#include "imageRender.h"

ImageRender::ImageRender(): vertexArray(make_shared<GlFloatArray>()), rgbaArray(make_shared<GlFloatArray>()) {
    ImageRender::initRender();
}

ImageRender::~ImageRender() {

}

void ImageRender::initRender() {
    char VERTEX_SHADER[] = R"(
        attribute vec4 vPosition;
        attribute vec4 vTexCoordinate;
        varying vec2 v_TexCoordinate;
        void main() {
            v_TexCoordinate = vec2(vTexCoordinate.x, vTexCoordinate.y);
            gl_Position = vPosition;
        }
    )";
    char FRAGMENT_SHADER[] = R"(
        precision mediump float;
        uniform sampler2D texture;
        varying vec2 v_TexCoordinate;
        void main () {
            gl_FragColor = texture2D(texture, v_TexCoordinate);
        }
    )";
    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "texture");
    positionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    textureLocation = glGetAttribLocation(shaderProgram, "vTexCoordinate");
}

void ImageRender::setLayerInfo(SimpleLayerInfo info) {

}

void ImageRender::draw() {
    if (textureId != -1) {
        glUseProgram(shaderProgram);
        vertexArray->setVertexAttribPointer(positionLocation);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        //加载纹理
        glUniform1i(uTextureLocation, 0);
        rgbaArray->setVertexAttribPointer(textureLocation);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}
