package com.yy.ymat.decoder

import android.graphics.SurfaceTexture
import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.os.Build
import android.os.HandlerThread
import android.view.Surface
import com.yy.ymat.bean.VideoDecodeInfo
import com.yy.ymat.bean.YMatMediaDataSource
import com.yy.ymat.bean.YMatPlayerInfo
import com.yy.ymat.player.IYMatVideoListener
import com.yy.ymat.utils.HandlerHolder
import com.yy.ymat.utils.YMLog
import com.yy.ymat.utils.YMMediaUtil
import com.yy.ymat.utils.YMatConstant
import com.yy.ymat.utils.YMatJniUtils
import com.yy.ymat.utils.YMatThread
import java.util.concurrent.CountDownLatch

/**
 * created by zengjiale
 */
class YMatHardDecoder(val decodeInfo: VideoDecodeInfo,
                      val playerInfo: YMatPlayerInfo,
                      val sf: SurfaceTexture,
                      val videoLatch: CountDownLatch,
                      var ymatAnimListener: IYMatVideoListener? = null): SurfaceTexture.OnFrameAvailableListener {
    private var TAG = "YMatHardDecoder-${decodeInfo.id}"
    var decodeThread: HandlerHolder = HandlerHolder(null, null)

    private val bufferInfo by lazy { MediaCodec.BufferInfo() }
    private var needDestroy = false

    // 动画的原始尺寸
    var videoWidth = 0
    var videoHeight = 0

    // 动画对齐后的尺寸
    private var alignWidth = 0
    private var alignHeight = 0

    private var outputFormat: MediaFormat? = null
    // 暂停
    private var isPause = false

    private var glTexture: SurfaceTexture? = null

    @Volatile
    private var awaitSync = false

    var fps: Int = decodeInfo.fps
    var playLoop = 1 // 循环播放次数
    var isLoop = false //无限循环
    var isRunning = false // 是否正在运行
    var isStopReq = false // 是否需要停止
    var mediaSource: YMatMediaDataSource? = null

    var extractor: MediaExtractor? = null
    var decoder: MediaCodec? = null
    private val lock = Object()  //用于暂停和帧抽取速度调整
    private var surface: Surface? = null

    fun prepareThread(video: String) {
        YMatThread.createThread(decodeThread, "ymat_decode_thread_${video}")
    }

    fun setListener(ymatAnimListener: IYMatVideoListener) {
        this.ymatAnimListener = ymatAnimListener
    }

    override fun onFrameAvailable(surfaceTexture: SurfaceTexture?) {

    }

    fun start(mediaSource: YMatMediaDataSource) {
        YMLog.i(TAG, "start mediaSource ${mediaSource.extend}")
        isStopReq = false
        isPause = false
        needDestroy = false
        isRunning = true
        prepareThread(decodeInfo.name)
        this.mediaSource = mediaSource
        startDecode(mediaSource)
    }

    private fun startDecode(mediaSource: YMatMediaDataSource) {
        var format: MediaFormat? = null
        var trackIndex = 0
        var duration = 0L

        try {
            extractor = YMMediaUtil.getExtractor(mediaSource)
            trackIndex = YMMediaUtil.selectVideoTrack(extractor!!)
            if (trackIndex < 0) {
                throw RuntimeException("No video track found")
            }
            extractor!!.selectTrack(trackIndex)
            format = extractor!!.getTrackFormat(trackIndex)
            if (format == null) throw RuntimeException("format is null")

            // 是否支持h265
            if (YMMediaUtil.checkIsHevc(format)) {
                if (!YMMediaUtil.checkSupportCodec(YMMediaUtil.MIME_HEVC)) {

                    onFailed(
                        YMatConstant.REPORT_ERROR_TYPE_HEVC_NOT_SUPPORT,
                        "${YMatConstant.ERROR_MSG_HEVC_NOT_SUPPORT} " +
                                "sdk:${Build.VERSION.SDK_INT}" +
                                ",support hevc:" + YMMediaUtil.checkSupportCodec(YMMediaUtil.MIME_HEVC))
                    release(null, null)
                    return
                }
            }

            videoWidth = format.getInteger(MediaFormat.KEY_WIDTH)
            videoHeight = format.getInteger(MediaFormat.KEY_HEIGHT)
            duration = format.getLong(MediaFormat.KEY_DURATION)
//            if (!decodeInfo.isSetFps) {
//                decodeInfo.fps = format.getInteger(MediaFormat.KEY_FRAME_RATE)
//            }
            // 防止没有INFO_OUTPUT_FORMAT_CHANGED时导致alignWidth和alignHeight不会被赋值一直是0
            alignWidth = videoWidth
            alignHeight = videoHeight
            YMLog.i(TAG, "Video size is $videoWidth x $videoHeight")

            glTexture = sf.apply {
                setOnFrameAvailableListener(this@YMatHardDecoder)
                setDefaultBufferSize(videoWidth, videoHeight)
            }
        } catch (e: Throwable) {
            YMLog.e(TAG, "MediaExtractor exception e=$e", e)
            onFailed(YMatConstant.REPORT_ERROR_TYPE_EXTRACTOR_EXC, "${YMatConstant.ERROR_MSG_EXTRACTOR_EXC} e=$e")
            release(decoder, extractor)
            return
        }

        try {
            val mime = format.getString(MediaFormat.KEY_MIME) ?: ""
            YMLog.i(TAG, "Video MIME is $mime")
            decoder = MediaCodec.createDecoderByType(mime).apply {
                surface?.release()
                surface = Surface(glTexture)
                configure(format, surface, null, 0)
                //跳转到需要的跳转位置
                if (decodeInfo.startPoint in 1 .. duration) {
                    extractor!!.seekTo(decodeInfo.startPoint, MediaExtractor.SEEK_TO_PREVIOUS_SYNC)
                    YMLog.i(TAG, "startPoint ${decodeInfo.startPoint}, sampleTime：${extractor!!.sampleTime}")
                    decodeInfo.sampleTime = extractor!!.sampleTime
                    extractor!!.advance()
                }
                start()
                decodeThread.handler?.post {
                    try {
                        startDecode(extractor!!, this)
                    } catch (e: Throwable) {
                        YMLog.e(TAG, "MediaCodec exception e=$e", e)
                        onFailed(YMatConstant.REPORT_ERROR_TYPE_DECODE_EXC, "${YMatConstant.ERROR_MSG_DECODE_EXC} e=$e")
                        release(decoder, extractor)
                    }
                }
            }
        } catch (e: Throwable) {
            YMLog.e(TAG, "MediaCodec configure exception e=$e", e)
            onFailed(YMatConstant.REPORT_ERROR_TYPE_DECODE_EXC, "${YMatConstant.ERROR_MSG_DECODE_EXC} e=$e")
            release(decoder, extractor)
            return
        }
    }

    fun pause() {
        YMLog.i(TAG, "pause")
        isPause = true
    }

    fun resume() {
        YMLog.i(TAG, "resume")
        isPause = false
    }

    private fun startDecode(extractor: MediaExtractor, decoder: MediaCodec) {
        val TIMEOUT_USEC = 10000L
        var inputChunk = 0
        var outputDone = false
        var inputDone = false
        var frameIndex = 0
        var isLoop = false

        val decoderInputBuffers = decoder.inputBuffers

        if (decodeInfo.startPoint > 0L) {
            // 得到key对应的帧
            frameIndex = (extractor.sampleTime / ((1000 * 1000 / decodeInfo.fps))).toInt() - 1
            YMLog.e(TAG, "decode frameIndex: $frameIndex")
        }

        synchronized(lock) {
        while (!outputDone && glTexture != null) {
            if (isStopReq) {
                YMLog.i(TAG, "stop decode")
                release(decoder, extractor)
                return
            }

            if (awaitSync || isPause) {
                lock.wait()
            }

            if (!inputDone) {
                val inputBufIndex = decoder.dequeueInputBuffer(TIMEOUT_USEC)
                if (inputBufIndex >= 0) {
                    val inputBuf = decoderInputBuffers[inputBufIndex]
                    val chunkSize = extractor.readSampleData(inputBuf, 0)
                    if (chunkSize < 0) {
                        decoder.queueInputBuffer(
                            inputBufIndex,
                            0,
                            0,
                            0L,
                            MediaCodec.BUFFER_FLAG_END_OF_STREAM
                        )
                        inputDone = true
                        YMLog.i(TAG, "decode EOS")
                    } else {
                        val presentationTimeUs = extractor.sampleTime
                        decoder.queueInputBuffer(inputBufIndex, 0, chunkSize, presentationTimeUs, 0)
//                        YMLog.i(TAG, "submitted frame $inputChunk to dec, size=$chunkSize")
                        inputChunk++
                        extractor.advance()
                    }
                } else {
                    YMLog.d(TAG, "input buffer not available")
                }
            }

            if (!outputDone) {
                //抽取解码数据或状态
                val decoderStatus = decoder.dequeueOutputBuffer(bufferInfo, TIMEOUT_USEC)
                when {
                    decoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER -> YMLog.d(
                        TAG,
                        "no output from decoder available"
                    )

                    decoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED -> YMLog.d(
                        TAG,
                        "decoder output buffers changed"
                    )

                    decoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED -> {
                        outputFormat = decoder.outputFormat
                        outputFormat?.apply {
                            try {
                                // 有可能取到空值，做一层保护
                                val stride = getInteger("stride")
                                val sliceHeight = getInteger("slice-height")
                                if (stride > 0 && sliceHeight > 0) {
                                    alignWidth = stride
                                    alignHeight = sliceHeight
                                }
                            } catch (t: Throwable) {
                                YMLog.e(TAG, "$t", t)
                            }
                        }
                        YMLog.i(TAG, "decoder output format changed: $outputFormat")
                    }

                    decoderStatus < 0 -> {
                        throw RuntimeException("unexpected result from decoder.dequeueOutputBuffer: $decoderStatus")
                    }

                    else -> {  //正常解析
                        var loop = 0
                        if (bufferInfo.flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM != 0) {
                            if (this.isLoop) {
                                loop = 1
                            } else {
                                loop = --playLoop
                                playerInfo.playLoop = playLoop // 消耗loop次数 自动恢复后能有正确的loop次数
                                outputDone = playLoop <= 0
                            }
                        }
                        val doRender = !outputDone
//                        if (doRender) { //控制帧率时间
//                            speedControlUtil.preRender(bufferInfo.presentationTimeUs)
//                        }


                        videoLatch.countDown() //认为能够正常运行，通知同步
                        videoLatch.await()

//                        if (needYUV && doRender) {
////                            yuvProcess(decoder, decoderStatus)
//                        }
                        //消耗解码数据
                        // release & render
                        decoder.releaseOutputBuffer(decoderStatus, doRender)

                        if (frameIndex == 0 && !isLoop) {
                            onVideoStart()
                        }

                        onVideoRender(frameIndex)
//                        onVideoRender(frameIndex, decodeInfo.configManager.config)
                        //输出帧数据
                        frameIndex++

                        //等待上一帧渲染完
                        if (!outputDone) {
                            //这个frameIndex是这个解码器里面的视频帧，并不是绘制的实际帧
                            setAwaitSync(true, frameIndex, true)
                        }
                        YMLog.i(TAG, "decode frameIndex=$frameIndex")
                    }
                }
            }
        }
        }
    }

    private fun release(decoder: MediaCodec?, extractor: MediaExtractor?, latch: CountDownLatch? = null) {
        decodeThread.handler?.post {
            try {
                YMLog.i(TAG, "decoder release")
                decoder?.apply {
                    stop()
                    release()
                }
                extractor?.release()
//                glTexture?.release()
//                glTexture = null
////                render?.releaseTexture()
//                YMatJniUtils.releaseVideoTexture(playerInfo.playerId, decodeInfo.id, decodeInfo.vectorId, decodeInfo.textureId)
            } catch (e: Throwable) {
                YMLog.e(TAG, "release e=$e", e)
            }
            isRunning = false
            onVideoComplete()
//            if (decodeInfo.isVideoRecord) {
//                decodeInfo.mediaRecorder.stopCodecRecordInThread()
//            }
            if (needDestroy) {
                destroyInner()
            }
            latch?.countDown()
        }
    }

    fun releaseTexture() {
        sf.release()
        sf.setOnFrameAvailableListener(null)
        glTexture?.setOnFrameAvailableListener(null)
        glTexture?.release()
        glTexture = null
        surface?.release()
    }

    private fun notReleaseLastFrame(decoder: MediaCodec?, extractor: MediaExtractor?) {
        YMLog.i(TAG, "notReleaseLastFrame")
        decoder?.apply {
            stop()
            release()
        }
        extractor?.release()
        isRunning = false
        //播放到最后一帧，也认为播放完成
        onVideoComplete(true)
    }

    private fun releaseLastFrame() {
        if (decodeInfo.isSetLastFrame) {
            decodeThread.handler?.post {
                YMatJniUtils.renderClearFrame(decodeInfo.textureId)
                try {
                    YMLog.i(TAG, "releaseLastFrame")
                    releaseTexture()
//                render?.releaseTexture()
                    YMatJniUtils.releaseTexture(decodeInfo.textureId)
                } catch (e: Throwable) {
                    YMLog.e(TAG, "release e=$e", e)
                }
                isRunning = false
                onVideoComplete()
                if (needDestroy) {
                    destroyInner()
                }
                decodeInfo.isSetLastFrame = false
            }
        }
    }

    fun destroy(latch: CountDownLatch? = null) {
        synchronized(lock) {
            lock.notifyAll()
//            if (isRunning) {
                needDestroy = true
                stop()
                release(decoder, extractor, latch)
                releaseTexture()
//            } else if (glTexture != null) {
//                releaseLastFrame()
//                destroyInner()
//            }
            ymatAnimListener = null
            mediaSource?.close()
            mediaSource = null
        }
    }

    fun stop() {
        YMLog.i(TAG, "stop true")
        isStopReq = true
    }

    private fun destroyInner() {
        YMLog.i(TAG, "destroyInner")

//            decodeInfo.pluginManager.onDestroy()
//            YMatJniUtils.destroyRender(decodeInfo.playerId)
//            if (decodeInfo.isVideoRecord) {
//                decodeInfo.mediaRecorder.stopCodecRecordInThread()
//            }
        onVideoDestroy()
        destroyDecodeThread()
    }

    private fun destroyDecodeThread() {
        YMLog.i(TAG, "destroyDecodeThread")
        decodeThread.handler?.removeCallbacksAndMessages(null)
        decodeThread.thread = quitSafely(decodeThread.thread)
    }

    private fun onVideoStart() {
        YMLog.i(TAG, "onVideoStart")
        ymatAnimListener?.onVideoStart(decodeInfo.id)
    }

    private fun onVideoRestart() {
        YMLog.i(TAG, "onVideoRestart")
        ymatAnimListener?.onVideoRestart(decodeInfo.id)
    }

    private fun onVideoRender(frameIndex: Int) {
        YMLog.i(TAG, "onVideoRender $frameIndex")
        ymatAnimListener?.onVideoRender(frameIndex, decodeInfo.id)
    }

    private fun onVideoComplete(lastFrame: Boolean = false) {
        YMLog.i(TAG, "onVideoComplete")
        ymatAnimListener?.onVideoComplete(decodeInfo.id, lastFrame)
    }

    private fun onVideoDestroy() {
        YMLog.i(TAG, "onVideoDestroy")
        ymatAnimListener?.onVideoDestroy(decodeInfo.id)
    }

    private fun onFailed(errorType: Int, errorMsg: String?) {
        YMLog.e(TAG, "onFailed errorType=$errorType, errorMsg=$errorMsg")
        ymatAnimListener?.onFailed(decodeInfo.id, errorType, errorMsg)
    }

    fun setAwaitSync(awaitSync: Boolean, frameIndex: Int, force: Boolean = false) {
        YMLog.i(TAG, "setAwaitSync $frameIndex, $awaitSync, force $force")
        if (force || (frameIndex >= decodeInfo.inFrame && frameIndex <= decodeInfo.outFrame)) {
            this.awaitSync = awaitSync
        }
        synchronized(lock) {
            if (!this.awaitSync) {
                lock.notifyAll()
            }
        }
//        decodeThread.handler?.post {
//            YMLog.i(TAG, "handler setAwaitSync $awaitSync")
//            this.awaitSync = awaitSync
//        }
    }

    fun releaseDecoder() {
        decoder?.apply {
            stop()
            release()
        }
        extractor?.release()
    }

    fun replay() {
        if (mediaSource != null) {
            YMLog.i(TAG, "replay decoder release")
            start(mediaSource!!)
            onVideoRestart()
        }
    }

    private fun quitSafely(thread: HandlerThread?): HandlerThread? {
        thread?.apply {
            thread.quitSafely()
        }
        return null
    }
}