package com.yy.ymat.decoder

import android.graphics.SurfaceTexture
import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaExtractor
import android.media.MediaFormat
import android.os.Build
import android.view.Surface
import com.yy.ymat.utils.HandlerHolder
import com.yy.ymat.utils.YMLog
import com.yy.ymat.utils.YMatThread

/**
 * created by zengjiale
 */
class YMatHardDecoder {
    private var TAG = "YMatRender"
    var decodeThread: HandlerHolder = HandlerHolder(null, null)

    private val bufferInfo by lazy { MediaCodec.BufferInfo() }
    private var needDestroy = false

    // 动画的原始尺寸
    private var videoWidth = 0
    private var videoHeight = 0

    // 动画对齐后的尺寸
    private var alignWidth = 0
    private var alignHeight = 0
    
    private var outputFormat: MediaFormat? = null
    // 暂停
    private var isPause = false

    fun preapareThread(video: String) {
//        if (decodeThread == null) {
//            decodeThread = HandlerHolder(null, null)
//        }
        YMatThread.createThread(decodeThread!!, "ymat_decode_thread_${video}")
    }


    override fun start(evaFileContainer: IEvaFileContainer) {
        isStopReq = false
        isPause = false
        needDestroy = false
        isRunning = true
        renderThread.handler?.post {
            startPlay(evaFileContainer)
        }
    }

    override fun onFrameAvailable(surfaceTexture: SurfaceTexture?) {
        if (isStopReq) return
        YMLog.d(TAG, "onFrameAvailable")
        renderData()
    }

    /**
     * 渲染元素MP4
     */
    fun renderData() {
        renderThread.handler?.post {
            try {
                glTexture?.apply {
                    updateTexImage()
                    //渲染mp4数据
                    EvaJniUtil.renderFrame(playerEva.controllerId)
                    //元素混合
                    playerEva.pluginManager.onRendering()
                    //录制
                    if (playerEva.isVideoRecord) {
                        playerEva.mediaRecorder.encodeFrameInThread(false, timestamp)
                    }
                    //交换前后景数据
                    EvaJniUtil.renderSwapBuffers(playerEva.controllerId)
                }
            } catch (e: Throwable) {
                YMLog.e(TAG, "render exception=$e", e)
            }
        }
    }

