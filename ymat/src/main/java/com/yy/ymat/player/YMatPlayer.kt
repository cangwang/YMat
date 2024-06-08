package com.yy.ymat.player

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.PorterDuff
import android.graphics.PorterDuffXfermode
import android.graphics.Rect
import android.graphics.SurfaceTexture
import android.graphics.Typeface
import androidx.annotation.Keep
import androidx.core.util.Pools.SynchronizedPool
import com.yy.ymat.bean.ElementsInfo
import com.yy.ymat.bean.KeyInfo
import com.yy.ymat.bean.ShapeInfoCache
import com.yy.ymat.bean.ShapeLayerInfo
import com.yy.ymat.bean.TexDocAttr
import com.yy.ymat.bean.VideoDecodeInfo
import com.yy.ymat.bean.YMatDataHeader
import com.yy.ymat.bean.YMatDataNode
import com.yy.ymat.bean.YMatMediaDataSource
import com.yy.ymat.bean.YMatPlayerInfo
import com.yy.ymat.decoder.YMatAudioDecoder
import com.yy.ymat.decoder.YMatHardDecoder
import com.yy.ymat.decoder.YMatParser
import com.yy.ymat.render.BaseShapeLayer
import com.yy.ymat.render.EllipseLayer
import com.yy.ymat.render.PathLayer
import com.yy.ymat.render.RectLayer
import com.yy.ymat.render.YMatRender
import com.yy.ymat.utils.HandlerHolder
import com.yy.ymat.utils.JsonParser
import com.yy.ymat.utils.YMLog
import com.yy.ymat.utils.YMatJniUtils
import com.yy.ymat.utils.YMatThread
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.CopyOnWriteArrayList
import java.util.concurrent.CountDownLatch


/**
 * created by zengjiale
 */
class YMatPlayer(var context: Context, var useTGFX: Boolean = false) {
    private val TAG = "YMatPlayer"
    var videoDecoderList = CopyOnWriteArrayList<YMatHardDecoder>()  //视频硬解
    var audioDecoder: YMatAudioDecoder? = null    //音频解码
    var videoInfoList = CopyOnWriteArrayList<VideoDecodeInfo>()  //视频基础信息
    var playerInfo = YMatPlayerInfo()  //播放器基础信息
    var syner: YMatSyner? = null  //同步器
    var render: YMatRender? = null  //总渲染器

    @Volatile
    private var isRunning = false
    private var parser = YMatParser(context)
    private var videoSourceMap = ConcurrentHashMap<Int, YMatMediaDataSource>()  //资源解析后的视频
    private var bitmapSourceMap = ConcurrentHashMap<Int, Bitmap>()              //资源解析后的图片
    private var videoLatch: CountDownLatch? = null

    @Volatile
    private var decoderInitLatch: CountDownLatch? = null

    private val shapeInfoMap = ConcurrentHashMap<Int, ShapeInfoCache>()

    //实际播放的帧数
    private var playFrameIndex = 0

    //处理线程
    private var shapeParseThread: HandlerHolder = HandlerHolder(null, null)

    private val canvasPool = SynchronizedPool<Canvas>(3)

    private var keyInfoMap = ConcurrentHashMap<Int, KeyInfo>()

    private var isWrapContent = true
    private val erasePaint = Paint().apply {
        xfermode = PorterDuffXfermode(PorterDuff.Mode.CLEAR)
    }
    private var json: String = "" //播放解析内容
    private var isLoop = false

    private val LOOP_DELAY_TIME = 300L

    private var yMatVideoListener: IYMatVideoListener? = null

    private var yMatListener: IYMatListener? = null
    private var playAfterDecode = false

    init {
        YMatThread.createThread(shapeParseThread, "ymat_shape_parse_thread")
        playerInfo.playerId = YMatJniUtils.createPlayer(useTGFX)
    }

    fun setVideoListener(listener: IYMatVideoListener) {
        this.yMatVideoListener = listener
        for (decoder in videoDecoderList) {
            decoder.setListener(listener)  //先销毁播放解码器
        }
    }

