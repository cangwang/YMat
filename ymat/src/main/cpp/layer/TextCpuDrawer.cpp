//
// Created by asus on 2024/5/19.
//

#include "TextCpuDrawer.h"
#define LOG_TAG "TextCpuDrawer"

void TextCpuDrawer::init() {
    if (info) {
        textDocAttr = dynamic_pointer_cast<TextLayerInfo>(info)->textDocAttr;
        if (textDocAttr) {
            if (textDocAttr->orientation == 0) {
                setAnchorOffXY(0, textDocAttr->fontSize);
            } else {
                setAnchorOffXY(textDocAttr->fontSize / 2.0f, textDocAttr->fontSize / 2.0f);
            }
        }
        if (info->textureInfo) {
            updateWidthHeight(info->textureInfo->width, info->textureInfo->height);
        } else {
            YMLOGE("no textureInfo");
        }
    }
    ImageDrawer::init();
}