    private fun startPlay(evaFileContainer: IEvaFileContainer) {
        var extractor: MediaExtractor? = null
        var decoder: MediaCodec? = null
        var format: MediaFormat? = null
        var trackIndex = 0
        var duration = 0L

        try {
            extractor = EvaMediaUtil.getExtractor(evaFileContainer)
            trackIndex = EvaMediaUtil.selectVideoTrack(extractor)
            if (trackIndex < 0) {
                throw RuntimeException("No video track found")
            }
            extractor.selectTrack(trackIndex)
            format = extractor.getTrackFormat(trackIndex)
            if (format == null) throw RuntimeException("format is null")

            // 是否支持h265
            if (EvaMediaUtil.checkIsHevc(format)) {
                if (Build.VERSION.SDK_INT  < Build.VERSION_CODES.LOLLIPOP
                    || !EvaMediaUtil.checkSupportCodec(EvaMediaUtil.MIME_HEVC)) {

                    onFailed(
                        EvaConstant.REPORT_ERROR_TYPE_HEVC_NOT_SUPPORT,
                        "${EvaConstant.ERROR_MSG_HEVC_NOT_SUPPORT} " +
                                "sdk:${Build.VERSION.SDK_INT}" +
                                ",support hevc:" + EvaMediaUtil.checkSupportCodec(EvaMediaUtil.MIME_HEVC))
                    release(null, null)
                    return
                }
            }

            videoWidth = format.getInteger(MediaFormat.KEY_WIDTH)
            videoHeight = format.getInteger(MediaFormat.KEY_HEIGHT)
            duration = format.getLong(MediaFormat.KEY_DURATION)
            if (!playerEva.isSetFps) {
                playerEva.fps = format.getInteger(MediaFormat.KEY_FRAME_RATE)
            }
            // 防止没有INFO_OUTPUT_FORMAT_CHANGED时导致alignWidth和alignHeight不会被赋值一直是0
            alignWidth = videoWidth
            alignHeight = videoHeight
            YMLog.i(TAG, "Video size is $videoWidth x $videoHeight")

            // 由于使用mediacodec解码老版本素材时对宽度1500尺寸的视频进行数据对齐，解码后的宽度变成1504，导致采样点出现偏差播放异常
            // 所以当开启兼容老版本视频模式并且老版本视频的宽度不能被16整除时要走YUV渲染逻辑
            // 但是这样直接判断有风险，后期想办法改
            needYUV = videoWidth % 16 != 0 && playerEva.enableVersion1

//            try {
//                if (!prepareRender(needYUV)) {
//                    throw RuntimeException("render create fail")
//                }
//            } catch (t: Throwable) {
//                onFailed(EvaConstant.REPORT_ERROR_TYPE_CREATE_RENDER, "${EvaConstant.ERROR_MSG_CREATE_RENDER} e=$t")
//                release(null, null)
//                return
//            }

            preparePlay(videoWidth, videoHeight)

            if (EvaJniUtil.getExternalTexture(playerEva.controllerId) != -1) {
                glTexture = playerEva.evaAnimView.getSurfaceTexture()?.apply {
                    setOnFrameAvailableListener(this@EvaHardDecoder)
                    setDefaultBufferSize(videoWidth, videoHeight)
                }
            } else {
                YMLog.e(TAG, "eva not init, can not get glTexture")
                return
            }
            EvaJniUtil.renderClearFrame(playerEva.controllerId)

        } catch (e: Throwable) {
            YMLog.e(TAG, "MediaExtractor exception e=$e", e)
            onFailed(EvaConstant.REPORT_ERROR_TYPE_EXTRACTOR_EXC, "${EvaConstant.ERROR_MSG_EXTRACTOR_EXC} e=$e")
            release(decoder, extractor)
            return
        }

        try {
            val mime = format.getString(MediaFormat.KEY_MIME) ?: ""
            YMLog.i(TAG, "Video MIME is $mime")
            decoder = MediaCodec.createDecoderByType(mime).apply {
                if (needYUV) {
                    format.setInteger(
                        MediaFormat.KEY_COLOR_FORMAT,
                        MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar
                    )
                    configure(format, null, null, 0)
                } else {
                    configure(format, Surface(glTexture), null, 0)
                }
                //跳转到需要的跳转位置
                if (playerEva.startPoint in 1 .. duration) {
                    extractor.seekTo(playerEva.startPoint, MediaExtractor.SEEK_TO_PREVIOUS_SYNC)
                    YMLog.i(TAG, "startPoint ${playerEva.startPoint}, sampleTime：${extractor.sampleTime}")
                    playerEva.sampleTime = extractor.sampleTime
                    extractor.advance()
                }
                start()
                decodeThread.handler?.post {
                    try {
                        startDecode(extractor, this)
                    } catch (e: Throwable) {
                        YMLog.e(TAG, "MediaCodec exception e=$e", e)
                        onFailed(EvaConstant.REPORT_ERROR_TYPE_DECODE_EXC, "${EvaConstant.ERROR_MSG_DECODE_EXC} e=$e")
                        release(decoder, extractor)
                    }
                }
            }
        } catch (e: Throwable) {
            YMLog.e(TAG, "MediaCodec configure exception e=$e", e)
            onFailed(EvaConstant.REPORT_ERROR_TYPE_DECODE_EXC, "${EvaConstant.ERROR_MSG_DECODE_EXC} e=$e")
            release(decoder, extractor)
            return
        }
    }

    override fun pause() {
        YMLog.i(TAG, "pause")
        isPause = true
    }

