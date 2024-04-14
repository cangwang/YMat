package com.yy.ymat.bean

import android.media.MediaDataSource

class YMatMediaDataSource(private val mediaData: ByteArray): MediaDataSource() {

    override fun close() {

    }

    override fun readAt(position: Long, buffer: ByteArray, offset: Int, size: Int): Int {
        if (position < 0 || position >= this.size) {
            return -1 // 表示读取位置超出范围
        }

        val start = position.toInt()
        val end = Math.min(start + size, mediaData.size)
        val bytesToRead = end - start
        System.arraycopy(mediaData, start, buffer, offset, bytesToRead)
        return bytesToRead
    }

    override fun getSize(): Long {
        return mediaData.size.toLong()
    }

}