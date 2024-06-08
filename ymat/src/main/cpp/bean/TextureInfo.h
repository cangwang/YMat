//
// Created by Cangwang on 2024/5/15.
//

#pragma once
#include <GLES3/gl3.h>

using namespace std;
namespace ymat {
    class TextureInfo {
    public:
        ~TextureInfo() {
            glDeleteTextures(1, &textureId);
        }
        int resId = 0;  //对应的资源id
        GLuint textureId = 0; //纹理为0则为无纹理
        int width = -1;
        int height = -1;
        int frameIndex = 0;
        bool forceUpdate = false; //强制更新
    };
}
