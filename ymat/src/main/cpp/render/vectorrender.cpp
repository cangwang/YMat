//
// Created by Cangwang on 2024/4/10.
//

#include "vectorrender.h"


VectorRender::VectorRender() {

}

VectorRender::~VectorRender() {

}

void VectorRender::initRender() {
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
    Render::initRender();
}

void VectorRender::setLayerInfo(SimpleLayerInfo info) {

}

void VectorRender::draw() {

}

void VectorRender::initRecordFrameBuffer(int width, int height) {
    //比对大小
    if (mFrameWidth != width || mFrameHeight !=height){
        destroyRecordFrameBuffer();
    }
    mFrameWidth = width;
    mFrameHeight = height;
    mFrameBuffer = 0;
    mFrameBufferTextures = 0;
    //生成帧缓冲id
    glGenFramebuffers(1,&mFrameBuffer);
    //生成纹理id
    glGenTextures(1,&mFrameBufferTextures);
    //绑定纹理
    glBindTexture(GL_TEXTURE_2D,mFrameBufferTextures);
    //纹理赋值为空，先纹理占位
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE, nullptr);
    //设定纹理参数
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    //绑定帧图
    glBindFramebuffer(GL_FRAMEBUFFER,mFrameBuffer);
    //绑定纹理到帧图
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,mFrameBufferTextures,0);
    //切换回默认纹理
    glBindTexture(GL_TEXTURE_2D,0);
    //切换回默认的帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    YMLOGV("framebufferTextures %d", mFrameBufferTextures);
}

void VectorRender::destroyRecordFrameBuffer() {
    glDeleteTextures(1, &mFrameBufferTextures);
    glDeleteFramebuffers(1, &mFrameBuffer);
    mFrameWidth = -1;
    mFrameHeight = -1;
    YMLOGV("destroyRecordFrameBuffer");
}
