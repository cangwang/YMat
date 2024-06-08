package com.yy.ymat.bean

import android.graphics.SurfaceTexture

class VideoDecodeInfo {
    var id = 0  //视频id
    var vectorId = -1  //所属组合id
    var textureId = -1  //纹理id
    var startPoint = 0L
    var sampleTime = 0L
    var fps = 30
    var isSetFps = false
    var isSetLastFrame = false
    var inFrame = 0
    var outFrame = 0
    var width = 0
    var height = 0
    var name = ""
    var sf: SurfaceTexture? = null
}