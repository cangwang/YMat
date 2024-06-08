//
// Created by Cangwang on 2024/5/9.
//

#include "FrameBuffer.h"
#define LOG_TAG "FrameBuffer"

ymat::FrameBuffer::FrameBuffer() {

}

ymat::FrameBuffer::~FrameBuffer() {
    destroy();
}

void ymat::FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER,buffer);
}

void ymat::FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER,GL_NONE);
}

void ymat::FrameBuffer::clear() {
    bind();
    //清空脏数据
    glClear(GL_COLOR_BUFFER_BIT);
    unbind();
}

GLuint ymat::FrameBuffer::init(float w, float h) {
    if (this->w != w || this->h != h) {
        this->w = w;
        this->h = h;
        destroy();
    }

    //生成帧缓冲id
    glGenFramebuffers(1, &buffer);
    //生成纹理id
    glGenTextures(1,&texture);
    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    //纹理赋值为空，先纹理占位
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h,0,GL_RGBA,GL_UNSIGNED_BYTE, nullptr);
    //设定纹理参数
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    //绑定帧图
    glBindFramebuffer(GL_FRAMEBUFFER,buffer);
    //绑定纹理到帧图
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, texture,0);
    //清空脏数据
    glClear(GL_COLOR_BUFFER_BIT);
    //切换回默认纹理
    glBindTexture(GL_TEXTURE_2D,GL_NONE);
    //切换回默认的帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER,GL_NONE);
    YMLOGV("FrameBuffer init %d", texture);
    return texture;
}

void ymat::FrameBuffer::viewport(float w, float h, bool noClr) {
    //比对大小
    if (this->w != w || this->h != h){
        this->w = w;
        this->h = h;
        //绑定纹理
        glBindTexture(GL_TEXTURE_2D,buffer);
        //纹理赋值为空，先纹理占位
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h,0,GL_RGBA,GL_UNSIGNED_BYTE, nullptr);
        //设定纹理参数
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        //绑定帧图
        glBindFramebuffer(GL_FRAMEBUFFER,buffer);
        //绑定纹理到帧图
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, texture,0);
        //切换回默认纹理
        glBindTexture(GL_TEXTURE_2D,0);
        //切换回默认的帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }
}

GLuint ymat::FrameBuffer::reset(float width, float height) {
    const GLuint srcTexture = texture;
    if (buffer != 0) {
        glDeleteFramebuffers(1, &buffer);
    }
    w = width;
    h = height;
    //生成帧缓冲id
    glGenFramebuffers(1, &buffer);
    //生成纹理id
    glGenTextures(1,&texture);
    //绑定纹理
    glBindTexture(GL_TEXTURE_2D,buffer);
    //纹理赋值为空，先纹理占位
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h,0,GL_RGBA,GL_UNSIGNED_BYTE, nullptr);
    //设定纹理参数
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    //绑定帧图
    glBindFramebuffer(GL_FRAMEBUFFER,buffer);
    //清理内容
    glClear(GL_COLOR_BUFFER_BIT);
    //绑定纹理到帧图
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, texture,0);
    //切换回默认纹理
    glBindTexture(GL_TEXTURE_2D,0);
    //切换回默认的帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    YMLOGV("FrameBuffer reset %d", texture);
    return srcTexture;
}

void ymat::FrameBuffer::destroy() {
    if (buffer != 0) {
        glDeleteFramebuffers(1, &buffer);
    }
    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }
}
