//
// Created by Cangwang on 2024/5/9.
//

#include "TextDrawer.h"

#define LOG_TAG "TextDrawer"

void TextDrawer::init() {
    textDocAttr = dynamic_pointer_cast<TextLayerInfo>(info)->textDocAttr;
    align = textDocAttr->textAligment;
//    if (textDocAttr->orientation) {  //横向画字
//        if (align == 1) {
//            setAnchorOffXY(width * 0.5, 0);
//        } else if (align == 2) {
//            setAnchorOffXY(width * 0.5, height * 0.5);
//        } else {
//            setAnchorOffXY(width * 0.5, height);
//        }
//    } else {  //竖向画字
//        if (align == 1) {
//            setAnchorOffXY(0, height);
//        } else if (align == 2) {
//            setAnchorOffXY(width * 0.5, height);
//        } else {
//            setAnchorOffXY(width, height);
//        }
//    }

//    setAnchorOffXY(width * 0.5, height * 0.5);
    //加入偏移
    setAnchorOffXY(0, textDocAttr->fontSize);
    if (textDocAttr->orientation) {
        auto str = textDocAttr->text.c_str();
        while (*str) {
            // 对于UTF-8编码，中文字符的第一个字节通常是 1110xxxx (E0-EF)
            // 接下来是 10xxxxxx (80-BF)
            // 简化处理：假设所有中文字符都是3字节长
            if ((unsigned char) *str >= 0xE0 && (unsigned char) *(str + 1) >= 0x80 &&
                (unsigned char) *(str + 2) >= 0x80) {
                // 输出整个3字节字符
                verticalStr.emplace_back(str, 3);
                str += 3; // 跳过这3个字节
            } else {
                // 对于非中文字符（可能是ASCII或其他单字节字符），或者不完整的中文字符，只输出一个字节
                verticalStr.emplace_back(str, 1);
                str += 1; // 跳过这个字节
            }
        }
    }
    //读取默认字体
    const std::string FallbackFontFileNames[] = {"/system/fonts/NotoSansCJK-Regular.ttc",
                                                 "/system/fonts/NotoSansSC-Regular.otf",
                                                 "/system/fonts/DroidSansFallback.ttf"};
    for (auto& fileName : FallbackFontFileNames) {
        auto typeface = tgfx::Typeface::MakeFromPath(fileName);
        if (typeface != nullptr) {
            this->typeface = typeface;
            break;
        }
    }
//
//    auto emoji = tgfx::Typeface::MakeFromPath("/system/fonts/NotoColorEmoji.ttf");
//    if (typeface != nullptr) {
//        this->emoji = emoji;
//    }

    ElementDrawer::init();
}

int TextDrawer::drawShape(shared_ptr<FrameInfo> frameInfo, tgfx::Canvas *canvas) {
    YMLOGV("drawShape %s", info->name.c_str());
    tgfx::Font font;
    font.setFauxBold(textDocAttr->fauxBold);
    font.setFauxItalic(textDocAttr->fauxItalic);
    font.setSize((float)textDocAttr->fontSize);
    font.setTypeface(typeface);
    tgfx::Point textStart = {0, (float) textDocAttr->fontSize};
    tgfx::Paint paint = {};
    auto color = textDocAttr->textColor;
    if (color.size() == 3) {
        paint.setColor({color[0], color[1], color[2], 1.0f});
    } else {
        paint.setColor({1.0f, 1.0f, 1.0f, 1.0f});
    }
    paint.setStyle(tgfx::PaintStyle::Fill);

    if (textDocAttr->orientation) {
        auto startY = 0.0f;
        for (const string t : verticalStr) {
            startY += (float) textDocAttr->fontSize;
            canvas->drawSimpleText(t, textStart.x, startY, font, paint);
        }
    } else {
        canvas->drawSimpleText(textDocAttr->text, textStart.x, textStart.y, font, paint);
    }
    return 0;
}

void TextDrawer::destroy() {

}
