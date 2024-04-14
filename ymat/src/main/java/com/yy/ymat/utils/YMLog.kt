package com.yy.ymat.utils

/**
 * created by zengjiale
 */
object YMLog {

    var isDebug = false

    var log: IYMLog? = null

    fun i(tag: String, msg: String) {
        log?.i(tag, msg)
    }

    fun d(tag: String, msg: String) {
        if (isDebug) {
            log?.d(tag, msg)
        }
    }

    fun e(tag: String, msg: String) {
        log?.e(tag, msg)
    }

    fun e(tag: String, msg: String, tr: Throwable) {
        log?.e(tag, msg, tr)
    }
}


interface IYMLog {
    fun i(tag: String, msg: String) {}

    fun d(tag: String, msg: String) {}

    fun e(tag: String, msg: String) {}

    fun e(tag: String, msg: String, tr: Throwable) {}
}


