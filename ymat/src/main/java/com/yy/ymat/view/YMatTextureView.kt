package com.yy.ymat.view

import android.content.Context
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.TextureView

class YMatTextureView  @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : TextureView(context, attrs, defStyleAttr) {
    private val TAG = "YMatTextureView"

    init {
        isOpaque = false
    }
    override fun dispatchTouchEvent(ev: MotionEvent?): Boolean {
        return super.dispatchTouchEvent(ev)
    }
}