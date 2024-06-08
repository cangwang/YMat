package com.yy.ymat.render

import android.graphics.Canvas
import android.graphics.RectF
import com.yy.ymat.bean.ElementsInfo
import com.yy.ymat.utils.YMLog

class EllipseLayer : BaseShapeLayer() {
    override fun draw(canvas: Canvas, elementsInfo: ElementsInfo, frame: Int): Boolean {
        val ellipseInfo = elementsInfo.ellipseInfo ?: return false
        val rect = RectF()
        val x = ellipseInfo.position?.getOrNull(0) ?: 0f
        val y = ellipseInfo.position?.getOrNull(1) ?: 0f
        val width = ellipseInfo.size?.getOrNull(0) ?: 0f
        val height = ellipseInfo.size?.getOrNull(1) ?: 0f
        rect.set(x, y, x + width, y + height)
        if (rect.isEmpty) {
            return false
        }
        var hasDraw = false

        getStrokePaint(elementsInfo, frame)?.let {
            val saved = canvas.saveLayer(null, it)
            canvas.drawOval(
                rect,
                it
            )
            canvas.restoreToCount(saved)
            hasDraw = true
        }
        getFillPaint(elementsInfo)?.let {
            val saved = canvas.saveLayer(null, it)
            canvas.drawOval(rect, it)
            canvas.restoreToCount(saved)
            hasDraw = true
        }
        YMLog.d("EllipseLayer", "draw:$hasDraw")
        return hasDraw
    }
}