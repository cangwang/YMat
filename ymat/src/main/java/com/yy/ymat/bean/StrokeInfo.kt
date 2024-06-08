package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName
@Keep
class StrokeInfo(//混合模式
    @SerializedName("blendMode") var blendMode: Int,
    @SerializedName("color") var color: FloatArray?,
    @SerializedName("opacity") var opacity: Int,
    @SerializedName("width") var width: Int,//线帽
    @SerializedName("lineCap") var lineCap: Int,//设置 2 个长度不为 0 的相连部分（线段、圆弧、曲线）如何连接在一起的属性
    @SerializedName("lineJoin") var lineJoin: Int,//设置斜接面限制比例的属性
    @SerializedName("miterLimit") var miterLimit: Int = 0,
    @SerializedName("dashesInfo") var dashesInfo: DashesInfo?
) {

    override fun toString(): String {
        return "StrokeInfo(blendMode=$blendMode, color=${color?.contentToString()}, " +
                "opacity=$opacity, width=$width, lineCap=$lineCap, lineJoin=$lineJoin," +
                " miterLimit=$miterLimit, dashesInfo=$dashesInfo)"
    }
}