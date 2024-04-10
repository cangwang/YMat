package com.yy.ymat.bean

data class TexDocAttr(
    var text: String = "",
    var textColor: ArrayList<Float>,
    var font: String = "",
    var fontFamily: String = "",
    var fontSize: Int,
    var fontStyle: String = "",
    var fauxBold: Boolean,
    var fauxItalic: Boolean,
    var lineSpacing: Float,
    var wordSpacing: Int,
    var textAligment: Int,
    var orientation: Int
)