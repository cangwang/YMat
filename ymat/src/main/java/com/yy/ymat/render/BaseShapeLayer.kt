package com.yy.ymat.render

import android.graphics.Canvas
import android.graphics.Color
import android.graphics.DashPathEffect
import android.graphics.Paint
import com.yy.ymat.bean.ElementsInfo

abstract class BaseShapeLayer {

    companion object {
        fun argb(alpha: Float, red: Float, green: Float, blue: Float): Int {
            // 将浮点数转换为 0-255 的整数
            val a = (alpha * 255).toInt() and 0xFF shl 24 // 左移 24 位并设置 alpha 值
            val r = (red * 255).toInt() and 0xFF shl 16   // 左移 16 位并设置 red 值
            val g = (green * 255).toInt() and 0xFF shl 8  // 左移 8 位并设置 green 值
            val b = (blue * 255).toInt() and 0xFF         // 设置 blue 值
            // 将所有值组合成一个整数
            return a or r or g or b
        }
    }

    abstract fun draw(canvas: Canvas, elementsInfo: ElementsInfo, frame: Int): Boolean

    var width = 0f
    var height = 0f

    fun getStrokePaint(elementsInfo: ElementsInfo, frame: Int = 0): Paint? {
        val strokeInfo = elementsInfo.strokeInfo ?: return null
        val paint = Paint()
        paint.style = Paint.Style.STROKE;
        paint.strokeWidth = strokeInfo.width.toFloat()

        if (strokeInfo.lineCap == 0) {
            paint.strokeCap = Paint.Cap.BUTT
        } else if (strokeInfo.lineCap == 1) {
            paint.strokeCap = Paint.Cap.ROUND
        } else if (strokeInfo.lineCap == 2) {
            paint.strokeCap = Paint.Cap.SQUARE
        }
        if (strokeInfo.lineJoin == 0) {
            paint.strokeJoin = Paint.Join.MITER
        } else if (strokeInfo.lineJoin == 1) {
            paint.strokeJoin = Paint.Join.ROUND
        } else if (strokeInfo.lineJoin == 2) {
            paint.strokeJoin = Paint.Join.BEVEL
        }
        paint.strokeMiter = strokeInfo.miterLimit.toFloat()
        val colors = strokeInfo.color
        if (colors != null && colors.isNotEmpty()) {
            val color = argb(
                colors.getOrNull(3) ?: 1f,
                colors.getOrNull(0) ?: 0f,
                colors.getOrNull(1) ?: 0f,
                colors.getOrNull(2) ?: 0f
            )
            paint.color = color
        }
        if (strokeInfo.dashesInfo != null) {
            paint.pathEffect =
                DashPathEffect(
                    strokeInfo.dashesInfo!!.dash,
                    strokeInfo.dashesInfo!!.offsetValue ?: 0F
                )
        }
        return paint;
    }
    

    fun getFillPaint(elements: ElementsInfo): Paint? {

        val fillInfo = elements.fillInfo ?: return null
        val paint = Paint()
        paint.style = Paint.Style.FILL
        val colors = fillInfo.color
        if (colors != null && colors.isNotEmpty()) {
            val color = argb(
                colors.getOrNull(3) ?: 1f,
                colors.getOrNull(0) ?: 0f,
                colors.getOrNull(1) ?: 0f,
                colors.getOrNull(2) ?: 0f
            )
            paint.color = color;
        }
        paint.alpha = (255 * fillInfo.opacity * 0.01).toInt()
        return paint;
    }
}