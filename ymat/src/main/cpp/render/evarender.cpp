//
// Created by Cangwang on 2024/4/10.
//

#include "evarender.h"

EvaRender::EvaRender(): vertexArray(make_shared<GlFloatArray>()), alphaArray(make_shared<GlFloatArray>()), rgbArray(make_shared<GlFloatArray>())  {
    EvaRender::initRender();
}

EvaRender::~EvaRender() {

}

void EvaRender::setLayerInfo(SimpleLayerInfo info) {

}

void EvaRender::initRender() {
    char VERTEX_SHADER[] = R"(
            attribute vec4 vPosition;
            attribute vec4 vTexCoordinateAlpha;
            attribute vec4 vTexCoordinateRgb;
            varying vec2 v_TexCoordinateAlpha;
            varying vec2 v_TexCoordinateRgb;
            void main() {
                v_TexCoordinateAlpha = vec2(vTexCoordinateAlpha.x, vTexCoordinateAlpha.y);
                v_TexCoordinateRgb = vec2(vTexCoordinateRgb.x, vTexCoordinateRgb.y);
                gl_Position = vPosition;
            }
    )";

    char FRAGMENT_SHADER[] = R"(
        #extension GL_OES_EGL_image_external : require
        precision mediump float;
        uniform samplerExternalOES texture;
        varying vec2 v_TexCoordinateAlpha;
        varying vec2 v_TexCoordinateRgb;
        void main () {
            vec4 alphaColor = texture2D(texture, v_TexCoordinateAlpha);
            vec4 rgbColor = texture2D(texture, v_TexCoordinateRgb);
            gl_FragColor = vec4(rgbColor.r, rgbColor.g, rgbColor.b, alphaColor.r);
        }
    )";

    shaderProgram = ShaderUtil::createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    uTextureLocation = glGetUniformLocation(shaderProgram, "texture");
    aPositionLocation = glGetAttribLocation(shaderProgram, "vPosition");
    aTextureAlphaLocation = glGetAttribLocation(shaderProgram, "vTexCoordinateAlpha");
    aTextureRgbLocation = glGetAttribLocation(shaderProgram, "vTexCoordinateRgb");

    Render::initRender();
}

void EvaRender::draw() {
    if (textureId != -1) {
        glUseProgram(shaderProgram);
        vertexArray->setVertexAttribPointer(aPositionLocation);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId);
        //加载纹理
        glUniform1i(uTextureLocation, 0);

        alphaArray->setVertexAttribPointer(aTextureAlphaLocation);
        rgbArray->setVertexAttribPointer(aTextureRgbLocation);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}
