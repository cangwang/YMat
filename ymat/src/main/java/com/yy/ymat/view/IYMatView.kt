package com.yy.ymat.view

import android.content.res.AssetManager
import java.io.File

interface IYMatView {
    //播放文件
    fun startPlay(file: File)

    //播放本地文件
    fun startPlay(assetManager: AssetManager, assetsPath: String)

//    fun startPlay(evaFileContainer: IEvaFileContainer)
    // 暂停播放
    fun pause()
    // 恢复播放
    fun resume()
    //停止播放
    fun stopPlay()
    //是否正在运行
    fun isRunning(): Boolean
}