package com.example.wzjopencv;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.util.TypedValue;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.wzjopencv.databinding.ActivityMainBinding;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {



    private ActivityMainBinding binding;

    private Bitmap bitmap;
    private FaceDetection faceDetection;
    private File mCascadeFile;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        ImageView imageView = binding.faceImage;

        bitmap = BitmapFactory.decodeResource(getResources(),R.mipmap.psu);
        imageView.setImageBitmap(bitmap);

        copyCascadeFile();

        faceDetection = new FaceDetection();
        faceDetection.loadCascade(mCascadeFile.getAbsolutePath());

        tv.setOnClickListener(v -> {
            //识别人脸，保存人脸
//            faceDetection.faceDetectionSaveInfo(bitmap);
//            imageView.setImageBitmap(bitmap);

            //逆世界
//            bitmap = NDKBitmapUtils.againstWorld(bitmap);
            //浮雕
//            bitmap = NDKBitmapUtils.anaglyph(bitmap);

            //马赛克
//            bitmap = NDKBitmapUtils.mosaic(bitmap);

            //高斯模糊
//            bitmap = NDKBitmapUtils.groundGlass(bitmap);

            //油画效果
//            bitmap = NDKBitmapUtils.oilPainting(bitmap);


            //灰度处理
            bitmap = NDKBitmapUtils.garyOptimize(bitmap);

            imageView.setImageBitmap(bitmap);
        });


//        readStream();
    }

    private void readStream() {
        TypedValue typedValue = new TypedValue();
        @SuppressLint("ResourceType") InputStream is = getResources().openRawResource(R.mipmap.psu,typedValue);

        try {
            int first = is.read();
            int second = is.read();
            Log.e("TAG", "readStream: first = " + first + ";  second = " + second);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void copyCascadeFile() {
        try {
            // load cascade file from application resources
            InputStream is = getResources().openRawResource(R.raw.lbpcascade_frontalface);
            File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
            mCascadeFile = new File(cascadeDir, "lbpcascade_frontalface.xml");
            if(mCascadeFile.exists()) return;
            FileOutputStream os = new FileOutputStream(mCascadeFile);

            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }
            is.close();
            os.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


}