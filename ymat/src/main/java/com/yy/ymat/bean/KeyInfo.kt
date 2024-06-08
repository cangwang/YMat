package com.yy.ymat.bean

/**
 *  path from 0:app path, 1:local path, 2:network url
 */
data class KeyInfo(val id: Int, var type: String, var path: String, var from: Int = 0)