    fun setYmatListener(listener: IYMatListener) {
        this.yMatListener = listener
    }

    fun loadAndPlayFromAssets(fileName: String) {
        decodeFromAssets(fileName, true)
    }

    /**
     * 解析文件
     */
    fun decodeFromAssets(fileName: String, playAfterDecode: Boolean = false) {
        isRunning = true
        this.playAfterDecode = playAfterDecode
        parser.decodeFromAssets(fileName, object : YMatParser.ParseCompletionCallback {
            override fun onComplete(data: List<YMatDataNode>) {
                val result = handleYMatDataList(data)
                YMLog.d(TAG, "decodeFromAssets $result")
            }

            override fun onError(errorCode: Int) {
                YMLog.d(TAG, "${YMatParser.getError(errorCode)}")
            }
        })
    }

    fun loadAndPlayFromFile(fileName: String) {
        decodeFromFile(fileName, true)
    }

    fun decodeFromFile(fileName: String, playAfterDecode: Boolean = false) {
        isRunning = true
        this.playAfterDecode = playAfterDecode
        parser.decodeFromFile(fileName, object : YMatParser.ParseCompletionCallback {
            override fun onComplete(data: List<YMatDataNode>) {
                val result = handleYMatDataList(data)
                YMLog.d(TAG, "decodeFromFile $result")
            }

            override fun onError(errorCode: Int) {
                YMLog.d(TAG, "${YMatParser.getError(errorCode)}")
            }
        })
    }

    fun loadAndPlayFromUrl(url: String) {
        decodeFromUrl(url, true)
    }

    fun decodeFromUrl(url: String, playAfterDecode: Boolean = false) {
        isRunning = true
        this.playAfterDecode = playAfterDecode
        parser.decodeFromUrl(url, object : YMatParser.ParseCompletionCallback {
            override fun onComplete(data: List<YMatDataNode>) {
                val result = handleYMatDataList(data)
                YMLog.d(TAG, "decodeFromUrl $url")
            }

            override fun onError(errorCode: Int) {
                YMLog.d(TAG, "${YMatParser.getError(errorCode)}")
            }
        }, true)
    }

    fun handleYMatDataList(data: List<YMatDataNode>): Boolean {
        var hasFileDesc = false
        YMLog.d(TAG, "handleYMatDataList $data")
        var bodyTxt = ""
        for (d in data) {
            when (d.head?.type) {
                YMatDataHeader.TYPE_DESC -> {  //文件描述文本
                    if (d.bodyTxt.isNullOrEmpty().not()) {
                        bodyTxt = d.bodyTxt!!
                        hasFileDesc = true
                    } else {
                        YMLog.e(TAG, "parseYmat bodyTxt empty")
                        break
                    }
                }

                YMatDataHeader.TYPE_IMG -> {  //图片资源
                    val id = d.head!!.extend
                    if (keyInfoMap.containsKey(id)) { //替换自定义图片资源
                        val bitmap = if (keyInfoMap[id]!!.from == 0) {
                            BitmapFactory.decodeResource(context.resources, keyInfoMap[id]!!.path.toInt())
                        } else {
                            BitmapFactory.decodeFile(keyInfoMap[id]!!.path)
                        }
                        bitmapSourceMap[id] = bitmap
                    } else {
                        d.bitmap?.let {
                            bitmapSourceMap[id] = it
                        }
                    }
                }

                YMatDataHeader.TYPE_VIDEO -> { //视频资源
                    val id = d.head!!.extend
                    d.body?.let {
                        val videoSource = YMatMediaDataSource(id, it)
                        if (keyInfoMap.containsKey(id)) { //替换视频资源
                            videoSource.path = keyInfoMap[id]!!.path
                        }
                        videoSourceMap[id] = videoSource
//                        videoDecoderList[d.head!!.extend]?.start(videoSource)
                    }
                }

                YMatDataHeader.TYPE_AUDIO -> { //音频资源

                }

                else -> {
                    YMLog.e(TAG, "unknown data node")
                }
            }
        }
        if (hasFileDesc) {
            //Native层解析ymat文件层级
            parseYmat(bodyTxt)
        }
        return hasFileDesc
    }

