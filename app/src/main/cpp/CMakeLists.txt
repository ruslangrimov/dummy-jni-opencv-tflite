cmake_minimum_required(VERSION 3.4.1)

# project(hello-jn LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#OPENCV
include_directories(${OpenCV_DIR}/jni/include)
add_library(lib_opencv SHARED IMPORTED)
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_java4.so)

add_library(hello-jni SHARED hello-jni.cpp)

# Include libraries needed for hello-jni lib
target_link_libraries(
    hello-jni
    android
    log
    lib_opencv
)
