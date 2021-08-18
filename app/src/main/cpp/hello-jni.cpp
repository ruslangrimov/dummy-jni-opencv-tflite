/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <opencv2/opencv.hpp>
#include <string.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <math.h>
#include <chrono>

#define log_print(...) __android_log_print(ANDROID_LOG_DEBUG, __VA_ARGS__)

std::string test_speed(AAssetManager *mgr);

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   hello-jni/app/src/main/java/com/example/hellojni/HelloJni.java
 */
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_hellojni_HelloJni_stringFromJNI( JNIEnv* env, jobject thiz, jobject assetManager)
{
#if defined(__arm__)
    #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
#define ABI "x86"
#elif defined(__x86_64__)
#define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#define ABI "mips64"
#elif defined(__mips__)
#define ABI "mips"
#elif defined(__aarch64__)
#define ABI "arm64-v8a"
#else
#define ABI "unknown"
#endif
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    auto test_out = test_speed(mgr);
    std::stringstream ss;
    ss << test_out << "ABI: " << ABI;
    return env->NewStringUTF(ss.str().c_str());
}

std::string test_speed(AAssetManager *mgr)
{
    std::vector<cv::Mat> frames;
    for (auto i = 150; i < 450; i ++) {
        std::stringstream fname;
        fname << "frames/frame_" << i << ".jpg";
        AAsset *asset = AAssetManager_open(mgr, fname.str().c_str(), AASSET_MODE_BUFFER);

        auto fsize = AAsset_getLength64(asset);

        cv::Mat raw_data(1, fsize, CV_8UC1, (void*)AAsset_getBuffer(asset));
        cv::Mat frame = cv::imdecode(raw_data, cv::IMREAD_COLOR);
        if (frame.data == NULL) {
            log_print("Dummy", "error on reading: %s", fname.str().c_str());
        } else {
            // cv::resize(frame, frame, {0, 0}, 1.0/2.0, 1.0/2.0);
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
            frames.push_back(frame);
        }
        AAsset_close(asset);
    }

    std::stringstream result;
    result << "Version: 1" << std::endl;

    for (int d : std::vector<int>({6, 4})) {
        for (int nthreads : std::vector<int>({1, 4})) {
            result << "d: " << d << ", threads: " << nthreads << std::endl;

            cv::setNumThreads(nthreads);

            /*
            here your code for initialization
            */

            auto start = std::chrono::steady_clock::now();
            int f = -1;
            int act_cnt = 0;
            for (auto frame: frames) {
                f ++;

                /*
                Here your code to process frame
                */
            }
            auto end = std::chrono::steady_clock::now();
            int time = int(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

            result << "Frames: " << frames.size() << ", Time: " << time <<  ", FPS: " << ((double)frames.size()/(double)time*1000.0) << ", act cnt: " << act_cnt;
            result << std::endl;
        }
    }

    return result.str();
}
