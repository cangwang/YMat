package com.yy.ymat.render

import android.graphics.Canvas
import android.graphics.RectF
import com.yy.ymat.bean.ElementsInfo

class RectLayer : BaseShapeLayer() {
    override fun draw(canvas: Canvas, elementsInfo: ElementsInfo,frame: Int): Boolean {
        val rectInfo = elementsInfo.rectInfo ?: return false
        val rect = RectF()
        val x = rectInfo.position?.getOrNull(0) ?: 0f
        val y = rectInfo.position?.getOrNull(1) ?: 0f
        val width = rectInfo.size?.getOrNull(0) ?: 0f
        val height = rectInfo.size?.getOrNull(1) ?: 0f
        rect.set(x, y, x + width, y + height)
        if (rect.isEmpty) {
            return false
        }
        var hasDraw = false
        getStrokePaint(elementsInfo,frame)?.let {
            val saved = canvas.saveLayer(null, it)
            if (rectInfo.roundness > 0) {
                canvas.drawRoundRect(
                    rect,
                    rectInfo.roundness.toFloat(),
                    rectInfo.roundness.toFloat(),
                    it
                )
            } else {
                canvas.drawRect(rect, it)
            }
            canvas.restoreToCount(saved)
            hasDraw = true
        }
        getFillPaint(elementsInfo)?.let {
            val saved = canvas.saveLayer(null, it)
            if (rectInfo.roundness > 0) {
                canvas.drawRoundRect(
                    rect,
                    rectInfo.roundness.toFloat(),
                    rectInfo.roundness.toFloat(),
                    it
                )
            } else {
                canvas.drawRect(rect, it)
            }
            canvas.restoreToCount(saved)
            hasDraw = true
        }
        return hasDraw
    }
}