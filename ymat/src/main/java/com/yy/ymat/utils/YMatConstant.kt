package com.yy.ymat.utils

object YMatConstant {
    const val TAG = "YMatConstant"

    const val OK = 0 // 成功

    const val REPORT_ERROR_TYPE_EXTRACTOR_EXC = 10001 // MediaExtractor exception
    const val REPORT_ERROR_TYPE_DECODE_EXC = 10002 // MediaCodec exception
    const val REPORT_ERROR_TYPE_CREATE_THREAD = 10003 // 线程创建失败
    const val REPORT_ERROR_TYPE_CREATE_RENDER = 10004 // render创建失败
    const val REPORT_ERROR_TYPE_PARSE_CONFIG = 10005 // 配置解析失败
    const val REPORT_ERROR_TYPE_CONFIG_PLUGIN_MIX = 10006 // 融合动画资源获取失败
    const val REPORT_ERROR_TYPE_FILE_ERROR = 10007 // 文件无法读取
    const val REPORT_ERROR_TYPE_HEVC_NOT_SUPPORT = 10008 // 不支持h265

    const val ERROR_MSG_EXTRACTOR_EXC = "0x1 MediaExtractor exception" // MediaExtractor exception
    const val ERROR_MSG_DECODE_EXC = "0x2 MediaCodec exception" // MediaCodec exception
    const val ERROR_MSG_CREATE_THREAD = "0x3 thread create fail" // 线程创建失败
    const val ERROR_MSG_CREATE_RENDER = "0x4 render create fail" // render创建失败
    const val ERROR_MSG_PARSE_CONFIG = "0x5 parse config fail" // 配置解析失败
    const val ERROR_MSG_CONFIG_PLUGIN_MIX = "0x6 yyeva fail" // 融合动画资源获取失败
    const val ERROR_MSG_FILE_ERROR = "0x7 file can't read" // 文件无法读取
    const val ERROR_MSG_HEVC_NOT_SUPPORT = "0x8 hevc not support" // 不支持h265

    const val TEXTURE_TEXT_RESOURCE = 1
    const val TEXTURE_IMAGE_RESOURCE = 2
    const val TEXTURE_VIDEO_RESOURCE = 3

    fun getErrorMsg(errorType: Int, errorMsg: String? = null): String {
        return when(errorType) {
            REPORT_ERROR_TYPE_EXTRACTOR_EXC -> ERROR_MSG_EXTRACTOR_EXC
            REPORT_ERROR_TYPE_DECODE_EXC -> ERROR_MSG_DECODE_EXC
            REPORT_ERROR_TYPE_CREATE_THREAD -> ERROR_MSG_CREATE_THREAD
            REPORT_ERROR_TYPE_CREATE_RENDER -> ERROR_MSG_CREATE_RENDER
            REPORT_ERROR_TYPE_PARSE_CONFIG -> ERROR_MSG_PARSE_CONFIG
            REPORT_ERROR_TYPE_CONFIG_PLUGIN_MIX -> ERROR_MSG_CONFIG_PLUGIN_MIX
            else -> "unknown"
        } + " ${errorMsg ?: ""}"
    }

}