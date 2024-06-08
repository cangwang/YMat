package com.yy.ymat.bean

import androidx.annotation.Keep
import com.google.gson.annotations.SerializedName
import com.yy.ymat.utils.TransformUtil

@Keep
class DashesInfo(
    @SerializedName("dash") var dash: FloatArray?,
    @SerializedName("offset")
    var offset: List<TransformSimpleBean>?,
    var offsetValue: Float? = null
) {
    override fun toString(): String {
        return "DashesInfo(dash=${dash?.contentToString()}, offset=$offset)"
    }

    fun isNeedReRender(frame: Int): Boolean {
        val value = getOffset(frame)
        if (value != offsetValue) {
            offsetValue = value
            return true
        }
        return false
    }

    fun getOffset(frame: Int): Float {
        if (offset.isNullOrEmpty()) {
            return 0F;
        }

        if (frame < offset!!.first().inFrame) {
            return 0F
        }
        val l = offset!!.size
        var idx = 0
        for (i in 0 until l) {
            if (offset!![i].inFrame == frame) {
                return offset!![i].value
            }
            if (offset!![i].inFrame > frame) {
                break
            }
            idx = i
        }
        if (idx == l - 1) {
            return offset!![idx].value
        }
        val lhs = offset!![idx]
        val rhs = offset!![idx + 1]

        var fas = 0f
        when (offset!![idx].timeFunc) {
            0 ->
                fas = TransformUtil.linear(frame, lhs.inFrame, rhs.inFrame)

            1 ->
                fas = TransformUtil.easeInQuad(frame, lhs.inFrame, rhs.inFrame)

            2 ->
                fas = TransformUtil.easeOutQuad(frame, lhs.inFrame, rhs.inFrame)

            3 ->
                fas = TransformUtil.easeInOutQuad(frame, lhs.inFrame, rhs.inFrame)

            else ->
                fas = TransformUtil.linear(frame, lhs.inFrame, rhs.inFrame);
        }
        val v = lhs.value + (rhs.value - lhs.value) * fas;
        return v
    }
}