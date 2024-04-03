package com.yy.ymatrix.utils


object YMatrixUtils {
    init {
        System.loadLibrary("ymatrix")
    }

    /**
     * 获取底层的textureId，用于创建和绑定surfaceView或textureView
     */
    external fun getExternalTexture(): Int
}