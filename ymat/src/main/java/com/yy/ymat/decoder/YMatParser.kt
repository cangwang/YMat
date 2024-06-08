package com.yy.ymat.decoder

import android.content.Context
import android.graphics.BitmapFactory
import android.os.SystemClock
import android.util.Log
import com.yy.ymat.bean.YMatDataHeader
import com.yy.ymat.bean.YMatDataNode
import com.yy.ymat.utils.FileParseUtil
import java.io.ByteArrayInputStream
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.net.HttpURLConnection
import java.net.URL
import java.security.MessageDigest
import java.util.concurrent.LinkedBlockingQueue
import java.util.concurrent.ThreadPoolExecutor
import java.util.concurrent.TimeUnit
import java.util.concurrent.atomic.AtomicInteger

class YMatParser(context: Context?) {

    companion object {
        const val TAG = "YMatParser"

        private val threadNum = AtomicInteger(0)
        private val taskQueueLocal by lazy { LinkedBlockingQueue<Runnable>() }
        private var externalThreadPoolExecutor: ThreadPoolExecutor? = null
        internal val threadPoolExecutor by lazy {
            if (externalThreadPoolExecutor != null) {
                externalThreadPoolExecutor!!
            } else {
                ThreadPoolExecutor(
                    1, 8, 20L,
                    TimeUnit.SECONDS, taskQueueLocal
                ) { r ->
                    Thread(r, "YMatParser-Thread-Local-${threadNum.getAndIncrement()}")
                }
            }
        }

        private const val FILE_MARK_LENGTH = 4L //文件标志固定4个字节

        private const val HEAD_TYPE_LENGTH = 1 //头部 type长度
        private const val HEAD_EXTEND_LENGTH = 2 //头部 extend 长度
        private const val HEAD_BODY_LENGTH = 4 //body长度


        const val ERROR_DOWNLOAD_FAILED = 2001
        const val ERROR_OPEN_FILE_FAILED = 2002
        const val ERROR_PARSE_FILE_FAILED = 2003

        fun getError(errorCode: Int): String = when (errorCode) {
            ERROR_DOWNLOAD_FAILED -> "文件下载失败"
            ERROR_OPEN_FILE_FAILED -> "打开文件失败"
            ERROR_PARSE_FILE_FAILED -> "文件解析失败"
            else -> "未知错误"
        }
    }

    private var mContext = context?.applicationContext
    private var externalCacheDir = ""

    fun setExternalCacheDir(dir: String) {
        this.externalCacheDir = dir
    }

    interface ParseCompletionCallback {
        fun onComplete(data: List<YMatDataNode>)
        fun onError(errorCode: Int)
    }

    fun decodeFromAssets(fileName: String, callback: ParseCompletionCallback?) {
        mContext ?: return
        threadPoolExecutor.execute {
            try {
                mContext?.assets?.open(fileName)?.let {
                    val dataNodeList = decodeFromInputStream(it, true)
                    Log.d(TAG, "decodeFromAssets dataNodeList:$dataNodeList")
                    if (dataNodeList.isNullOrEmpty()) {
                        callback?.onError(ERROR_PARSE_FILE_FAILED)
                    } else {
                        callback?.onComplete(dataNodeList)
                    }
                } ?: kotlin.run {
                    callback?.onError(ERROR_OPEN_FILE_FAILED)
                }
            } catch (e: IOException) {
                callback?.onError(ERROR_OPEN_FILE_FAILED)
            }
        }
    }

    fun decodeFromFile(path: String?, callback: ParseCompletionCallback?) {
        if (path.isNullOrEmpty()) {
            return
        }
        val file = File(path)
        if (!file.exists()) {
            return
        }
        threadPoolExecutor.execute {
            val fileInputStream = FileInputStream(file)
            val dataNodeList = decodeFromInputStream(fileInputStream, true)
            Log.d(TAG, "decodeFromFile dataNodeList:$dataNodeList")
            if (dataNodeList.isNullOrEmpty()) {
                callback?.onError(ERROR_PARSE_FILE_FAILED)
            } else {
                callback?.onComplete(dataNodeList)
            }
        }
    }

    fun decodeFromUrl(url: String?, callback: ParseCompletionCallback?, isCache: Boolean = true) {
        if (url.isNullOrEmpty()) {
            return
        }
        Log.d(TAG, "decodeFromUrl $url")
        val filePath = getFilePath(url)
        val file = File(filePath)
        if (file.exists()) {
            Log.d(TAG, "decodeFromUrl $filePath")
            decodeFromFile(filePath, callback)
        } else {
            threadPoolExecutor.execute {
                downloadFile(url, isCache, callback)
            }
        }
    }

