package com.yy.ymat.decoder

import android.os.HandlerThread
import android.os.Handler
import android.util.Log
import com.yy.ymat.utils.HandlerHolder
import com.yy.ymat.utils.YMLog
import com.yy.ymat.utils.SpeedControlUtil

abstract class YMatDecoder() {

    companion object {
        private const val TAG = "YMatDecoder"

        fun createThread(handlerHolder: HandlerHolder, name: String): Boolean {
            try {
                if (handlerHolder.thread == null || handlerHolder.thread?.isAlive == false) {
                    handlerHolder.thread = HandlerThread(name).apply {
                        start()
                        handlerHolder.handler = Handler(looper)
                    }
                }
                return true
            } catch (e: OutOfMemoryError) {
                YMLog.e(TAG, "createThread OOM", e)
            }
            return false
        }

        fun quitSafely(thread: HandlerThread?): HandlerThread? {
            thread?.apply {
                thread.quitSafely()
            }
            return null
        }
    }

    val renderThread = HandlerHolder(null, null)
    val decodeThread = HandlerHolder(null, null)

    var fps: Int = 0
        set(value) {
            //设置帧率
            speedControlUtil.setFixedPlaybackRate(value)
            field = value
        }
    var playLoop = 0 // 循环播放次数
    var isRunning = false // 是否正在运行
    var isStopReq = false // 是否需要停止
    val speedControlUtil by lazy { SpeedControlUtil() }

    abstract fun start()

    fun stop() {
        Log.i(TAG, "stop true")
        isStopReq = true
    }

    abstract fun pause()

    abstract fun resume()

    abstract fun destroy()

    fun prepareThread(): Boolean {
        return createThread(renderThread, "ymat_render_thread") && createThread(decodeThread, "ymat_decode_thread")
    }

    fun destroyThread() {
        YMLog.i(TAG, "destroyThread")
        renderThread.handler?.removeCallbacksAndMessages(null)
        decodeThread.handler?.removeCallbacksAndMessages(null)
        renderThread.thread = quitSafely(renderThread.thread)
        decodeThread.thread = quitSafely(decodeThread.thread)
        renderThread.handler = null
        decodeThread.handler = null
    }
}

