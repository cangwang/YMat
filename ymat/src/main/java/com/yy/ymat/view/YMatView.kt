package com.yy.ymat.view

import android.content.Context
import android.content.res.AssetManager
import android.graphics.SurfaceTexture
import android.util.AttributeSet
import android.view.TextureView
import android.widget.FrameLayout
import com.yy.ymat.YMatrixPlayer
import java.io.File

/**
 * created by zengjiale
 */
class YMatView@JvmOverloads constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0):
    IYMatView,
    FrameLayout(context, attrs, defStyleAttr),
    TextureView.SurfaceTextureListener {

    private val player = YMatrixPlayer()
    override fun startPlay(file: File) {

    }

    override fun startPlay(assetManager: AssetManager, assetsPath: String) {

    }

    override fun pause() {

    }

    override fun resume() {

    }

    override fun stopPlay() {

    }

    override fun isRunning(): Boolean {
        return false
    }

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {

    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {

    }

    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
        return true
    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {

    }

}