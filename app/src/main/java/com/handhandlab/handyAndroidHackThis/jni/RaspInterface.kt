package com.handhandlab.handyAndroidHackThis.jni

import android.util.Log
import java.io.BufferedReader
import java.io.IOException
import java.io.InputStreamReader
import java.io.PrintWriter
import java.net.ServerSocket

class RaspInterface(private val jniCallback: JniCallback) {

    init {
        start()
    }

    companion object {
        init {
            System.loadLibrary("SimpleRasp")
//            System.loadLibrary("DetectFrida")
        }

        const val PORT = 12345
        const val RESPONSE_OK = "ok"
        const val TAG = "haha RaspInterface"
    }

    external fun entryPoint(): String

    external fun startRuntimeApplicationSelfProtection(jniCallback: JniCallback)

    private fun start() {
        Thread {
            listenForResult()
        }.start()
    }

    private fun listenForResult() {
        try {
            ServerSocket(PORT).use { serverSocket ->
                println("Server is listening on port $PORT")
                while (true) {
                    val socket = serverSocket.accept()
                    Log.d(TAG, "Client connected")

                    // Read data from client
                    val br = BufferedReader(InputStreamReader(socket.getInputStream()))
                    val message = br.readLine()
                    Log.d(TAG, "Received: $message")
                    jniCallback.onJniCallback(message.toInt(), "Library modification - DETECTED!")

                    // Send response to client
                    val out = PrintWriter(socket.getOutputStream(), true)
                    out.println(RESPONSE_OK);

                    socket.close()
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }
}
