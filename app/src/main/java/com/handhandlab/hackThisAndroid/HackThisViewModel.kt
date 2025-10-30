package com.handhandlab.hackThisAndroid

import android.util.Log
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.handhandlab.hackThisAndroid.jni.JniCallback
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.DEBUGGER
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.EMULATOR
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.FRIDA
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.LIB_PATCH
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.ROOT
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_SECURE
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_WARNING
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.codeToString
import com.handhandlab.hackThisAndroid.jni.RaspInterface
import com.handhandlab.hackThisAndroid.model.DetectionData
import com.handhandlab.hackThisAndroid.network.BaiduService
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import okhttp3.OkHttpClient
import retrofit2.Retrofit

class HackThisViewModel: ViewModel() {

    private val raspInterface: RaspInterface

    init {
        // init AndroidSecurityGuard
        AndroidSecurityGuard.jniCallback = object : JniCallback {
            override fun onJniCallback(code: Int, message: String) {
                viewModelScope.launch(Dispatchers.Main) {
                    asgResult.value = _asgResultList.addDetectionData(
                        DetectionData(
                            type = codeToString(code),
                            message = message
                        )
                    )
                }
            }
        }

        // init SimpleRASP
        val raspCallback = object : JniCallback {
            override fun onJniCallback(code: Int, message: String) {
                viewModelScope.launch(Dispatchers.Main) {
                    Log.d("haha", "onJniCallback: $code $message")
                    simpleRaspResult.value = _simpleRaspResultList.addDetectionData(
                        DetectionData(
                            type = codeToString(code),
                            message = message
                        )
                    )
                }
            }
        }

        raspInterface = RaspInterface(raspCallback)
        // TODO: move to application
        raspInterface.startRuntimeApplicationSelfProtection(raspCallback)
        initApiService()
    }

    val apiDataMsg = mutableStateOf("")
    val loading = mutableStateOf(false)

    private lateinit var apiService: BaiduService
    private var currentApiRequest: Job? = null

    private val _simpleRaspResultList = mutableListOf(
        DetectionData(codeToString(EMULATOR),  status = STATUS_WARNING),
        DetectionData(codeToString(ROOT),  status = STATUS_WARNING),
        DetectionData(codeToString(FRIDA),  status = STATUS_WARNING)
    )
    val simpleRaspResult = mutableStateOf(_simpleRaspResultList.toList())

    // AndroidSecurityGuard won't send message if no suspicious is found, so default is PASS
    private val _asgResultList = mutableListOf(
        DetectionData(codeToString(ROOT),  "Nothing reported", status = STATUS_SECURE),
        DetectionData(codeToString(FRIDA),  "Nothing reported", status = STATUS_SECURE),
        DetectionData(codeToString(LIB_PATCH), "Nothing reported", status = STATUS_SECURE),
        DetectionData(codeToString(DEBUGGER), "Nothing reported", status = STATUS_SECURE)
    )
    val asgResult = mutableStateOf(emptyList<DetectionData>())

    private fun MutableList<DetectionData>.addDetectionData(
        detectionData: DetectionData
    ): List<DetectionData> {
        removeIf { it.type == detectionData.type }
        add(detectionData)
        sortBy {
            it.type
        }
        return toList()
    }

    private fun initApiService() {
        apiService = Retrofit.Builder()
            .baseUrl("https://www.baidu.com")
            .client(OkHttpClient())
            .build()
            .create(BaiduService::class.java)
    }

    fun doNetworkRequest() {
        apiDataMsg.value = ""
        currentApiRequest?.cancel()
        loading.value = true
        currentApiRequest = viewModelScope.launch {
            try {
                val responseString = apiService.query()
                apiDataMsg.value = responseString.string()
            } catch (e: Exception) {
                apiDataMsg.value = e.message ?: "Unknown network error"
            }
            loading.value = false
        }
    }
}
