package com.stelnet.demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import com.stelnet.demo.ui.main.MainScreen
import com.stelnet.demo.ui.main.MainScreenViewModel
import com.stelnet.demo.ui.theme.StelnetDemoTheme

class MainActivity : ComponentActivity() {
    private val viewModel: MainScreenViewModel by viewModels { MainScreenViewModel.factory() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            StelnetDemoTheme {
                MainScreen(viewModel)
            }
        }
    }
}
