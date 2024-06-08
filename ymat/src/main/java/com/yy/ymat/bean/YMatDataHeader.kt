package com.yy.ymat.bean

class YMatDataHeader {
    companion object {
        const val TYPE_UNKNOWN = -1
        const val TYPE_DESC = 0
        const val TYPE_IMG = 1
        const val TYPE_VIDEO = 2
        const val TYPE_AUDIO = 3
        const val TYPE_TEXT = 4
    }

    var type: Int = TYPE_UNKNOWN
    var extend: Int = -1
    var length: Int = -1
    override fun toString(): String {
        return "YMatDataHeader(type=$type, extend=$extend, length=$length)"
    }


}