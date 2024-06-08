package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName

@Keep
class ShapeLayerInfo(
    @SerializedName("type") var type: String = "",
    @SerializedName("name") var name: String = "",
    @SerializedName("blendMode") var blendMode: Int = 1,
    @SerializedName("elements") var elements: ElementsInfo? = null,
) {

    override fun toString(): String {
        return "ShapeLayerInfo(type='$type', name='$name', blendMode=$blendMode, " +
                "elements=$elements)"
    }
}