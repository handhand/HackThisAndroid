package com.handhandlab.handyAndroidHackThis.jni

import android.util.Log
import java.io.BufferedReader
import java.io.IOException
import java.io.InputStreamReader
import java.io.PrintWriter
import java.net.ServerSocket

/**
 * Jni interface for SimpleRasp
 * jniCallback will be called by Jni
 */
class RaspInterface(private val jniCallback: JniCallback) {

    init {
        start()
    }

    companion object {
        init {
            System.loadLibrary("SimpleRasp")
        }

        const val PORT = 12345
        const val RESPONSE_OK = "ok"
        const val TAG = "haha RaspInterface"
    }

    external fun entryPoint(): String

    /**
     * Start the protection
     */
    external fun startRuntimeApplicationSelfProtection(jniCallback: JniCallback)

    /**
     * Start listen for the socket, the result of the lib modification will be sent by socket
     */
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
