package com.yy.ymat.utils

import android.util.Log
import java.io.ByteArrayOutputStream
import java.io.InputStream
import java.util.zip.Inflater
import kotlin.experimental.and

object FileParseUtil {

    const val TAG = "FileParseUtil"

    /**
     * 读取特定长度字节的数据
     */
    fun fetchData(length: Int, inputStream: InputStream): ByteArray? {
        var leftLength = length
        var dataBytes: ByteArray?
        try {
            dataBytes = ByteArray(length)
            while (leftLength > 0) {
                val readLength = inputStream.read(dataBytes, length - leftLength, length)
                if (readLength > 0) {
                    leftLength -= readLength
                } else {
                    return null
                }
            }
        } catch (e: Throwable) {
            return null
        }
        return dataBytes
    }

    /**
     * byte数组转int
     */
    fun byteArrayToInt(byteArray: ByteArray?): Int {
        if (byteArray?.size != 4) {
            return -1
        }
        return (byteArray[0].toInt() and 0xFF) or
                ((byteArray[1].toInt() and 0xFF) shl 8) or
                ((byteArray[2].toInt() and 0xFF) shl 16) or
                ((byteArray[3].toInt() and 0xFF) shl 24)
    }

    fun byteToInt(byte1: Byte?, byte2: Byte?): Int {
        val value1 = (byte1?.toInt() ?: 0) and 0xFF
        val value2 = (byte2?.toInt() ?: 0) and 0xFF
        return value2 shl 8 or value1
    }

    /**
     * 获取最高位
     */
    fun getHighestBit(b: Byte): Int {
        return (b.toInt() and 0x80) shr 7
    }

    /**
     * 获取后7位
     */
    fun getBits1to7(b: Byte): Int {
        return b.toInt() and 0x7F
    }

    /**
     * zlib解压缩
     */
    fun zlibInflate(compressedData: ByteArray): ByteArray? {
        var output: ByteArray?
        val inflater = Inflater()
        inflater.setInput(compressedData)

        val out = ByteArrayOutputStream(compressedData.size)
        try {
            val buf = ByteArray(1024)
            while (!inflater.finished()) {
                val i = inflater.inflate(buf)
                out.write(buf, 0, i)
            }
            output = out.toByteArray()
        } catch (e: Exception) {
            output = null
        } finally {
            try {
                out.close()
            } catch (e: Exception) {
            }
        }
        inflater.end()
        return output
    }

    fun printBits(b: Byte) {
        for (i in 7 downTo 0) {
            val bit = (b.toInt() shr i) and 0x01
            Log.d(TAG, "printBits bit:$bit $i")
        }
        Log.d(TAG, "printBits bit============================")
    }
}