    override fun resume() {
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

        if (playerEva.startPoint > 0L) {
            // 得到key对应的帧
            frameIndex = (extractor.sampleTime / ((1000 * 1000 / playerEva.fps))).toInt() - 1
            YMLog.e(TAG, "decode frameIndex: $frameIndex")
        }

        while (!outputDone) {
            if (isStopReq) {
                YMLog.i(TAG, "stop decode")
                release(decoder, extractor)
                return
            }

            if (isPause) {
                continue
            }

            if (!inputDone) {
                val inputBufIndex = decoder.dequeueInputBuffer(TIMEOUT_USEC)
                if (inputBufIndex >= 0) {
                    val inputBuf = decoderInputBuffers[inputBufIndex]
                    val chunkSize = extractor.readSampleData(inputBuf, 0)
                    if (chunkSize < 0) {
                        decoder.queueInputBuffer(inputBufIndex, 0, 0, 0L, MediaCodec.BUFFER_FLAG_END_OF_STREAM)
                        inputDone = true
                        YMLog.i(TAG, "decode EOS")
                    } else {
                        val presentationTimeUs = extractor.sampleTime
                        decoder.queueInputBuffer(inputBufIndex, 0, chunkSize, presentationTimeUs, 0)
                        YMLog.i(TAG, "submitted frame $inputChunk to dec, size=$chunkSize")
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
                    decoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER -> YMLog.d(TAG, "no output from decoder available")
                    decoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED -> YMLog.d(TAG, "decoder output buffers changed")
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
                                playerEva.playLoop = playLoop // 消耗loop次数 自动恢复后能有正确的loop次数
                                outputDone = playLoop <= 0
                            }
                        }
                        val doRender = !outputDone
                        if (doRender) { //控制帧率时间
                            speedControlUtil.preRender(bufferInfo.presentationTimeUs)
                        }

                        if (needYUV && doRender) {
//                            yuvProcess(decoder, decoderStatus)
                        }
                        //消耗解码数据
                        // release & render
                        decoder.releaseOutputBuffer(decoderStatus, doRender && !needYUV)

                        if (frameIndex == 0 && !isLoop) {
                            onVideoStart()
                        }

                        playerEva.pluginManager.onDecoding(frameIndex)
                        onVideoRender(frameIndex, playerEva.configManager.config)
                        //输出帧数据
                        frameIndex++
                        YMLog.d(TAG, "decode frameIndex=$frameIndex")
                        if (loop > 0) {
                            YMLog.d(TAG, "Reached EOD, looping")
                            playerEva.pluginManager.onLoopStart()
                            extractor.seekTo(0, MediaExtractor.SEEK_TO_CLOSEST_SYNC)
                            inputDone = false
                            decoder.flush()
                            speedControlUtil.reset()
                            frameIndex = 0
                            isLoop = true
                            onVideoRestart()
                        }
                        if (outputDone) {  //输出完成
                            if (playerEva.isSetLastFrame) {
                                notReleaseLastFrame(decoder, extractor)
                            } else {
                                release(decoder, extractor)
                            }
                        }
                    }
                }
            }
        }
    }

    private fun release(decoder: MediaCodec?, extractor: MediaExtractor?) {
        renderThread.handler?.post {
            EvaJniUtil.renderClearFrame(playerEva.controllerId)
            try {
                YMLog.i(TAG, "release")
                decoder?.apply {
                    stop()
                    release()
                }
                extractor?.release()
                glTexture?.release()
                glTexture = null
                speedControlUtil.reset()
                playerEva.pluginManager.onRelease()
                playerEva.videoMode = EvaConstant.VIDEO_MODE_NORMAL_MP4 //重置为MP4
//                render?.releaseTexture()
                EvaJniUtil.releaseTexture(playerEva.controllerId)
            } catch (e: Throwable) {
                YMLog.e(TAG, "release e=$e", e)
            }
            isRunning = false
            onVideoComplete()
            if (playerEva.isVideoRecord) {
                playerEva.mediaRecorder.stopCodecRecordInThread()
            }
            if (needDestroy) {
                destroyInner()
            }
        }
    }

    private fun notReleaseLastFrame(decoder: MediaCodec?, extractor: MediaExtractor?) {
        YMLog.i(TAG, "notReleaseLastFrame")
        decoder?.apply {
            stop()
            release()
        }
        extractor?.release()
        speedControlUtil.reset()
        playerEva.pluginManager.onRelease()
        isRunning = false
        //播放到最后一帧，也认为播放完成
        onVideoComplete(true)
    }

    private fun releaseLastFrame() {
        if (playerEva.isSetLastFrame) {
            renderThread.handler?.post {
                EvaJniUtil.renderClearFrame(playerEva.controllerId)
                try {
                    YMLog.i(TAG, "releaseLastFrame")
                    glTexture?.release()
                    glTexture = null
//                render?.releaseTexture()
                    EvaJniUtil.releaseTexture(playerEva.controllerId)
                } catch (e: Throwable) {
                    YMLog.e(TAG, "release e=$e", e)
                }
                isRunning = false
                onVideoComplete()
                if (needDestroy) {
                    destroyInner()
                }
                playerEva.isSetLastFrame = false
            }
        }
    }

    override fun destroy() {
        if (isRunning) {
            needDestroy = true
            stop()
        } else {
            releaseLastFrame()
            destroyInner()
        }
    }

    private fun destroyInner() {
        YMLog.i(TAG, "destroyInner")
        renderThread.handler?.post {
            playerEva.pluginManager.onDestroy()
            EvaJniUtil.destroyRender(playerEva.controllerId)
            if (playerEva.isVideoRecord) {
                playerEva.mediaRecorder.stopCodecRecordInThread()
            }
            playerEva.controllerId = -1
            onVideoDestroy()
            destroyThread()
        }
    }

    fun destroyDecodeThread() {
        YMLog.i(TAG, "destroyDecodeThread")
        decodeThread?.handler?.removeCallbacksAndMessages(null)
        decodeThread?.thread = YMatDecoder.quitSafely(decodeThread?.thread)
        decodeThread?.handler = null
    }
}