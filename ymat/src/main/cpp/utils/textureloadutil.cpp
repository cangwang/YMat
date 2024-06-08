//
// Created by zengjiale on 2022/4/21.
//
#include "textureloadutil.h"
#define LOG_TAG "TextureLoadUtil"

shared_ptr<TextureInfo> TextureLoadUtil::loadTexture(unsigned char *bitmap, AndroidBitmapInfo* info) {
    if (bitmap == nullptr) {
        YMLOGE("loadTexture bitmap = null");
        return nullptr;
    }

    GLuint textureObjectIds = 0;
    glGenTextures(1, &textureObjectIds);
    if (textureObjectIds == 0) {
        YMLOGE("loadTexture textureObjectIds = 0");
        return nullptr;
    }

    glBindTexture(GL_TEXTURE_2D, textureObjectIds);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //纹理放大缩小使用线性插值
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //超出的部份会重复纹理坐标的边缘，产生一种边缘被拉伸的效果，s/t相当于x/y轴坐标
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    GLint format = GL_RGB;
    if (info->format == ANDROID_BITMAP_FORMAT_RGB_565) { //RGB三通道，例如jpg格式
        format = GL_RGB;
    } else if (info->format == ANDROID_BITMAP_FORMAT_RGBA_8888) {  //RGBA四通道，例如png格式 android图片是ARGB排列，所以还是要做图片转换的
        format = GL_RGBA;
    }
    //将图片数据生成一个2D纹理
    glTexImage2D(GL_TEXTURE_2D, 0, format, info->width, info->height, 0, format, GL_UNSIGNED_BYTE,
                 bitmap);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    auto textureInfo = make_shared<TextureInfo>();
    textureInfo->width = info->width;
    textureInfo->height = info->height;
    textureInfo->textureId = textureObjectIds;
    return textureInfo;
}

void TextureLoadUtil::updateTexture(shared_ptr<TextureInfo> textureInfo, unsigned char *bitmap,
                                          AndroidBitmapInfo *info) {
    if (bitmap == nullptr) {
        YMLOGE("updateTexture bitmap = null");
    }

    if (!textureInfo || textureInfo->textureId == 0) {
        YMLOGE("updateTexture no texture");
    }

    glBindTexture(GL_TEXTURE_2D, textureInfo->textureId);

    GLint format = GL_RGB;
    if (info->format == ANDROID_BITMAP_FORMAT_RGB_565) { //RGB三通道，例如jpg格式
        format = GL_RGB;
    } else if (info->format == ANDROID_BITMAP_FORMAT_RGBA_8888) {  //RGBA四通道，例如png格式 android图片是ARGB排列，所以还是要做图片转换的
        format = GL_RGBA;
    }
    //将图片数据生成一个2D纹理
    glTexImage2D(GL_TEXTURE_2D, 0, format, info->width, info->height,
                 0, format, GL_UNSIGNED_BYTE,bitmap);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureInfo->width = info->width;
    textureInfo->height = info->height;
}

shared_ptr<TextureInfo> TextureLoadUtil::loadVideoTexture() {
    //shader
    GLuint textureId;
    glGenTextures(1,&textureId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    auto textureInfo = make_shared<TextureInfo>();
    textureInfo->textureId = textureId;
    return textureInfo;
}

void ymat::TextureLoadUtil::releaseTexture(GLuint textureId) {
    if (textureId != 0) {
        glDeleteTextures(1,&textureId);
    }
}
