package com.yy.ymat.utils


object YMatUtils {
    init {
        System.loadLibrary("ymat")
    }

    /**
     * 获取底层的textureId，用于创建和绑定surfaceView或textureView
     */
    external fun getExternalTexture(): Int

    external fun setYMatConfig(json: String)
}