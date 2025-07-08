package com.handhandlab.handyAndroidHackThis

import com.handhandlab.handyAndroidHackThis.jni.JniCallback

class AndroidSecurityGuard {

    companion object {

        var jniCallback: JniCallback? = null

        init {
            System.loadLibrary("NativeGuard")
        }

        @JvmStatic
        fun addLog(code: Int, message: String) {
            jniCallback?.onJniCallback(code, message)
        }

    }
}