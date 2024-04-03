package com.duowan.ymatrix

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.yy.ymatrix.utils.YMatrixUtils

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        YMatrixUtils.getExternalTexture()
    }
}