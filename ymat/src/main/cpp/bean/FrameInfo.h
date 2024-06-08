//
// Created by Cangwang on 2024/5/9.
//

#pragma once
#include <bean/FrameBuffer.h>

namespace ymat {
    struct FrameInfo {
        int frames;
        int frameId;
        float width;    //父级宽度
        float height;   //父级高度
        float opacity;  // 父级的透明度
        std::shared_ptr<FrameBuffer> framebuffer;
    };
}
