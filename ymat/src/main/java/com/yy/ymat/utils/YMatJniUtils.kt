package com.yy.ymat.utils

/**
 * created by zengjiale
 */
object YMatJniUtils {
    init {
        System.loadLibrary("ymat")
    }

    external fun setYMatConfig(json: String): Int

    external fun renderFrame(playeId: Int)
}