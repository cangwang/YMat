package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName
@Keep
class EllipseInfo(
    @SerializedName("direction") var direction: Int,
    @SerializedName("size") var size: FloatArray?,
    @SerializedName("position") var position: FloatArray?
) {


    override fun toString(): String {
        return "EllipseInfo(direction=$direction, " +
                "size=${size?.contentToString()}, position=${position?.contentToString()})"
    }
}