    /**
     * 解析运行json
     */
    fun parseYmat(json: String) {
        YMLog.i(TAG, "parseYMat playerId ${playerInfo.playerId}, $json")
        this.json = json
        //解析ymat文件信息
        YMatJniUtils.setYMatConfig(playerInfo.playerId, json, this)
        yMatListener?.onParsed()
        shapeParseThread.handler?.post {
            createDecoder()
        }
    }

    private fun createDecoder() {
        //要等解析完成并且view初始化完成
        if (render != null
            && decoderInitLatch == null //通过decoderInitLatch判空来做不重复进去
            && (videoSourceMap.isNotEmpty() || bitmapSourceMap.isNotEmpty())
        ) {
            //资源同步 视频+图片
            decoderInitLatch = CountDownLatch(videoInfoList.size + bitmapSourceMap.size)
            YMLog.i(TAG, "createDecoder")
            if (videoInfoList.isNotEmpty()) {
                //需要等待多少个Video解码器初始化
                videoLatch = CountDownLatch(videoInfoList.size)
                //解析完成后，创建视频纹理，初始化解码器
                for (info in videoInfoList) {
                    initVideoTexture(
                        info.id,
                        info.vectorId,
                        decoderInitLatch!!
                    ) { videoId, type, textureId, st ->
                        YMLog.i(TAG, "initVideoTexture videoId $videoId, textureId $textureId")
                        info.textureId = textureId
                        info.sf = st
                        val decoder =
                            YMatHardDecoder(info, playerInfo, st, videoLatch!!, yMatVideoListener)
                        videoDecoderList.add(decoder)
                    }
                }
            }
            if (bitmapSourceMap.isNotEmpty()) {
                //创建图片纹理
                for (source in bitmapSourceMap.entries) {
                    initImageTexture(
                        source.key,
                        source.value,
                        decoderInitLatch!!
                    ) { id, type, textureId ->
                        YMLog.i(TAG, "initImageTexture id ${source.key}, textureId $textureId")
                    }
                }
            }

//            decoderInitLatch?.await(1, TimeUnit.SECONDS)
            //同步等待countDown和获取不能同一个线程
            decoderInitLatch?.await()
            yMatListener?.onLoaded()
            if (this.playAfterDecode) {
                play()
            }
        }
    }

    fun initVideoTexture(
        videoId: Int,
        vectorId: Int,
        countDownLatch: CountDownLatch,
        block: (Int, Int, Int, SurfaceTexture) -> Unit
    ) {
        render?.getVideoTextureId(videoId, vectorId, countDownLatch, block)
    }

    fun initImageTexture(
        imageId: Int, image: Bitmap,
        countDownLatch: CountDownLatch,
        block: (Int, Int, Int) -> Unit
    ) {
        render?.getImageTextureId(imageId, image, countDownLatch, block)
    }

    fun updateTextureSize(resId: Int, width: Int, height: Int) {
        render?.updateTextureSize(resId, width, height);
    }

    fun updateImageTextureInfo(imageId: Int, frameIndex: Int, image: Bitmap) {
        render?.updateImageTextureInfo(imageId, frameIndex, image)
    }

    fun initRender(surface: SurfaceTexture) {
        YMLog.i(TAG, "initRender")
        render = YMatRender(playerInfo)
        render?.initRender(surface) {
            success->
            if (success) {
                shapeParseThread.handler?.post {
                    createDecoder()
                }
            } else {
                YMLog.e(TAG, "init render error")
                clear()
                yMatListener?.onComplete()
                YMLog.i(TAG, "onComplete")
            }
        }
    }

    fun updateViewPoint(width: Int, height: Int) {
        render?.updateViewPoint(width, height)
    }

