package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName
@Keep
class FillInfo(
    @SerializedName("blendMode") var blendMode: Int,
    @SerializedName("color") var color: FloatArray?,
    @SerializedName("opacity") var opacity: Int
) {

    override fun toString(): String {
        return "FillInfo(blendMode=$blendMode, color=${color?.contentToString()}, opacity=$opacity)"
    }
}