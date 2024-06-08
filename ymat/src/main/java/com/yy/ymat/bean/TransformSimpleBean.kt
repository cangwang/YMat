package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName
@Keep
class TransformSimpleBean {
    @SerializedName("inFrame")
    var inFrame = 0

    @SerializedName("value")
    var value = 0f

    @SerializedName("timeFunc")
    var timeFunc = 0
    override fun toString(): String {
        return "TransformSimpleBean(inFrame=$inFrame, value=$value, timeFunc=$timeFunc)"
    }


}