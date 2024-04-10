package com.yy.ymat.utils


object YMatUtils {
    init {
        System.loadLibrary("ymat")
    }

    external fun setYMatConfig(json: String): Int
}