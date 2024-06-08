//
// Created by Cangwang on 2024/5/9.
//

#pragma once

#include <tgfx/ElementDrawer.h>

using namespace std;
namespace ymat {
    class TextDrawer : public ElementDrawer {
    public:
        void init() override;

        int drawShape(shared_ptr<FrameInfo> frameInfo, tgfx::Canvas *canvas) override;

        void destroy() override;
    private:
        shared_ptr<TexDocAttr> textDocAttr;
        int align = 1;  //0: left 1:center 2:right
        shared_ptr<tgfx::Typeface> typeface;
        shared_ptr<tgfx::Typeface> emoji;

        vector<string> verticalStr;
    };
}
