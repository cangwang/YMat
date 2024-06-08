package com.duowan.ymatrix

import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import android.widget.RelativeLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.yy.ymat.player.IYMatListener
import com.yy.ymat.utils.IYMLog
import com.yy.ymat.utils.YMLog
import com.yy.ymat.view.YMatView
import java.io.BufferedReader
import java.io.InputStreamReader
import java.util.concurrent.LinkedBlockingQueue

class RestartActivity : AppCompatActivity() {

    private val URL = "http://lxcode.bs2cdn.yy.com/e466b43f-6f7d-441b-a4d7-18d5c627e8fe.ymat"
    private val queue = LinkedBlockingQueue<String>()
    private var ymat: YMatView? = null
    private val TAG = "RestartActivity"
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_restart)

        window.setFlags(WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED, WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED)

        val containter = findViewById<RelativeLayout>(R.id.ymat_container)


        findViewById<TextView>(R.id.play).setOnClickListener {
//            ymat.loadAndPlayFromAssets("color2.ymat")
            if (ymat == null) {
                addYmat(containter)
            }
            for (i in 0.. 5) {
                queue.add("color2.ymat")
            }

            if (ymat != null && !ymat?.isRunning()!!) {
                ymat?.loadAndPlayFromAssets(queue.poll())
            }
//            ymat.loadAndPlayFromUrl("http://lxcode.bs2cdn.yy.com/bb3b4576-a33b-4f58-8458-f2cadd2bf3e6.ymat")
        }

        findViewById<TextView>(R.id.stop).setOnClickListener {
            ymat?.stop()
            queue.clear()
        }

        findViewById<TextView>(R.id.replay).setOnClickListener {
            ymat?.replay()
        }

        findViewById<TextView>(R.id.pause).setOnClickListener {
            ymat?.pause()
        }

        findViewById<TextView>(R.id.resume).setOnClickListener {
            ymat?.resume()
        }


//        ymat.postDelayed({
//            ymat.loadAndPlayFromAssets("color2.ymat")
////        ymat.loadAndPlayFromUrl("http://lxcode.bs2cdn.yy.com/bb3b4576-a33b-4f58-8458-f2cadd2bf3e6.ymat")
////            ymat.setLoop(true)
////        YMatParser(this).decodeFromUrl(URL, null, true)
//        },300)

    }

    fun addYmat(container: RelativeLayout) {
        if (ymat == null) {
            ymat = YMatView(this)
            val lp = RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT)
            container.addView(ymat, lp)
        }
        ymat?.setLog(object : IYMLog {
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
        ymat?.setYmatListener(object : IYMatListener {
            override fun onComplete() {
                YMLog.i(TAG, "onComplete")
//                if (queue.isNotEmpty()) {
//                    ymat?.loadAndPlayFromAssets(queue.poll())
//                } else {
//                    ymat?.destroy()
//                }
                if (queue.isNotEmpty()) {
//                    addYmat(container)
                    ymat?.loadAndPlayFromAssets(queue.poll())
                }
            }

            override fun onDestroy() {
                super.onDestroy()
                container.removeView(ymat)
                YMLog.i(TAG, "onDestroy")
                ymat = null
                if (queue.isNotEmpty()) {
                    addYmat(container)
                    ymat?.loadAndPlayFromAssets(queue.poll())
                }
            }
        })
    }

    override fun onDestroy() {
        super.onDestroy()
        queue.clear()
        ymat?.destroy()
    }

    fun readJsonFromAsset(context: Context, fileName: String): String {
        return context.assets.open(fileName).use { stream ->
            BufferedReader(InputStreamReader(stream)).use { reader ->
                reader.readText()
            }
        }
    }
}