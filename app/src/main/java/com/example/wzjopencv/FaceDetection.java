package com.example.wzjopencv;

import android.graphics.Bitmap;

public class FaceDetection {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }


    /**
     * 检测人脸并保存人脸信息
     * @param bitmap
     */
    public native int faceDetectionSaveInfo(Bitmap bitmap);

    /**
     * 加载人脸识别的分类器文件
     * @param filePath
     */
    public native void loadCascade(String filePath);
}
