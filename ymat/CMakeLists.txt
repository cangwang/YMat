# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("ymat")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/jniLibs/${ANDROID_ABI})
include_directories(${CMAKE_SOURCE_DIR} src/main/cpp)

add_library(ymat
        SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        src/main/cpp/ymat.cpp
        src/main/cpp/egl/eglcore.cpp
        src/main/cpp/player/ymatcontroller.cpp
        src/main/cpp/player/ymatplayer.cpp
        src/main/cpp/player/rendercontroller.cpp
        src/main/cpp/layer/layer.cpp
        src/main/cpp/layer/imagelayer.cpp
        src/main/cpp/layer/videolayer.cpp
        src/main/cpp/layer/textlayer.cpp
        src/main/cpp/layer/vectorlayer.cpp
        src/main/cpp/layer/shaderlayer.cpp
        src/main/cpp/utils/shaderutil.cpp
        src/main/cpp/utils/texcoordsutil.cpp
        src/main/cpp/utils/vertexutil.cpp
        src/main/cpp/utils/glfloatarray.cpp
        src/main/cpp/bean/YMatConfig.cpp
        src/main/cpp/render/mp4render.cpp
        src/main/cpp/render/imagerender.cpp
        src/main/cpp/render/evarender.cpp
        src/main/cpp/render/vectorrender.cpp
        )

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
add_library(parson
        STATIC
        src/main/cpp/utils/parson.c)

target_link_libraries(
        ymat
        # List libraries link to the target library
        android
        GLESv3
        EGL
        jnigraphics
        parson
        log)