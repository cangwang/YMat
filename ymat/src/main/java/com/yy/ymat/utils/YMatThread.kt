package com.yy.ymat.utils

import android.os.Handler
import android.os.HandlerThread

/**
 * created by zengjiale
 */
data class HandlerHolder(var thread: HandlerThread?, var handler: Handler?)
object YMatThread {
    private const val TAG = "YMatThread"

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