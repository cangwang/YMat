package com.yy.ymat.utils

import android.graphics.Bitmap
import android.view.Surface
import androidx.annotation.Keep
import com.yy.ymat.player.YMatPlayer

/**
 * created by zengjiale
 */
@Keep
object YMatJniUtils {
    init {
        System.loadLibrary("ymat")
    }

    external fun createPlayer(useTGFX: Boolean = false): Int

    external fun setYMatConfig(playerId: Int, json: String, player: YMatPlayer?)

    external fun initRender(playerId: Int, surface: Surface)

    external fun initLayer(playerId: Int): Boolean

    external fun initVideoTexture(playerId: Int, videoId: Int, vectorId: Int): Int

    external fun initImageTexture(playerId: Int, imageId: Int, bitmap: Bitmap): Int

    external fun renderClearFrame(playerId: Int)

    external fun updateViewPoint(playerId: Int, width: Int, height: Int)

    external fun readyRender(playerId: Int, frameIndex: Int): Boolean
    external fun renderFrame(playerId: Int, frameIndex: Int)
    external fun renderSwapBuffer(playerId: Int, frameIndex: Int)
    external fun releaseTexture(playerId: Int)

    external fun releaseVideoTexture(playerId: Int, videoId: Int, vectorId: Int, videoTextureId: Int)

    external fun updateTextureSize(playerId: Int, resId: Int, width: Int, height: Int)

    external fun updateBitmapTextureInfo(playerId: Int, resId: Int, frameIndex: Int, bitmap: Bitmap)

    external fun destroyRender(playerId: Int)

    external fun destroyLayer(playerId: Int)

    external fun setLog(log: IYMLog)

    external fun showImage(playerId: Int, bitmap: Bitmap)

    external fun setParams(playerId: Int, fillMode: Int)
}