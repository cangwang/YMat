package com.yy.ymat.view

import com.yy.ymat.bean.KeyInfo
import com.yy.ymat.player.IYMatListener
import com.yy.ymat.player.IYMatVideoListener
import com.yy.ymat.utils.IYMLog

interface IYMatView {
    //播放文件
    fun startPlay(address: String)

    //播放本地文件
    fun startPlayFromAsset(assetsPath: String)

    // 暂停播放
    fun pause()
    // 恢复播放
    fun resume()
    //停止播放
    fun stop()
    //是否正在运行
    fun isRunning(): Boolean

    //设置日志打印
    fun setLog(log: IYMLog)

    fun replay()

    fun destroy()

    fun load(path: String)

    fun play()

    fun setKeys(keyInfo: KeyInfo)

    fun setLoop(isLoop: Boolean)

    fun setVideoListener(listener: IYMatVideoListener)

    fun setYmatListener(listener: IYMatListener)

    fun loadAndPlayFromAssets(fileName: String)

    fun loadAndPlayFromFile(fileName: String)

    fun loadAndPlayFromUrl(url: String)

    fun setFillMode(fillMode: Int)
}