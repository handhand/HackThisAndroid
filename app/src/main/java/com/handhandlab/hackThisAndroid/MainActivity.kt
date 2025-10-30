package com.handhandlab.hackThisAndroid

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Tab
import androidx.compose.material3.TabRow
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults.topAppBarColors
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.withStyle
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_HIGH_RISK
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_SECURE
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_WARNING
import com.handhandlab.hackThisAndroid.model.DetectionData
import com.handhandlab.hackThisAndroid.theme.ExperimentsTheme
import com.handhandlab.hackThisAndroid.theme.Purple80
import com.handhandlab.hackThisAndroid.theme.PurpleGrey80

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
                navigationIcon = {
                    Image(
                        modifier = Modifier
                            .size(32.dp),
                        painter = painterResource(id = R.mipmap.ic_droid),
                        contentDescription = null
                    )
                },
                colors = topAppBarColors(
                    containerColor = Purple80,
                    titleContentColor = PurpleGrey80,
                ),
                title = {
                    Text(
                        text = buildAnnotatedString {
                            append("Ha")
                            withStyle(style = SpanStyle(color = Color.Gray)) {
                                append("ck")
                            }
                            append("th")
                            withStyle(style = SpanStyle(color = Color.Gray)) {
                                append("is")
                            }
                            append("A")
                            withStyle(style = SpanStyle(color = Color.Gray)) {
                                append("ndroid")
                            }
                        }
                    )
                }
            )
        }
    ) { paddingValues ->
        Column(modifier = Modifier.padding(paddingValues)) {
            var tabIndex by remember { mutableIntStateOf(0) }
            val tabList = arrayOf("Simple RASP", "AndroidSecurityGuard", "HTTP Request")
            TabRow(
                modifier = Modifier.fillMaxWidth(),
                selectedTabIndex = tabIndex
            ) {
                tabList.forEachIndexed { index, title ->
                    Tab(
                        text = { Text(title) },
                        selected = tabIndex == index,
                        onClick = {
                            tabIndex = index
                        }
                    )
                }
            }
            when(tabIndex) {
                0 -> ListView(viewModel.simpleRaspResult.value)
                1 -> ListView(viewModel.asgResult.value)
                2 -> NetworkRequestView(viewModel)
            }

        }
    }

}

@Composable
fun ListView(detectionDataList: List<DetectionData>) {
    LazyColumn {
        itemsIndexed(detectionDataList) { index, detection ->
            // Your item UI here
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(start = 16.dp, top = if (index == 0) 16.dp else 8.dp, end = 16.dp, bottom = 8.dp),
                shape = RoundedCornerShape(4.dp),
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.surface
                ),
                elevation = CardDefaults.cardElevation(1.dp)
            ) {
                Row {
                    Image(
                        modifier = Modifier
                            .size(48.dp)
                            .align(Alignment.CenterVertically)
                            .padding(8.dp),
                        // Use a placeholder image from resources
                        painter = painterResource(
                                    id = when(detection.status) {
                                        STATUS_SECURE -> R.drawable.ic_tick
                                        STATUS_WARNING -> R.drawable.ic_warning
                                        STATUS_HIGH_RISK -> R.drawable.ic_high_error
                                        else -> R.drawable.ic_info
                                    }
                                ),
                        contentDescription = null
                    )
                    Column(modifier = Modifier.padding(16.dp)) {
                        Text(
                            text = detection.type,
                            style = MaterialTheme.typography.titleMedium
                        )
                        Text(
                            text = detection.message,
                            style = MaterialTheme.typography.bodyMedium
                        )
                    }
                }

            }
        }
    }
}

@Composable
fun NetworkRequestView(viewModel: HackThisViewModel) {
    Column(
        modifier = Modifier.padding(10.dp)
    ) {
        var text by remember { mutableStateOf("www.baidu.com") }
        Row(
            modifier = Modifier.border(
                width = 1.dp,
                color = Color.LightGray,
                shape = RoundedCornerShape(10.dp)
            ).padding(
                end = 8.dp
            )
        ) {
            TextField(
                value = text,
                onValueChange = { newText -> text = newText },
                label = { Text("https://") },
                placeholder = { Text("url") },
                modifier = Modifier.weight(1f).align(
                    Alignment.CenterVertically
                ),
                colors = TextFieldDefaults.colors(
                    cursorColor = Color.Black,
                    focusedContainerColor = Color.Transparent,
                    unfocusedContainerColor = Color.Transparent,
                    focusedIndicatorColor = Color.Transparent,
                    unfocusedIndicatorColor = Color.Transparent,
                ),
                singleLine = true,
            )
            Button(
                modifier = Modifier.align(
                    Alignment.CenterVertically
                ),
                onClick = {
                    viewModel.doNetworkRequest()
                }
            ) {
                Text(text = "Get")
            }
        }
        if (viewModel.loading.value) {
            CircularProgressIndicator(
                modifier = Modifier.padding(top = 128.dp)
                    .width(48.dp).align(Alignment.CenterHorizontally),
                color = MaterialTheme.colorScheme.secondary,
                trackColor = MaterialTheme.colorScheme.surfaceVariant,
            )
        }
        if (viewModel.apiDataMsg.value.isNotEmpty()) {
            val scrollState = rememberScrollState()
            Text(modifier = Modifier.padding(top = 16.dp), text = "Response:")
            Column(
                modifier = Modifier
                    .border(
                        width = 1.dp,
                        color = Color.LightGray,
                        shape = RoundedCornerShape(10.dp)
                    ).padding(8.dp)
                    .verticalScroll(scrollState)
                    .fillMaxWidth()
            ) {
                Text(viewModel.apiDataMsg.value)
            }
        }
    }
}
