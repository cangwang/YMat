package com.yy.ymat.bean

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.nfc.Tag
import android.util.Log
import com.yy.ymat.utils.FileParseUtil

class YMatDataNode {
    var head: YMatDataHeader? = null
    var body: ByteArray? = null
    var bodyTxt: String? = null

    var bitmap: Bitmap? = null

    fun transformBody() {
        if (body == null || body!!.isEmpty()) {
            return
        }
        if (head?.type == 0 || head?.type == 4 || head?.type == 5) {
            FileParseUtil.zlibInflate(body!!)?.let {
                bodyTxt = String(it)
            }
            Log.d("YMatDataNode", "transformBody$bodyTxt")
        } else if (head?.type == 1) {
            bitmap = BitmapFactory.decodeByteArray(body!!, 0, body!!.size)
        }
    }

    override fun toString(): String {
        return "YMatDataNode(head=$head, body=${body?.size}, bodyTxt=$bodyTxt ,bitmap=$bitmap)"
    }
}