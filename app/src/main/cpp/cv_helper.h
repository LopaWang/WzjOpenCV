//
// Created by 曾辉 on 2018/12/20.
//

#ifndef STUDY_CV_HELPER_H
#define STUDY_CV_HELPER_H

#include <jni.h>
#include "opencv2/opencv.hpp"
#include <android/bitmap.h>

class cv_helper {
public:
    static int bitmap2mat(JNIEnv *env, jobject &bitmap, cv::Mat &dst);

    static int mat2bitmap(JNIEnv *env, cv::Mat &src, jobject &bitmap);

    static jobject createBitmap(JNIEnv *env, jint width, jint height, int type);
};


#endif //STUDY_CV_HELPER_H
