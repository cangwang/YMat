package com.yy.ymat.view

import android.content.Context
import android.graphics.PixelFormat
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.SurfaceView

class YMatSurfaceView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : SurfaceView(context, attrs, defStyleAttr) {
    private val TAG = "YMatSurfaceView"

    init {
        setBackgroundResource(android.R.color.transparent)
        setZOrderOnTop(true)
        holder.setFormat(PixelFormat.TRANSLUCENT)
    }

    override fun dispatchTouchEvent(ev: MotionEvent?): Boolean {
        return super.dispatchTouchEvent(ev)
    }
}