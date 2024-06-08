package com.yy.ymat.utils

import android.media.MediaCodecList
import android.media.MediaDataSource
import android.media.MediaExtractor
import android.media.MediaFormat
import android.os.Build
import androidx.annotation.RequiresApi
import com.yy.ymat.bean.YMatMediaDataSource
import kotlin.collections.HashMap


object YMMediaUtil {

    private const val TAG = "YMMediaUtil"

    private var isTypeMapInit = false
    private val supportTypeMap = HashMap<String, Boolean>()

    const val MIME_HEVC = "video/hevc"

    fun getExtractor(ymMediaSource: YMatMediaDataSource): MediaExtractor {
        val extractor = MediaExtractor()
        if (ymMediaSource.path.isEmpty()) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                extractor.setDataSource(ymMediaSource)
            } else {
                YMLog.e(TAG, "setDataSource error api below 23")
            }
        } else {
            extractor.setDataSource(ymMediaSource.path)
        }
        return extractor
    }

    /**
     * 是否为h265的视频
     */
    fun checkIsHevc(videoFormat: MediaFormat):Boolean {
        val mime = videoFormat.getString(MediaFormat.KEY_MIME) ?: ""
        return mime.contains("hevc")
    }

    fun selectVideoTrack(extractor: MediaExtractor): Int {
        val numTracks = extractor.trackCount
        for (i in 0 until numTracks) {
            val format = extractor.getTrackFormat(i)
            val mime = format.getString(MediaFormat.KEY_MIME) ?: ""
            if (mime.startsWith("video/")) {
                YMLog.i(TAG, "Extractor selected track $i ($mime): $format")
                return i
            }
        }
        return -1
    }

    fun selectAudioTrack(extractor: MediaExtractor): Int {
        val numTracks = extractor.trackCount
        for (i in 0 until numTracks) {
            val format = extractor.getTrackFormat(i)
            val mime = format.getString(MediaFormat.KEY_MIME) ?: ""
            if (mime.startsWith("audio/")) {
                YMLog.i(TAG, "Extractor selected track $i ($mime): $format")
                return i
            }
        }
        return -1
    }

    /**
     * 检查设备解码支持类型
     */
    @Synchronized
    fun checkSupportCodec(mimeType: String): Boolean {
        if (!isTypeMapInit) {
            isTypeMapInit = true
            getSupportType()
        }
        return supportTypeMap.containsKey(mimeType.toLowerCase())
    }


    private fun getSupportType() {
        try {
            val numCodecs = MediaCodecList.getCodecCount()
            for (i in 0 until numCodecs) {
                val codecInfo = MediaCodecList.getCodecInfoAt(i)
                if (!codecInfo.isEncoder) {
                    continue
                }
                val types = codecInfo.supportedTypes
                for (j in types.indices) {
                    supportTypeMap[types[j].toLowerCase()] = true
                }
            }
            YMLog.i(TAG, "supportType=${supportTypeMap.keys}")
        } catch (t: Throwable) {
            YMLog.e(TAG, "getSupportType $t")
        }
    }

}