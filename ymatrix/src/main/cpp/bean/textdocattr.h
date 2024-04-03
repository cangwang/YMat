//
// Created by Cangwang on 2024/4/2.
//

#pragma once
#include <string>
#include <vector>

using namespace std;
namespace ymat {
    class TexDocAttr {
    public:
        string text;
        vector<float> textColor;
        string font;
        string fontFamily;
        int fontSize;
        string fontStyle;
        bool fauxBold;
        bool fauxItalic;
        float lineSpacing;
        int wordSpacing;
        int textAligment;
        int orientation;
    };
}
