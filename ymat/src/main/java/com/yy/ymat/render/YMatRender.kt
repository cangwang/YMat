package com.yy.ymat.render

import com.yy.ymat.utils.HandlerHolder
import com.yy.ymat.decoder.YMatDecoder
import com.yy.ymat.utils.YMatThread
import com.yy.ymat.utils.YMLog
import com.yy.ymat.utils.YMatJniUtils

/**
 * created by zengjiale
 */
class YMatRender {
    private var TAG = "YMatRender"
    var renderThread: HandlerHolder? = null

    fun preapareThread() {
        if (renderThread == null) {
            renderThread = HandlerHolder(null, null)
        }
        YMatThread.createThread(renderThread!!, "ymat_render_thread")
    }

    fun renderFrame() {
        YMatJniUtils
    }

    fun release() {
        YMLog.i(TAG, "destroyRenderThread")
        renderThread?.handler?.removeCallbacksAndMessages(null)
        renderThread?.thread = YMatDecoder.quitSafely(renderThread?.thread)
        renderThread?.handler = null
    }
}