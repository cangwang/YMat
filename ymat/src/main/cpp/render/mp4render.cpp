//
// Created by Cangwang on 2024/4/10.
//

#include "mp4render.h"

ymat::Mp4Render::Mp4Render(): vertexArray(make_shared<GlFloatArray>()), rgbaArray(make_shared<GlFloatArray>()) {
    Mp4Render::initRender();
}

ymat::Mp4Render::~Mp4Render() {

}

void ymat::Mp4Render::initRender() {
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
            #extension GL_OES_EGL_image_external : require
            precision mediump float;
            uniform samplerExternalOES texture;
            varying vec2 v_TexCoordinate;\n"
            void main () {
                gl_FragColor = texture2D(texture, v_TexCoordinate);
            }
    )";
    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "texture");
    positionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    textureLocation = glGetAttribLocation(shaderProgram, "vTexCoordinate");

    Render::initRender();
}

void Mp4Render::setLayerInfo(SimpleLayerInfo info) {

}


void ymat::Mp4Render::draw() {
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
