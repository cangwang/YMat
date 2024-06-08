package com.yy.ymat.player

import com.yy.ymat.bean.YMatPlayerInfo
import com.yy.ymat.utils.YMLog
import java.util.Timer
import java.util.TimerTask

/**
 * created by zengjiale2
 */
class YMatSyner(val playerInfo: YMatPlayerInfo) {
    private val TAG = "YMatSyner"
    private val timer = Timer()
    private var isPause = false

    private val timerTask = object : TimerTask() {
        override fun run() {
            if (!isPause) {
                YMLog.i(TAG, "startFrame $frameIndex")
                listeners.forEach { it.startFrame(frameIndex) }
                frameIndex++
            }
        }
    }

    private var frameIndex = 0
    private var listeners = ArrayList<YMatFrameListener>()

    fun reset() {
        frameIndex = 0
    }

    fun addFrameListener(listener: YMatFrameListener) {
        listeners.add(listener)
    }

    /**
     * 启动定时器
     */
    fun startTimer() {
        if (playerInfo.frameRate > 0) {
            val period = 1000L / playerInfo.frameRate
            YMLog.i(TAG, "startTimer period $period")
            timer.scheduleAtFixedRate(timerTask, 0, period)
        }
    }

    fun pause() {
        isPause = true
    }

    fun resume() {
        isPause = false
    }

    /**
     * 用于重复播放
     */
    fun resetFrameIndex() {
        frameIndex = 0
    }

    fun cancelTimer() {
        YMLog.i(TAG, "cancelTimer")
        listeners.clear()
        timerTask.cancel()
        timer.cancel()
        isPause = false
        resetFrameIndex()
    }

    interface YMatFrameListener {
        fun startFrame(frameIndex: Int)
    }
}