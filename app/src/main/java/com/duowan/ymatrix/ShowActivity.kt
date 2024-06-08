package com.duowan.ymatrix

import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.yy.ymat.bean.KeyInfo
import com.yy.ymat.player.IYMatListener
import com.yy.ymat.utils.IYMLog
import com.yy.ymat.utils.YMLog
import com.yy.ymat.view.YMatView
import java.io.BufferedReader
import java.io.InputStreamReader
import java.util.concurrent.LinkedBlockingQueue
import com.duowan.ymatrix.R

class ShowActivity : AppCompatActivity() {

    private val URL = "http://lxcode.bs2cdn.yy.com/e466b43f-6f7d-441b-a4d7-18d5c627e8fe.ymat"
    private val queue = LinkedBlockingQueue<String>()
    private val TAG = "ShowActivity"
    private lateinit var ymat: YMatView
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_show)

        window.setFlags(WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED, WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED)
//        val json = readJsonFromAsset(this, "mock.json")
//        YMatJniUtils.setYMatConfig(json, null)
        ymat = findViewById<YMatView>(R.id.ymat)
        ymat.setLog(object : IYMLog {
            override fun i(tag: String, msg: String) {
                Log.i(tag, msg)
            }

            override fun d(tag: String, msg: String) {
                Log.d(tag, msg)
            }

            override fun e(tag: String, msg: String) {
                Log.e(tag, msg)
            }
        })

        findViewById<TextView>(R.id.play).setOnClickListener {
//            ymat.loadAndPlayFromAssets("color2.ymat")
            queue.add("pathdemo.ymat")
            if (!ymat.isRunning()) {
                ymat.loadAndPlayFromAssets(queue.poll())
            }

//            ymat.loadAndPlayFromUrl("http://lxcode.bs2cdn.yy.com/bb3b4576-a33b-4f58-8458-f2cadd2bf3e6.ymat")
        }

        findViewById<TextView>(R.id.stop).setOnClickListener {
            ymat.stop()
            queue.clear()
        }

        findViewById<TextView>(R.id.replay).setOnClickListener {
            ymat.replay()
        }

        findViewById<TextView>(R.id.pause).setOnClickListener {
            ymat.pause()
        }

        findViewById<TextView>(R.id.resume).setOnClickListener {
            ymat.resume()
        }

        ymat.setYmatListener(object : IYMatListener {
            override fun onComplete() {
                if (queue.isNotEmpty()) {
                    ymat.loadAndPlayFromAssets(queue.poll())
                }
            }

            override fun onStart() {
                super.onStart()
                YMLog.i(TAG, "onStart")
            }
        })
        ymat.setKeys(KeyInfo(3474, "image", R.drawable.ball_3.toString()))
        ymat.postDelayed({
            ymat.loadAndPlayFromAssets("pathdemo.ymat")
//        ymat.loadAndPlayFromUrl("http://lxcode.bs2cdn.yy.com/bb3b4576-a33b-4f58-8458-f2cadd2bf3e6.ymat")
//            ymat.setLoop(true)
//        YMatParser(this).decodeFromUrl(URL, null, true)
        },300)

    }

    override fun onDestroy() {
        super.onDestroy()
        ymat.destroy()
    }

    fun readJsonFromAsset(context: Context, fileName: String): String {
        return context.assets.open(fileName).use { stream ->
            BufferedReader(InputStreamReader(stream)).use { reader ->
                reader.readText()
            }
        }
    }
}