    fun play() {
        //等待解码器和图片资源准备完成
//        decoderInitLatch?.await()
        YMLog.i(TAG, "play")
        isRunning = true
        //启动解码器
        for (decoder in videoDecoderList) {
            if (videoSourceMap.containsKey(decoder.decodeInfo.id)) {
                decoder.start(videoSourceMap[decoder.decodeInfo.id]!!)
                //解码后立刻更新纹理宽高
                updateTextureSize(decoder.decodeInfo.id, decoder.videoWidth, decoder.videoHeight)
            }
        }
//        audioDecoder?.start()

        //解析完成后，初始化同步器
        syner?.cancelTimer()  //防止万一多线程出现问题
        syner = YMatSyner(playerInfo)
        playFrameIndex = 0
        syner?.addFrameListener(object : YMatSyner.YMatFrameListener {
            override fun startFrame(frameIndex: Int) {
                //检测是否都准备就绪
                render?.readyFrame(playFrameIndex, videoInfoList, null) { isReady ->
                    if (playFrameIndex == 0) {
                        yMatListener?.onStart()
                    }
                    YMLog.i(
                        TAG,
                        "startFrame frameIndex $frameIndex, factIndex $playFrameIndex, renderFinish $isReady"
                    )
                    yMatListener?.onRender(playFrameIndex)
                    if (isReady) {
                        if (playFrameIndex > playerInfo.duration * playerInfo.frameRate) {
                            if (isLoop) {
                                render?.clearRender()
                                for (decoder in videoDecoderList) {
                                    decoder.releaseDecoder()  //先销毁播放解码器
                                }
                                shapeParseThread.handler?.postDelayed({  //销毁需要耗时，延后让动画不会出现卡顿感
                                    syner?.reset()
                                    playFrameIndex = 0
                                    for (decoder in videoDecoderList) {
                                        decoder.replay()  //重播
                                        yMatListener?.onRestart()
                                        YMLog.i(TAG, "onRestart")
                                    }
                                }, LOOP_DELAY_TIME)
                            } else {
                                clear()
                                yMatListener?.onComplete()
                                YMLog.i(TAG, "onComplete")
                            }
                            return@readyFrame
                        }
                        //让继续解码器继续抽帧准备
                        for (decoder in videoDecoderList) {
                            if (playFrameIndex in decoder.decodeInfo.inFrame..decoder.decodeInfo.outFrame) {
                                decoder.setAwaitSync(false, playFrameIndex, false)
                            }
                        }

                        if (!useTGFX) {
                            //使用cpu绘制需要一直刷新的shape图形
                            for (shapeInfo in shapeInfoMap.entries) {
                                if (playFrameIndex in shapeInfo.value.inFrame..shapeInfo.value.outFrame
                                    && shapeInfo.value.isNeedReRender(playFrameIndex)
                                ) {
                                    shapeParseThread.handler?.post {
                                        drawShapeBitmap(shapeInfo.key, playFrameIndex)
                                        if (bitmapSourceMap.size > 0 && bitmapSourceMap.containsKey(
                                                shapeInfo.key
                                            )
                                        ) {
                                            updateImageTextureInfo(
                                                shapeInfo.key,
                                                playFrameIndex,
                                                bitmapSourceMap[shapeInfo.key]!!
                                            )
                                        }
                                    }
                                }
                            }
                        }
                        playFrameIndex++
                    }
                }
            }
        })
        //等待视频同步装填
        videoLatch?.await()
        YMLog.i(TAG, "all decode ready")
        //初始化ymat层级
        render?.initLayer { initSuccess ->
            if (initSuccess) {
                //启动定时刷新
                syner?.startTimer()
            } else {
                YMLog.e(TAG, "initLayer fail")
            }
        }

    }

    fun stop() {
        if (isRunning) {
            clear()
        }
    }

    fun pause() {
        syner?.pause()
    }

    fun resume() {
        syner?.resume()
    }

