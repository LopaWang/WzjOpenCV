#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <android/log.h>
#include "cv_helper.h"

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

//使用命名空间
using namespace cv;


extern "C"{
    JNIEXPORT jint JNICALL
    Java_com_example_wzjopencv_FaceDetection_faceDetectionSaveInfo(JNIEnv *env, jobject thiz,
                                                                   jobject bitmap);
    void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap);

    void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap);

    JNIEXPORT void JNICALL
    Java_com_example_wzjopencv_FaceDetection_loadCascade(JNIEnv *env, jobject thiz, jstring file_path);
}


void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap) {
    //Mat里面有个type；CV_8UC4正好对应Bitmap里面的ARGB_8888,CV_8UC2正好对应Bitmap里面的RGB_565

    //1.获取bitmap信息
    AndroidBitmapInfo bitmapInfo;
    void* pixels;
    AndroidBitmap_getInfo(env,bitmap,&bitmapInfo);
    //2.锁定bitmap画布
    AndroidBitmap_lockPixels(env,bitmap,&pixels);

    if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888){
        //创建临时的Mat
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC4,pixels);
        //把数据重新赋值给mat
        if(mat.type() == CV_8UC4){
            mat.copyTo(temp);
        } else if(mat.type() == CV_8UC2){
            cvtColor(mat,temp,COLOR_BGR5652BGRA);
        } else if(mat.type() == CV_8UC1){
            cvtColor(mat,temp,COLOR_GRAY2BGRA);
        }

    } else if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565){
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC2,pixels);
        if(mat.type() == CV_8UC4){
            cvtColor(mat,temp,COLOR_BGRA2BGR565);
        } else if(mat.type() == CV_8UC2){
            mat.copyTo(temp);
        } else if(mat.type() == CV_8UC1){
            cvtColor(mat,temp,COLOR_GRAY2BGR565);
        }
    }

    AndroidBitmap_unlockPixels(env,bitmap);
}

void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap) {
    //Mat里面有个type；CV_8UC4正好对应Bitmap里面的ARGB_8888,CV_8UC2正好对应Bitmap里面的RGB_565

    //1.获取bitmap信息
    AndroidBitmapInfo bitmapInfo;
    void* pixels;
    AndroidBitmap_getInfo(env,bitmap,&bitmapInfo);
    //2.锁定bitmap画布
    AndroidBitmap_lockPixels(env,bitmap,&pixels);

    mat.create(bitmapInfo.height,bitmapInfo.width,CV_8UC4);

    if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888){
        //创建临时的Mat
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC4,pixels);
        //把数据重新赋值给mat
        temp.copyTo(mat);

    } else if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565){
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC2,pixels);
        //mat是CV_8UC2 -> CV_8UC4
        cvtColor(temp,mat,COLOR_BGR5652BGRA);
    }

    AndroidBitmap_unlockPixels(env,bitmap);
}

