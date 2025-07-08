package com.handhandlab.handyAndroidHackThis

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults.topAppBarColors
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import com.handhandlab.handyAndroidHackThis.theme.ExperimentsTheme

class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
//        Log.e("haha", jniInterface.entryPoint())
        setContent {
            ExperimentsTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    RootView()
                }
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun RootView(
    viewModel: HackThisViewModel = viewModel()
) {
    Scaffold(
        topBar = {
            TopAppBar(
                colors = topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.primaryContainer,
                    titleContentColor = MaterialTheme.colorScheme.primary,
                ),
                title = {
                    Text("Hack this")
                }
            )
        }
    ) { paddingValues ->
        Column(modifier = Modifier.padding(paddingValues)) {
            Text(modifier = Modifier
                .fillMaxWidth()
                .background(Color.LightGray), text = "SimpleRASP")
            Text(viewModel.basicMsg.value)
            Text(viewModel.fridaMsg.value)
            Text(viewModel.libPatchMsg.value)
            Text(viewModel.emulatorMsg.value)
            Text(viewModel.rootMsg.value)

            Text(modifier = Modifier
                .padding(top = 12.dp)
                .fillMaxWidth()
                .background(Color.LightGray), text = "AndroidSecurityGuard")
            Text(viewModel.asgBasicMsg.value)
            Text(viewModel.asgFridaMsg.value)
            Text(viewModel.asgLibPatchMsg.value)
            Text(viewModel.asgEmulatorMsg.value)
            Text(viewModel.asgRootMsg.value)
            Text(viewModel.asgDebuggerMsg.value)
            Button(modifier = Modifier.padding(all = 12.dp)
                .fillMaxWidth(), onClick = {viewModel.doSomeThing()}) {
                Text(
                    text = "TODO: make cert pinning https request")
            }
        }
    }

}