    fun isRunning(): Boolean {
        return isRunning
    }

    fun clear() {
        YMLog.i(TAG, "player clear")
        if (isRunning) {
            for (decoder in videoDecoderList) {
                decoder.pause()
            }
            audioDecoder?.pause()
            syner?.cancelTimer()
            isRunning = false
        }
        for (decoder in videoDecoderList) {
            decoder.destroy()
        }
        audioDecoder?.destroy()
        videoDecoderList.clear()
        videoInfoList.clear()
        for (source in videoSourceMap.values) {
            source.close()
        }
        videoSourceMap.clear()
        for (bitmap in bitmapSourceMap.values) {
            bitmap.recycle()
        }
        bitmapSourceMap.clear()
        playFrameIndex = 0
        render?.releaseRender()
        render?.destroyLayer {
            YMLog.i(TAG, "render destroyLayer finish")
        }
        shapeInfoMap.clear()
        decoderInitLatch = null
//        shapeParseThread.handler?.removeCallbacksAndMessages(null)
//        shapeParseThread.thread = YMatThread.quitSafely(shapeParseThread.thread)
//        shapeParseThread.handler = null
    }

    fun release() {
        shapeParseThread.handler?.post {
            YMLog.i(TAG, "player release")
            if (isRunning) {
                for (decoder in videoDecoderList) {
                    decoder.pause()
                }
                audioDecoder?.pause()
                //同步器关闭
                syner?.cancelTimer()
                isRunning = false
            }
            //创建同步等待
            val releaseLatch = CountDownLatch(videoDecoderList.size + 1)
            //释放视频解码器
            for (decoder in videoDecoderList) {
                decoder.destroy(releaseLatch)
            }
            videoDecoderList.clear()
            audioDecoder?.destroy()
            videoInfoList.clear()
            //释放视频
            for (source in videoSourceMap.values) {
                source.close()
            }
            videoSourceMap.clear()
            //释放图片
            for (bitmap in bitmapSourceMap.values) {
                bitmap.recycle()
            }
            bitmapSourceMap.clear()
            playFrameIndex = 0
            //释放纹理
            render?.releaseRender()
            //移除全部层级内容
            render?.destroyLayer { }
            //释放渲染器
            render?.destroyRender {
                YMLog.i(TAG, "render destroyRender finish")
                releaseLatch.countDown()
            }
            //释放处理线程
            shapeParseThread.handler?.removeCallbacksAndMessages(null)
            shapeParseThread.thread = YMatThread.quitSafely(shapeParseThread.thread)
            shapeParseThread.handler = null
            decoderInitLatch = null
            //需要等待全部解码器和渲染器释放后再回掉onDestroy
            releaseLatch.await()
            yMatListener?.onDestroy()
            yMatListener = null
            yMatVideoListener = null
        }
    }

    @Keep
    fun setPlayerInfo(width: Int, height: Int, duration: Int, frameRate: Int) {
        YMLog.i(
            TAG,
            "playId ${playerInfo.playerId}, width $width, height: $height, duration $duration, frameRate $frameRate"
        )
        playerInfo.width = width
        playerInfo.height = height
        playerInfo.frameRate = frameRate
        playerInfo.duration = duration
        render?.playerInfo = playerInfo
    }

    @Keep
    fun setVideoInfo(id: Int, vectorId: Int, inFrame: Int, outFrame: Int) {
//        if (id == 253 || id == 88 || id == 79 || id == 220) {
//            return
//        }
        YMLog.i(TAG, "setVideoInfo $id, vectorId $vectorId, inFrame $inFrame, outFrame: $outFrame")
        val info = VideoDecodeInfo()
        info.id = id
        info.vectorId = vectorId
        info.inFrame = inFrame
        info.outFrame = outFrame
        videoInfoList.add(info)
    }

