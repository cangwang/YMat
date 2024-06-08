package com.yy.ymat.bean

class ShapeInfoCache {
    var width: Int = 0
    var height: Int = 0
    var shapeList: List<ShapeLayerInfo>? = null
    var inFrame: Int = 0
    var outFrame: Int = 0

    fun isNeedReRender(frame: Int): Boolean {
        return shapeList?.find { it.type == ElementsInfo.SHAPE_PATH }?.elements?.strokeInfo?.dashesInfo?.isNeedReRender(
            frame
        ) == true
    }
}