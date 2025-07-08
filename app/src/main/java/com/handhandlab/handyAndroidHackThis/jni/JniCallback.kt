package com.handhandlab.handyAndroidHackThis.jni

interface JniCallback {
    fun onJniCallback(code: Int, message: String);

    companion object {
        const val EMULATOR = 1
        const val ROOT = 2
        const val FRIDA = 3
        const val LIB_PATCH = 4
    }
}