//
// Created by asus on 2024/5/19.
//

#pragma once
#include <memory>
#include <layer/ImageDrawer.h>


namespace ymat {
    class TextCpuDrawer : public ImageDrawer {
    public:
        void init() override;
    private:
        shared_ptr<TexDocAttr> textDocAttr;
    };
}
