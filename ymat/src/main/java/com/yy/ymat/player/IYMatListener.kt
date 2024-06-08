package com.yy.ymat.player

interface IYMatListener {

    //解析YMat文件完成,可以在这个时期设置key
    fun onParsed() {}

    //解码器创建完成
    fun onLoaded() {}

    fun onStart() {}

    fun onRender(frameIndex: Int) {}

    fun onComplete() {}

    /**
     * 销毁
     */
    fun onDestroy() {}

    /**
     * 循坏重播开始
     */
    fun onRestart() {}

    fun onFailed(errorMsg: String) {}

}