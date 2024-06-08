package com.yy.ymat.render

import android.graphics.Canvas
import android.graphics.Path
import com.yy.ymat.bean.ElementsInfo
import com.yy.ymat.utils.YMLog

class PathLayer : BaseShapeLayer() {

    override fun draw(canvas: Canvas, elementsInfo: ElementsInfo, frame: Int): Boolean {
        elementsInfo.shapeInfo ?: return false
        val points = elementsInfo.shapeInfo!!.points
        if (points.isNullOrEmpty()) {
            return false
        }
        val actions = elementsInfo.shapeInfo!!.actions ?: return false
        if (actions.isEmpty()) {
            return false
        }
        var currentPoint = 0
        val path = Path()
        for (action in actions) {
            if (action == 0) {
                val movePoint = points[currentPoint++]
                path.moveTo(movePoint[0], movePoint[1])
            } else if (action == 1) {
                val linePoint = points[currentPoint++]
                path.lineTo(linePoint[0], linePoint[1])
            } else if (action == 2) {
                val control1 = points[currentPoint++]
                val control2 = points[currentPoint++]
                path.cubicTo(
                    control1[0], control1[1], control2[0], control2[1],
                    points[currentPoint][0], points[currentPoint][1]
                )
            } else if (action == 3) {
                path.close()
            }
        }
        var hasDraw = false
        getStrokePaint(elementsInfo, frame)?.let {
            val saved = canvas.saveLayer(null, it)
            canvas.translate(width / 2 ,height / 2)
            canvas.drawPath(path, it)
            canvas.restoreToCount(saved)
            hasDraw = true
        }
        getFillPaint(elementsInfo)?.let {
            val saved = canvas.saveLayer(null, it)
            canvas.translate(width / 2 ,height / 2)
            canvas.drawPath(path, it)
            canvas.restoreToCount(saved)
            hasDraw = true
        }
        YMLog.d("PathLayer", "hasDraw:$hasDraw")
        return hasDraw
    }
}