CascadeClassifier cascadeClassifier;
extern "C"
JNIEXPORT void JNICALL
Java_com_example_wzjopencv_FaceDetection_loadCascade(JNIEnv *env, jobject thiz, jstring file_path) {
    const char *filePath = env->GetStringUTFChars(file_path, 0);
    cascadeClassifier.load(filePath);
    LOGE("加载分类器文件成功%s",filePath);
    env->ReleaseStringUTFChars(file_path, filePath);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_wzjopencv_FaceDetection_faceDetectionSaveInfo(JNIEnv *env, jobject thiz,
                                                               jobject bitmap) {
//    // 检测人脸
    Mat mat;
    bitmap2Mat(env,mat,bitmap);

    //处理灰度 opencv处理灰度图，提高效率，一般所有的操作都会对其进行灰度处理

    Mat gray_mat;
    cvtColor(mat,gray_mat,COLOR_BGRA2GRAY);

    //再次处理，直方均衡补偿
    Mat equalize_mat;
    equalizeHist(gray_mat,equalize_mat);

    // 识别人脸，当然我们可以直接用 彩色图去做,识别人脸要加载人脸分类器文件
    std::vector<Rect> faces;
    cascadeClassifier.detectMultiScale(equalize_mat,faces);
    LOGE("人脸个数：%d",faces.size());
    if(faces.size() == 1){
        Rect faceRect = faces[0];

        // 在人脸部分花个图
        rectangle(mat,faceRect,Scalar(255,155,155),8);
        // 把 mat 我们又放到 bitmap 里面
        mat2Bitmap(env,mat,bitmap);
        // 保存人脸信息 Mat , 图片 jpg
        Mat face_info_mat(equalize_mat,faceRect);
        // 保存 face_info_mat
    } else if(faces.size() > 1){
        for (int i = 0; i < faces.size(); i++) {
            Rect faceRect = faces[i];

            // 在人脸部分花个图
            rectangle(mat,faceRect,Scalar(255,155,155),8);
            // 把 mat 我们又放到 bitmap 里面
            mat2Bitmap(env,mat,bitmap);
            // 保存人脸信息 Mat , 图片 jpg
            Mat face_info_mat(equalize_mat,faceRect);
        }
    }

    // 保存人脸信息


    return 0;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_wzjopencv_NDKBitmapUtils_againstWorld(JNIEnv *env, jclass clazz, jobject bitmap) {
    // bitmap -> mat
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    Mat res(src.size(), src.type());
    // 获取图片宽高
    int src_w = src.cols;
    int src_h = src.rows;
    int mid_h = src_h >> 1;
    int a_h = mid_h >> 1;
    // 处理下半部分
    for (int rows = 0; rows < mid_h; ++rows) {
        for (int cols = 0; cols < src_w; ++cols) {
            // 4 rgba  , 判断 type()
            res.at<Vec4b>(rows + mid_h, cols) = src.at<Vec4b>(rows + a_h, cols);
        }
    }

    // 处理下半部分
    for (int rows = 0; rows < mid_h; ++rows) {
        for (int cols = 0; cols < src_w; ++cols) {
            // 4 rgba  , 判断 type()
            res.at<Vec4b>(rows, cols) = src.at<Vec4b>(src_h - a_h - rows, cols);
        }
    }

    cv_helper::mat2bitmap(env, res, bitmap);

    return bitmap;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_wzjopencv_NDKBitmapUtils_anaglyph(JNIEnv *env, jclass clazz, jobject bitmap) {
    // 有立体感，突出了轮廓信息，opencv  卷积
    // [1,0]
    // [0,-1]
    // bitmap -> mat
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    Mat res(src.size(), src.type());
    // 获取图片宽高
    int src_w = src.cols;
    int src_h = src.rows;

    for (int rows = 0; rows < src_h - 1; ++rows) {
        for (int cols = 0; cols < src_w - 1; ++cols) {
            Vec4b pixels_p = src.at<Vec4b>(rows, cols);
            Vec4b pixels_n = src.at<Vec4b>(rows + 1, cols + 1);
            // bgra
            res.at<Vec4b>(rows, cols)[0] = saturate_cast<uchar>(pixels_p[0] - pixels_n[0] + 128);
            res.at<Vec4b>(rows, cols)[1] = saturate_cast<uchar>(pixels_p[1] - pixels_n[1] + 128);
            res.at<Vec4b>(rows, cols)[2] = saturate_cast<uchar>(pixels_p[2] - pixels_n[2] + 128);
        }
    }

    cv_helper::mat2bitmap(env, res, bitmap);

    return bitmap;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_wzjopencv_NDKBitmapUtils_mosaic(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    // 获取图片宽高
    int src_w = src.cols;
    int src_h = src.rows;

    // 省略人脸识别
    int rows_s = src_h >> 2;
    int rows_e = src_h * 3 / 4;
    int cols_s = src_w >> 2;
    int cols_e = src_w * 3 / 4;
    int size = 10;

    for (int rows = rows_s; rows < rows_e; rows += size) {
        for (int cols = cols_s; cols < cols_e; cols += size) {
            int pixels = src.at<int>(rows, cols);
            for (int m_rows = 1; m_rows < size; ++m_rows) {
                for (int m_cols = 1; m_cols < size; ++m_cols) {
                    src.at<int>(rows + m_rows, cols + m_cols) = pixels;
                }
            }
        }
    }

    cv_helper::mat2bitmap(env, src, bitmap);
    return bitmap;
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_wzjopencv_NDKBitmapUtils_groundGlass(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    // 高斯模糊，毛玻璃 （对某个区域取随机像素）
    int src_w = src.cols;
    int src_h = src.rows;
    int size = 8;
    // 知识不是用来背的 20% ，用来唤醒大家的
    RNG rng(time(NULL));
    for (int rows = 0; rows < src_h - size; ++rows) {
        for (int cols = 0; cols < src_w - size; ++cols) {
            int random = rng.uniform(0, 8);
            src.at<int>(rows, cols) = src.at<int>(rows + random, cols + random);
        }
    }
    cv_helper::mat2bitmap(env, src, bitmap);
    return bitmap;
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_wzjopencv_NDKBitmapUtils_oilPainting(JNIEnv *env, jclass clazz, jobject bitmap) {
    // 油画基于直方统计
    // 1. 每个点需要分成 n*n 小块
    // 2. 统计灰度等级
    // 3. 选择灰度等级中最多的值
    // 4. 找到所有的像素取平均值
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    Mat gary;
    cvtColor(src, gary, COLOR_BGRA2GRAY);
    Mat res(src.size(), src.type());

    int src_w = src.cols;
    int src_h = src.rows;
    int size = 8;
    // 知识不是用来背的 20% ，用来唤醒大家的
    for (int rows = 0; rows < src_h - size; ++rows) {
        for (int cols = 0; cols < src_w - size; ++cols) {
            int g[8] = {0}, b_g[8] = {0}, g_g[8] = {0}, r_g[8] = {0};// 255/8
            // 这个位置  64 循环 -> 1 个像素点 ， 高斯模糊 ，想想怎么优化
            for (int o_rows = 0; o_rows < size; ++o_rows) {
                for (int o_cols = 0; o_cols < size; ++o_cols) {
                    uchar gery = gary.at<uchar>(rows + o_rows, cols + o_cols);
                    uchar index = gery / (254 / 7); // 254*8/254
                    g[index] += 1;
                    // 等级的像素值之和
                    b_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[0];
                    g_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[1];
                    r_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[2];
                }
            }
            // 最大的角标找出来
            int max_index = 0;
            int max = g[0];
            for (int i = 1; i < size; ++i) {
                if (g[max_index] < g[i]) {
                    max_index = i;
                    max = g[i];
                }
            }
            // 会超过 255 ， 会不会超过，超过了会怎样 （头条面试）  截取掉最高位
            res.at<Vec4b>(rows, cols)[0] = b_g[max_index] / max;
            res.at<Vec4b>(rows, cols)[1] = g_g[max_index] / max;
            res.at<Vec4b>(rows, cols)[2] = r_g[max_index] / max;
        }
    }

    cv_helper::mat2bitmap(env, res, bitmap);

    return bitmap;
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_wzjopencv_NDKBitmapUtils_garyOptimize(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    // 高斯模糊，毛玻璃 （对某个区域取随机像素）
    int src_w = src.cols;
    int src_h = src.rows;
    // 知识不是用来背的 20% ，用来唤醒大家的
    for (int rows = 0; rows < src_h; ++rows) {
        for (int cols = 0; cols < src_w; ++cols) {
            Vec4b pixels = src.at<Vec4b>(rows, cols);
            uchar b = pixels[0];
            uchar g = pixels[1];
            uchar r = pixels[2];
            // 好几种算法求灰度
            // uchar gery = (b + g + r) / 3;
            // 位移 > +- >  */  int > float
            // uchar gery = 0.299 * r + 0.587 * g + 0.1114 * b;
            uchar gery = (r + (g << 1) + b) >> 2; // 0.4  sobel  |x|+|y|
            src.at<Vec4b>(rows, cols)[0] = gery;
            src.at<Vec4b>(rows, cols)[1] = gery;
            src.at<Vec4b>(rows, cols)[2] = gery;
        }
    }
    return bitmap;
}