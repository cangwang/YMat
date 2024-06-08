package com.duowan.ymatrix

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        findViewById<Button>(R.id.show).setOnClickListener {
            startActivity(Intent(this, ShowActivity::class.java))
        }

        findViewById<Button>(R.id.restart).setOnClickListener {
            startActivity(Intent(this, RestartActivity::class.java))
        }
    }
}