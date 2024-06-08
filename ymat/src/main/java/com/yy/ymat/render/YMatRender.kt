package com.yy.ymat.render

import android.graphics.Bitmap
import android.graphics.SurfaceTexture
import android.os.Build
import android.view.Surface
import com.yy.ymat.bean.VideoDecodeInfo
import com.yy.ymat.bean.YMatPlayerInfo
import com.yy.ymat.utils.HandlerHolder
import com.yy.ymat.utils.YMatThread
import com.yy.ymat.utils.YMLog
import com.yy.ymat.utils.YMatConstant
import com.yy.ymat.utils.YMatJniUtils
import java.util.concurrent.CountDownLatch

/**
 * created by zengjiale
 */
class YMatRender(var playerInfo: YMatPlayerInfo) {
    private var TAG = "YMatRender"
    var renderThread = HandlerHolder(null, null)
    var s: Surface? = null

    init {
        prepareThread()
    }

    fun prepareThread() {
        YMatThread.createThread(renderThread, "ymat_render_thread")
    }

    fun onFrameAvailable(surfaceTexture: SurfaceTexture?) {
//        if (isStopReq) return
        YMLog.d(TAG, "onFrameAvailable")
    }

    fun initRender(st: SurfaceTexture, initResult: ((Boolean) -> Unit)? = null) {
        YMLog.i(TAG, "initRender playerId ${playerInfo.playerId}")
        renderThread.handler?.post {
            s = Surface(st)
            YMatJniUtils.initRender(playerInfo.playerId, s!!)
            initResult?.invoke(true)
        }
    }

    /**
     * 获取视频纹理id
     */
    fun getVideoTextureId(videoId: Int, vectorId: Int, countDownLatch: CountDownLatch,
                          block: (Int, Int, Int, SurfaceTexture)-> Unit) {
        YMLog.i(TAG, "getVideoTextureId playerId ${playerInfo.playerId}, videoId $videoId, vectorId $vectorId")
        renderThread.handler?.post {
            val textureId = YMatJniUtils.initVideoTexture(playerInfo.playerId, videoId, vectorId)
            block.invoke(videoId, YMatConstant.TEXTURE_VIDEO_RESOURCE, textureId, SurfaceTexture(textureId))
            countDownLatch.countDown()
        }
    }

    /**
     * 获取图片纹理id
     * type = 2
     */
    fun getImageTextureId(imageId: Int, bitmap: Bitmap, countDownLatch: CountDownLatch, block: (Int, Int, Int)-> Unit) {
        YMLog.i(TAG, "getImageTextureId playerId ${playerInfo.playerId}, imageId $imageId")
        getImageTextureId(imageId, YMatConstant.TEXTURE_IMAGE_RESOURCE, bitmap, countDownLatch, block)
    }

    /**
     * 动态更新图片
     */
    fun updateImageTextureInfo(imageId: Int, frameIndex: Int, bitmap: Bitmap) {
        renderThread.handler?.post {
            YMatJniUtils.updateBitmapTextureInfo(playerInfo.playerId, imageId, frameIndex, bitmap);
        }
    }

    /**
     * 动态纹理信息
     */
    fun updateTextureSize(restId: Int, width: Int, height: Int) {
        renderThread.handler?.post {
            YMatJniUtils.updateTextureSize(playerInfo.playerId, restId, width, height)
        }
    }

    /**
     * 获取纹理id
     * block playerId, type, textureId
     */
    fun getImageTextureId(id: Int, type: Int, bitmap: Bitmap, countDownLatch: CountDownLatch, block: (Int, Int, Int)-> Unit) {
        renderThread.handler?.post {
            block.invoke(id, type, YMatJniUtils.initImageTexture(playerInfo.playerId, id, bitmap))
            countDownLatch.countDown()
        }
    }

    /**
     * 等video和image textureId准备好后
     * 初始化渲染层
     */
    fun initLayer(block: (Boolean)-> Unit) {
        renderThread.handler?.post {
            //初始化渲染层
            block.invoke(YMatJniUtils.initLayer(playerInfo.playerId))
        }
    }

    fun readyFrame(frameIndex: Int, videoList: MutableList<VideoDecodeInfo>,
                   renderStart: ((Boolean) -> Unit)? = null,
                   renderFinish: ((Boolean) -> Unit) ? = null) {
        renderThread.handler?.post {
            try {
//                YMLog.i(TAG, "readyFrame $frameIndex")
                val isReady = YMatJniUtils.readyRender(playerInfo.playerId, frameIndex)
                YMLog.i(TAG, "readyFrame $frameIndex $isReady")
                renderStart?.invoke(isReady)
                if (isReady) {
                    try {
                        //设置解码器纹理数据刷新到surface
                        for (video in videoList) {
                            if (frameIndex in video.inFrame .. video.outFrame) {
                                if (video.sf != null) {
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                                        if (!video.sf?.isReleased!!) {
                                            video.sf?.updateTexImage()
                                        } else {
                                            YMLog.e(TAG, "readyFrame $video isreleased")
                                        }
                                    } else {
                                        video.sf?.updateTexImage()
                                    }
                                } else {
                                    YMLog.e(TAG, "readyFrame $video sf is null")
                                }
                            }
                        }
                        //渲染mp4数据
                        YMatJniUtils.renderFrame(playerInfo.playerId, frameIndex)
                        //交换前后景数据
                        YMatJniUtils.renderSwapBuffer(playerInfo.playerId, frameIndex)
                    } catch (e: Exception) {
                        YMLog.e(TAG, e.toString())
                    }
                }
                renderFinish?.invoke(isReady)
            } catch (e: Throwable) {
                YMLog.e(TAG, "render exception=$e", e)
            }
        }
    }

    fun updateViewPoint(width: Int, height: Int) {
        YMLog.i(TAG, "updateViewPoint playerId ${playerInfo.playerId} w $width, h $height")
        renderThread.handler?.post {
            YMatJniUtils.updateViewPoint(playerInfo.playerId, width, height)
        }
    }

    fun releaseRender() {
        YMLog.d(TAG, "releaseRender playerId ${playerInfo.playerId}")
        renderThread.handler?.post {
            YMatJniUtils.renderClearFrame(playerInfo.playerId)
            YMatJniUtils.releaseTexture(playerInfo.playerId)
        }
    }

    fun clearRender() {
        renderThread.handler?.post {
            YMatJniUtils.renderClearFrame(playerInfo.playerId)
        }
    }

    fun destroyLayer(deleteResult: (Boolean) -> Unit) {
        YMLog.d(TAG, "destroyRender playerId ${playerInfo.playerId}")
        renderThread.handler?.post {
            YMatJniUtils.destroyLayer(playerInfo.playerId)
            deleteResult.invoke(true)
        }
    }

    fun destroyRender(deleteResult: (Boolean) -> Unit) {
        YMLog.d(TAG, "destroyRender playerId ${playerInfo.playerId}")
        renderThread.handler?.post {
            YMatJniUtils.destroyRender(playerInfo.playerId)
            releaseThread()
            s?.release()
            s = null
            deleteResult.invoke(true)
        }
    }

    fun releaseThread() {
        YMLog.i(TAG, "destroyRenderThread playerId ${playerInfo.playerId}")
        renderThread.handler?.removeCallbacksAndMessages(null)
        renderThread.thread = YMatThread.quitSafely(renderThread.thread)
        renderThread.handler = null
    }

    fun setParams(fillMode: Int) {
        renderThread.handler?.post {
            YMatJniUtils.setParams(playerInfo.playerId, fillMode)
        }
    }
}