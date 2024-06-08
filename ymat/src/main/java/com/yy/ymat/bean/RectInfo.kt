package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName
@Keep
class RectInfo(
    @SerializedName("direction") var direction: Int,
    @SerializedName("size") var size: FloatArray?,
    @SerializedName("position") var position: FloatArray?,
    @SerializedName("roundness") var roundness: Int
) {

    override fun toString(): String {
        return "RectInfo(direction=$direction, size=${size?.contentToString()}," +
                " position=${position?.contentToString()}, roundness=$roundness)"
    }
}