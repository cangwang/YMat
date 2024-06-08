package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName

@Keep
class ElementsInfo {

    companion object {
        const val SHAPE_RECT = "Rect"
        const val SHAPE_PATH = "Path"
        const val SHAPE_ELLIPSE = "Ellipse"
    }

    @SerializedName("ellipseInfo")
    var ellipseInfo: EllipseInfo? = null

    @SerializedName("fillInfo")
    var fillInfo: FillInfo? = null

    @SerializedName("rectInfo")
    var rectInfo: RectInfo? = null

    @SerializedName("shapeInfo")
    var shapeInfo: ShapeInfo? = null

    @SerializedName("strokeInfo")
    var strokeInfo: StrokeInfo? = null
    override fun toString(): String {
        return "ElementsInfo(ellipseInfo=$ellipseInfo, fillInfo=$fillInfo, " +
                "rectInfo=$rectInfo, shapeInfo=$shapeInfo, strokeInfo=$strokeInfo)"
    }


}