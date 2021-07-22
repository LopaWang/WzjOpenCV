package com.example.wzjopencv;

import android.graphics.Bitmap;

/**
 * NDK 层处理一些 Bitmap 效果
 */
public class NDKBitmapUtils {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * 实现逆世界效果
     *
     * @param bitmap 原图像
     * @return 逆世界
     */
    public static final native Bitmap againstWorld(Bitmap bitmap);


    /**
     * 浮雕效果
     *
     * @param bitmap 原图像
     * @return 浮雕效果图像
     */
    public static final native Bitmap anaglyph(Bitmap bitmap);

    /**
     * 实现马赛克效果
     * @param bitmap 原图像
     * @return 马赛克图片
     */
    public static final native Bitmap mosaic(Bitmap bitmap);

    /**
     * 实现图片毛玻璃效果
     *
     * @param bitmap 原图像
     * @return 毛玻璃效果
     */
    public static final native Bitmap groundGlass(Bitmap bitmap);

    /**
     * 实现图像油画效果
     *
     * @param bitmap
     * @return 油画效果图像
     */
    public static final native Bitmap oilPainting(Bitmap bitmap);

    /**
     * 灰度图像处理效果
     * @param bitmap 原图像
     * @return 优化后的灰度图像
     */
    public static final native Bitmap garyOptimize (Bitmap bitmap);

}