    /**
     * 解析文件操作
     */
    private fun decodeFromInputStream(
        inputStream: InputStream,
        isFinalClose: Boolean = true
    ): List<YMatDataNode>? {
        if (inputStream.skip(FILE_MARK_LENGTH) != FILE_MARK_LENGTH) {
            return null
        }
        val start = SystemClock.elapsedRealtime()
        val dataNodeList = mutableListOf<YMatDataNode>()
        try {
            while (true) {
                val typeBytes = FileParseUtil.fetchData(HEAD_TYPE_LENGTH, inputStream) ?: break
                val header = YMatDataHeader()
                val extendMark = FileParseUtil.getHighestBit(typeBytes[0])
                header.type = FileParseUtil.getBits1to7(typeBytes[0])
                Log.d(TAG, "decodeFromInputStream extendMark:$extendMark")
                if (extendMark == 1) {
                    val extendBytes = FileParseUtil.fetchData(HEAD_EXTEND_LENGTH, inputStream)
                    header.extend =
                        FileParseUtil.byteToInt(extendBytes?.get(0), extendBytes?.get(1))
                }
                val bodyLenBytes = FileParseUtil.fetchData(HEAD_BODY_LENGTH, inputStream)
                header.length = FileParseUtil.byteArrayToInt(bodyLenBytes)
                Log.d(TAG, "decodeFromInputStream header:${header}")
                val yMatDataNode = YMatDataNode().apply {
                    this.head = header
                }
                if (header.length > 0) {
                    yMatDataNode.body = FileParseUtil.fetchData(header.length, inputStream)
                    yMatDataNode.transformBody()
                }
                dataNodeList.add(yMatDataNode)
            }
        } catch (e: Throwable) {
            Log.e(TAG, "decodeFromInputStream ERROR:$e")
            return null
        } finally {
            if (isFinalClose) {
                inputStream.close()
            }
        }
        Log.d(TAG, "parse cost time:${SystemClock.elapsedRealtime() - start}")
        return dataNodeList
    }

    private fun buildFileName(str: String): String {
        val messageDigest = MessageDigest.getInstance("MD5")
        messageDigest.update(str.toByteArray(charset("UTF-8")))
        val digest = messageDigest.digest()
        var sb = ""
        for (b in digest) {
            sb += String.format("%02x", b)
        }
        return sb
    }

    private fun getFilePath(url: String): String {
        return if (externalCacheDir.isEmpty()) {
            "${mContext?.cacheDir?.absolutePath}/ymatRes/${buildFileName(url)}"
        } else {
            "$externalCacheDir/ymatRes/${buildFileName(url)}"
        }
    }

    private fun downloadFile(
        fileUrl: String,
        isCache: Boolean,
        callback: ParseCompletionCallback?
    ): Boolean {
        var inputStream: InputStream? = null
        var outputStream: OutputStream? = null
        var connection: HttpURLConnection? = null
        var byteArrayOutputStream: ByteArrayOutputStream? = null
        try {
            Log.d(TAG, "downloadFile start")
            val url = URL(fileUrl)
            connection = (url.openConnection() as? HttpURLConnection)?.apply {
                connectTimeout = 20 * 1000
                requestMethod = "GET"
                useCaches = true
                connect()
            }
            Log.d(TAG, "downloadFile  ${connection?.responseCode}")
            if (connection?.responseCode != HttpURLConnection.HTTP_OK) {
                callback?.onError(ERROR_DOWNLOAD_FAILED)
                return false
            }
            inputStream = connection.inputStream
            if (isCache) {
                try {
                    val destinationFilePath = getFilePath(fileUrl)
                    Log.d(TAG, "destinationFilePath $destinationFilePath")
                    val file = File(destinationFilePath)
                    file.parent?.let { File(it).mkdirs() }
                    outputStream = FileOutputStream(file)
                    val buffer = ByteArray(1024)
                    while (true) {
                        val length = inputStream.read(buffer)
                        if (length <= 0) {
                            break
                        }
                        outputStream.write(buffer, 0, length)
                    }
                    decodeFromFile(destinationFilePath, callback)
                    Log.d(TAG, "downloadFile save finish")
                } catch (e: Exception) {
                    callback?.onError(ERROR_DOWNLOAD_FAILED)
                    Log.e(TAG, "save file error:$e")
                }
            } else {
                byteArrayOutputStream = ByteArrayOutputStream()
                val buffer = ByteArray(2024)
                while (true) {
                    val count = inputStream.read(buffer, 0, 2024)
                    if (count <= 0) {
                        break
                    }
                    byteArrayOutputStream.write(buffer, 0, count)
                }
                val dataNodeList = decodeFromInputStream(
                    ByteArrayInputStream(byteArrayOutputStream.toByteArray()),
                    true
                )
                if (dataNodeList.isNullOrEmpty()) {
                    callback?.onError(ERROR_PARSE_FILE_FAILED)
                } else {
                    callback?.onComplete(dataNodeList)
                }
            }
            return true
        } catch (e: Exception) {
            Log.e(TAG, "downloadFile error:$e")
            callback?.onError(ERROR_DOWNLOAD_FAILED)
            return false
        } finally {
            try {
                byteArrayOutputStream?.close()
                inputStream?.close()
                outputStream?.close()
                connection?.disconnect()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }


}