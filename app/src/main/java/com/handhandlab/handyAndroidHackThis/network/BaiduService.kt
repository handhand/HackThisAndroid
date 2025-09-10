package com.handhandlab.handyAndroidHackThis.network

import okhttp3.ResponseBody
import retrofit2.http.GET


interface BaiduService {

    @GET("/")
    suspend fun query(): ResponseBody

}