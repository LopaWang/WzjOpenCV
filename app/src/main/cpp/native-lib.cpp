#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <android/log.h>

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