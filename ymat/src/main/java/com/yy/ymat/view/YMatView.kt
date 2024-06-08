package com.yy.ymat.view

import android.content.Context
import android.graphics.SurfaceTexture
import android.os.Handler
import android.os.Looper
import android.util.AttributeSet
import android.view.Surface
import android.view.TextureView
import android.widget.FrameLayout
import com.yy.ymat.bean.KeyInfo
import com.yy.ymat.player.IYMatListener
import com.yy.ymat.player.IYMatVideoListener
import com.yy.ymat.player.YMatPlayer
import com.yy.ymat.utils.IYMLog
import com.yy.ymat.utils.YMLog
import com.yy.ymat.utils.YMatJniUtils

/**
 * created by zengjiale
 */
class YMatView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0)
    : FrameLayout(context, attrs, defStyleAttr), IYMatView, TextureView.SurfaceTextureListener {

    private val TAG = "YMatView"

    private val uiHandler by lazy { Handler(Looper.getMainLooper()) }
    private var surface: SurfaceTexture? = null
    private var s: Surface? = null
    private var innerTextureView: YMatTextureView? = null
    private var handleListener: IYMatListener? = null

    init {
        innerTextureView = YMatTextureView(context).apply {
            surfaceTextureListener = this@YMatView
        }
        addView(innerTextureView)
    }

    private fun ui(f: () -> Unit) {
        if (Looper.myLooper() == Looper.getMainLooper()) f() else uiHandler.post { f() }
    }


    private val player = YMatPlayer(getContext())


    private fun hide() {
        ui {
            removeAllViews()
        }
    }


    override fun startPlay(address: String) {
//        player.parseYmat()
        player.decodeFromFile(address)
    }

    override fun startPlayFromAsset(assetsPath: String) {
        player.decodeFromAssets(assetsPath)
    }

    override fun loadAndPlayFromAssets(fileName: String) {
        player.loadAndPlayFromAssets(fileName)
    }

    override fun loadAndPlayFromFile(fileName: String) {
        player.loadAndPlayFromFile(fileName)
    }

    override fun loadAndPlayFromUrl(url: String) {
        player.loadAndPlayFromUrl(url)
    }

    override fun load(path: String) {
//        player.parseYmat()
        player.decodeFromFile(path)
    }

    override fun play() {
        player.play()
    }

    override fun pause() {
        player.pause()
    }

    override fun resume() {
        player.resume()
    }

    override fun stop() {
        player.stop()
    }

    override fun isRunning(): Boolean {
        return player.isRunning()
    }

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
        YMLog.i(TAG, "onSurfaceTextureAvailable width=$width height=$height")
        this.surface = surface
        player.initRender(surface)
    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {
        player.updateViewPoint(width, height)
    }

    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
        return true
    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {

    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
    }

    override fun onDetachedFromWindow() {
        YMLog.i(TAG, "onDetachedFromWindow")
        super.onDetachedFromWindow()
        release()

//        player.isDetachedFromWindow = true
//        player.onSurfaceTextureDestroyed()
        ui {
            uiHandler.removeCallbacksAndMessages(null)
            handleListener = null
        }
        innerTextureView?.surfaceTextureListener = null
        removeView(innerTextureView)
        innerTextureView = null
    }

    override fun replay() {
        player.replay()
    }

    override fun destroy() {
        player.release()
    }

    override fun setKeys(keyInfo: KeyInfo) {
        player.setKeys(keyInfo)
    }

    private fun release() {
        try {
            surface?.release()
        } catch (error: Throwable) {
            YMLog.e(TAG, "failed to release mSurfaceTexture= $surface: ${error.message}", error)
        }
        surface = null
    }

    override fun setLog(log: IYMLog) {
        YMLog.log = log
        YMatJniUtils.setLog(log)
    }

    override fun setLoop(isLoop: Boolean) {
        player.setLoop(isLoop)
    }

    override fun setVideoListener(listener: IYMatVideoListener) {
        val handleListener = object : IYMatVideoListener {
            override fun onVideoConfigReady(resId: Int): Boolean {
                ui {
                    listener.onVideoConfigReady(resId)
                }
                return true
            }

            override fun onVideoStart(resId: Int) {
                ui {
                    listener.onVideoStart(resId)
                }
            }

            override fun onVideoRestart(resId: Int) {
                ui {
                    listener.onVideoRestart(resId)
                }
            }

            override fun onVideoRender(frameIndex: Int, resId: Int) {
                ui {
                    listener.onVideoRender(frameIndex, resId)
                }
            }

            override fun onVideoComplete(resId: Int, lastFrame: Boolean) {
                ui {
                    listener.onVideoComplete(resId, lastFrame)
                }
            }

            override fun onVideoDestroy(resId: Int) {
                ui {
                    listener.onVideoDestroy(resId)
                }
            }

            override fun onFailed(resId: Int, errorType: Int, errorMsg: String?) {
                ui {
                    listener.onFailed(resId,errorType, errorMsg)
                }
            }
        }
        player.setVideoListener(handleListener)
    }

    override fun setYmatListener(listener: IYMatListener) {
        handleListener = object : IYMatListener {
            override fun onParsed() {
                ui {
                    listener.onParsed()
                }
            }
            override fun onLoaded() {
                ui {
                    listener.onLoaded()
                }
            }

            override fun onStart() {
                ui {
                    listener.onStart()
                }
            }

            override fun onRender(frameIndex: Int) {
                ui {
                    listener.onRender(frameIndex)
                }
            }

            override fun onComplete() {
                ui {
                    YMLog.i(TAG, "onComplete")
                    listener.onComplete()
                }
            }

            override fun onDestroy() {
                ui {
                    YMLog.i(TAG, "onDestroy")
                    listener.onDestroy()
                }
            }

            override fun onRestart() {
                ui {
                    listener.onRestart()
                }
            }

            override fun onFailed(errorMsg: String) {
                ui {
                    listener.onFailed(errorMsg)
                }
            }
        }
        player.setYmatListener(handleListener!!)
    }

    override fun setFillMode(fillMode: Int) {
        player.setParams(fillMode)
    }
}