package com.handhandlab.handyAndroidHackThis.jni

interface JniCallback {
    fun onJniCallback(code: Int, message: String);

    companion object {
        const val EMULATOR = 1
        const val ROOT = 2
        const val FRIDA = 3
        const val LIB_PATCH = 4
        const val DEBUGGER = 5
        const val PTHREAD_CREATE = 6

        const val STATUS_SECURE = 0
        const val STATUS_WARNING = 1
        const val STATUS_HIGH_RISK = 2
        const val STATUS_INFO = 100

        fun codeToString(code: Int): String {
            return when (code) {
                EMULATOR -> "Emulator"
                ROOT -> "Root"
                FRIDA -> "Frida"
                LIB_PATCH -> "LibPatch"
                DEBUGGER -> "Debugger"
                PTHREAD_CREATE -> "pthread_create"
                else -> "Info"
            }
        }
    }
}