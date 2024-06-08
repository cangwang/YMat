package com.yy.ymat.player

/**
 * created by zengjiale
 */
interface IYMatVideoListener {

    /**
     * 配置准备好后回调
     * @return true 继续播放 false 结束播放
     */
    fun onVideoConfigReady(resId: Int): Boolean {
        return true // 默认继续播放
    }

    /**
     * 开始播放
     */
    fun onVideoStart(resId: Int) {}

    /**
     * 循环播放开始
     */
    fun onVideoRestart(resId: Int) {}


    /**
     * 视频渲染每一帧时的回调
     * @param frameIndex 帧索引
     */
    fun onVideoRender(frameIndex: Int, resId: Int) {}

    /**
     * 视频播放结束
     */
    fun onVideoComplete(resId: Int, lastFrame: Boolean = false) {}

    /**
     * 视频被销毁
     */
    fun onVideoDestroy(resId: Int) {}

    /**
     * 失败回调
     * @param errorType 错误类型
     * @param errorMsg 错误消息
     */
    fun onFailed(resId: Int, errorType: Int, errorMsg: String?) {}
}