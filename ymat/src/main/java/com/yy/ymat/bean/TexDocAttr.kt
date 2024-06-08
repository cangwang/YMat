package com.yy.ymat.bean

import androidx.annotation.Keep

@Keep
class TexDocAttr(
    var text: String = "",
    var textColor: FloatArray? = null,
    var font: String = "",
    var fontFamily: String = "",
    var fontSize: Int,
    var fontStyle: String = "",
    var fauxBold: Boolean,
    var fauxItalic: Boolean,
    var lineSpacing: Float,
    var wordSpacing: Int,
    var textAligment: Int,
    var orientation: Int,
    var with: Int,
    var height: Int
) {
    override fun toString(): String {
        return "TexDocAttr(text='$text', textColor=${textColor?.contentToString()}," +
                " font='$font', fontFamily='$fontFamily', fontSize=$fontSize," +
                " fontStyle='$fontStyle', fauxBold=$fauxBold, fauxItalic=$fauxItalic," +
                " lineSpacing=$lineSpacing, wordSpacing=$wordSpacing, textAligment=$textAligment," +
                " orientation=$orientation)"
    }
}