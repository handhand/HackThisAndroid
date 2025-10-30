package com.handhandlab.hackThisAndroid.model

import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_HIGH_RISK
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_INFO
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_SECURE
import com.handhandlab.hackThisAndroid.jni.JniCallback.Companion.STATUS_WARNING

data class DetectionData(
//    val source: String,
    val type: String,
    val message: String = "Checking result not received yet.",
    val status: Int = getStatusByMessage(message)
)

fun getStatusByMessage(message: String): Int {
    return when {
        message.contains("tampered", ignoreCase = true) ||
        message.contains("detected", ignoreCase = true) -> STATUS_HIGH_RISK
        message.contains("pass", ignoreCase = true) -> STATUS_SECURE
        message.contains("warning", ignoreCase = true) -> STATUS_WARNING
        else -> STATUS_INFO
    }
}