    @Keep
    fun setText2Image(id: Int, texDocAttr: TexDocAttr) {
        if (useTGFX) return
        YMLog.i(TAG, "setTextImage $id textDoc:$texDocAttr")
        if (texDocAttr.text.isEmpty() || texDocAttr.with == 0 || texDocAttr.height == 0) {
            return
        }
        val paint = Paint().apply {
            this.textSize = texDocAttr.fontSize.toFloat()
            this.color = BaseShapeLayer.argb(
                1.0f,
                texDocAttr.textColor?.getOrNull(0) ?: 1f,
                texDocAttr.textColor?.getOrNull(1) ?: 1f,
                texDocAttr.textColor?.getOrNull(2) ?: 1f,
            )
            this.isAntiAlias = true
            this.isFakeBoldText = texDocAttr.fauxBold
            if (texDocAttr.fauxItalic) {
                this.textSkewX = -0.25f
            }
        }
        val text = if (keyInfoMap.containsKey(id)) {  //替换文字资源
            keyInfoMap[id]!!.path
        } else {
            texDocAttr.text
        }
        if (text.isEmpty()) {
            return
        }
        var canvas = canvasPool.acquire() ?: Canvas()
        val saved = canvas.saveLayer(null, paint)
        var bitmap: Bitmap
        val textList = text.split("\r".toRegex())
        val fontMetrics = paint.fontMetrics
        val wordHeight = fontMetrics.descent - fontMetrics.ascent
        val largestTxt = findLongestString(textList)
        val lineSpace = (texDocAttr.lineSpacing - texDocAttr.fontSize).let {
            if (it >= 0)
                it
            else
                0.0f
        }

        if (texDocAttr.orientation == 0) { //横向
            if (isWrapContent) {
                texDocAttr.with =
                    (paint.measureText(largestTxt) + if (texDocAttr.fauxItalic) texDocAttr.fontSize * 0.25 else 0.0).toInt()
                texDocAttr.height =
                    (wordHeight * textList.size + lineSpace * (textList.size - 1)).toInt()
            }
            bitmap =
                Bitmap.createBitmap(texDocAttr.with, texDocAttr.height, Bitmap.Config.ARGB_8888)
            canvas.setBitmap(bitmap)
            var y = -fontMetrics.ascent
            textList.forEach {
                val lineWidth = paint.measureText(it)
                val x = if (texDocAttr.textAligment == 0) {
                    0.0f
                } else if (texDocAttr.textAligment == 1) {
                    (texDocAttr.with - lineWidth) / 2
                } else if (texDocAttr.textAligment == 2) {
                    texDocAttr.with - lineWidth
                } else {
                    0.0f
                }
                canvas.drawText(it, x, y, paint)
                y += (wordHeight + lineSpace)
            }

        } else {  //纵向
            val lineWith = paint.measureText(text[0].toString()).toInt()
            if (isWrapContent) {
                texDocAttr.with =
                    lineWith * textList.size + (lineSpace * (textList.size - 1) +
                            if (texDocAttr.fauxItalic) texDocAttr.fontSize * 0.25 else 0.0).toInt()
                texDocAttr.height = (wordHeight * largestTxt.length).toInt()
            }
            bitmap =
                Bitmap.createBitmap(
                    texDocAttr.with,
                    texDocAttr.height,
                    Bitmap.Config.ARGB_8888
                )
            canvas.setBitmap(bitmap)
            var x = 0f
            textList.reversed().forEach { line ->
                val lineHeight = wordHeight * line.length
                var y = if (texDocAttr.textAligment == 0) {
                    -fontMetrics.ascent
                } else if (texDocAttr.textAligment == 1) {
                    (texDocAttr.height - lineHeight) / 2 - fontMetrics.ascent
                } else if (texDocAttr.textAligment == 2) {
                    texDocAttr.height - lineHeight - fontMetrics.ascent
                } else {
                    -fontMetrics.ascent
                }
                line.forEach {
                    canvas.drawText(it.toString(), x, y, paint)
                    y += wordHeight
                }
                x += (lineWith + lineSpace)
            }
        }
        if (!isWrapContent) {
            canvas.translate(texDocAttr.with / 2.0f, texDocAttr.height / 2.0f)
        }
        canvas.restoreToCount(saved)
        bitmapSourceMap[id] = bitmap
        canvasPool.release(canvas)
    }

