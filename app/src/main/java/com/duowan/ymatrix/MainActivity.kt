package com.duowan.ymatrix

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.yy.ymat.utils.YMatUtils
import java.io.BufferedReader
import java.io.InputStreamReader

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val json = readJsonFromAsset(this, "mock.json")
        YMatUtils.setYMatConfig(json)
    }

    fun readJsonFromAsset(context: Context, fileName: String): String {
        return context.assets.open(fileName).use { stream ->
            BufferedReader(InputStreamReader(stream)).use { reader ->
                reader.readText()
            }
        }
    }
}