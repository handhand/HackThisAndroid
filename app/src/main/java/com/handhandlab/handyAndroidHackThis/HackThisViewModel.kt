package com.handhandlab.handyAndroidHackThis

import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.handhandlab.handyAndroidHackThis.jni.JniCallback
import com.handhandlab.handyAndroidHackThis.jni.JniCallback.Companion.DEBUGGER
import com.handhandlab.handyAndroidHackThis.jni.JniCallback.Companion.EMULATOR
import com.handhandlab.handyAndroidHackThis.jni.JniCallback.Companion.FRIDA
import com.handhandlab.handyAndroidHackThis.jni.JniCallback.Companion.LIB_PATCH
import com.handhandlab.handyAndroidHackThis.jni.JniCallback.Companion.ROOT
import com.handhandlab.handyAndroidHackThis.jni.RaspInterface
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class HackThisViewModel: ViewModel() {

    // for SimpleRasp, if no found or not found message received, then it's suspicious
    val basicMsg = mutableStateOf("N/A")
    val rootMsg = mutableStateOf("Root - N/A")
    val emulatorMsg = mutableStateOf("Emulator - suspicious")
    val fridaMsg = mutableStateOf("Frida - suspicious")
    val libPatchMsg = mutableStateOf("Lib modification - PASS")

    // AndroidSecurityGuard won't send message if no suspicious is found, so default is N/A
    val asgBasicMsg = mutableStateOf("N/A")
    val asgRootMsg = mutableStateOf("Root detection - PASS")
    val asgEmulatorMsg = mutableStateOf("Emulator detection - N/A")
    val asgFridaMsg = mutableStateOf("Frida detection - PASS")
    val asgLibPatchMsg = mutableStateOf("Lib modification detection - PASS")
    val asgDebuggerMsg = mutableStateOf("Debugger detection - PASS")

    private val raspCallback: JniCallback = object : JniCallback {
        override fun onJniCallback(code: Int, message: String) {
            viewModelScope.launch(Dispatchers.Main) {
                when (code) {
                    EMULATOR -> {
                        emulatorMsg.value = message
                    }
                    ROOT -> {
                        rootMsg.value = message
                    }
                    FRIDA -> {
                        fridaMsg.value = message
                    }
                    LIB_PATCH -> {
                        libPatchMsg.value = message
                    }
                    else -> {
                        basicMsg.value = message
                    }
                }
            }
        }
    }

    private val asgCallback: JniCallback = object : JniCallback {
        override fun onJniCallback(code: Int, message: String) {
            viewModelScope.launch(Dispatchers.Main) {
                when (code) {
                    EMULATOR -> {
                        asgEmulatorMsg.value = message
                    }
                    ROOT -> {
                        asgRootMsg.value = message
                    }
                    FRIDA -> {
                        asgFridaMsg.value = message
                    }
                    LIB_PATCH -> {
                        asgLibPatchMsg.value = message
                    }
                    DEBUGGER -> {
                        asgDebuggerMsg.value = message
                    }
                    else -> {
                        asgBasicMsg.value = message
                    }
                }
            }
        }
    }

    private val raspInterface = RaspInterface(raspCallback)

    // TODO: move to application
    init {
        AndroidSecurityGuard.jniCallback = asgCallback
        raspInterface.startRuntimeApplicationSelfProtection(raspCallback)
    }

    fun doSomeThing() {
        raspInterface.entryPoint()
    }
}