    private fun findLongestString(strings: List<String>): String {
        var longestString = strings[0] // 假设第一个字符串是最长的
        for (string in strings) {
            if (string.length > longestString.length) {
                longestString = string
            }
        }
        return longestString
    }

    private val layerHashMap = ConcurrentHashMap<String, BaseShapeLayer>()

    @Keep
    @Synchronized
    fun shapeBitmap(
        id: Int,
        content: String,
        width: Int,
        height: Int,
        inFrame: Int,
        outFrame: Int
    ) {
        //使用tgfx会在gpu绘制
        if (useTGFX) return
        if (content.isEmpty() || width == 0 || height == 0) {
            return
        }

//        YMLog.i(TAG, "shapeBitmap content:$content")
        shapeParseThread.handler?.post {
            YMLog.i(TAG, "shapeBitmap content:$content")
            val shapeLayerInfoList = JsonParser.parseJsonList(
                content,
                ShapeLayerInfo::class.java
            )
            YMLog.i(TAG, "shapeBitmap elementsInfo:$shapeLayerInfoList")
            if (shapeLayerInfoList.isNullOrEmpty()) {
                return@post
            }
            //记录shape的数据
            shapeInfoMap[id] = ShapeInfoCache().apply {
                this.width = width
                this.height = height
                this.inFrame = inFrame
                this.outFrame = outFrame
                this.shapeList = shapeLayerInfoList
            }
            drawShapeBitmap(id, 0)
        }
    }

    /**
     * 绘制shape图片
     */
    fun drawShapeBitmap(id: Int, frame: Int) {
        YMLog.i(TAG, "drawShapeBitmap $id $frame")
        val shapeInfoCache = shapeInfoMap[id] ?: return
        val width = shapeInfoCache.width
        val height = shapeInfoCache.height
        if (width <= 0 || height <= 0) {
            return
        }
        //canvas缓冲池
        var canvas = canvasPool.acquire() ?: Canvas()
        var bitmap = bitmapSourceMap[id]
        if (bitmap != null && !bitmap.isRecycled) {
            canvas.setBitmap(bitmap)
            canvas.drawRect(Rect(0, 0, width, height), erasePaint)
        } else {
            bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            canvas.setBitmap(bitmap)
            bitmapSourceMap[id] = bitmap
        }
        shapeInfoCache.shapeList?.forEach {
            var layer = layerHashMap[it.type]
            if (layer == null) {
                layer = when (it.type) {
                    ElementsInfo.SHAPE_RECT -> {
                        RectLayer()
                    }

                    ElementsInfo.SHAPE_ELLIPSE -> {
                        EllipseLayer()
                    }

                    ElementsInfo.SHAPE_PATH -> {
                        PathLayer()
                    }

                    else -> {
                        null
                    }
                }
                if (layer != null) {
                    layerHashMap[it.type] = layer
                }
            }
            //只有path的时候会使用到
            layer?.width = width.toFloat()
            layer?.height = height.toFloat()
            try {
                it.elements?.let { element ->
                    layer?.draw(canvas, element, frame)
                }
            } catch (e: Exception) {
                YMLog.e(TAG, e.toString())
            }
        }
        canvasPool.release(canvas)
    }

    fun replay() {
        YMatJniUtils.setYMatConfig(playerInfo.playerId, json, this)
        shapeParseThread.handler?.post {
            createDecoder()
        }
    }

    fun setKeys(keyInfo: KeyInfo) {
        keyInfoMap[keyInfo.id] = keyInfo
    }

    fun setLoop(isLoop: Boolean) {
        this.isLoop = isLoop
    }

    fun setParams(fillMode: Int) {
        render?.setParams(fillMode)
    }
}