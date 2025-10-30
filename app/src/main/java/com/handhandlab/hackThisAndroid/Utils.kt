package com.handhandlab.hackThisAndroid

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Color
import android.graphics.Picture
import java.io.File
import java.nio.ByteBuffer





object Utils {
    fun getFileFromAssets(context: Context, fileName: String): File = File(context.cacheDir, fileName)
        .also {
            if (!it.exists()) {
                it.outputStream().use { cache ->
                    context.assets.open(fileName).use { inputStream ->
                        inputStream.copyTo(cache)
                    }
                }
            }
        }

    const val MNIST_WIDTH = 28
    const val MNIST_HEIGHT = 28

    fun createBitmapFromPicture(picture: Picture): Bitmap {
        // convert picture to bitmap
        val bitmap = Bitmap.createBitmap(
            picture.width,
            picture.height,
            Bitmap.Config.ARGB_8888
        )

        val canvas = android.graphics.Canvas(bitmap)
        canvas.drawColor(android.graphics.Color.WHITE)
        canvas.drawPicture(picture)

        // scale bitmap to align with MNIST input size
        return Bitmap.createScaledBitmap(bitmap, MNIST_WIDTH, MNIST_HEIGHT, false)
    }

    fun convertRGBBitmapToNormGrayscaleArray(bitmap: Bitmap): FloatArray {
        // convert to grayscale
        val pixels = FloatArray(bitmap.width * bitmap.height)
        // loop ourself to make sure the order is aligned with the model
        for (y in 0 until bitmap.height) {
            for (x in 0 until bitmap.width) {
                val pixel = bitmap.getPixel(x, y)
                // I don't know why, it looks like it should minus by 255...
                val red =  255 - Color.red(pixel)
                val green = 255 - Color.green(pixel)
                val blue = 255 - Color.blue(pixel)
                val gray = (red * 0.3f + green * 0.59f + blue * 0.11f) / 255.0f
                pixels[y * bitmap.width + x] = gray
            }
        }
        return pixels
    }

    fun floatArrayToByteArray(values: FloatArray): ByteArray {
        val buffer = ByteBuffer.allocate(4 * values.size)
        for (value in values) {
            buffer.putFloat(value)
        }
        return buffer.array()
    }
}
