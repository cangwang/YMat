package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName
@Keep
class ShapeInfo {
    @SerializedName("points")
    var points: List<List<Float>>? = null

    @SerializedName("actions")
    var actions: IntArray? = null

    override fun toString(): String {
        return "ShapeInfo(points=${points}, actions=${actions?.contentToString()})"
    }


}