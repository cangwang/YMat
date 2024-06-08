# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.10.2)

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

# 是否加入TGFX打包
option(USE_TGFX "Enable or disable some feature" OFF)  # ON 打开 OFF关闭

if (USE_TGFX)
        add_definitions(-DUSE_TGFX)
        #tgfx 2d
        include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)
        add_library(tgfx STATIC IMPORTED)
        set_target_properties(tgfx PROPERTIES IMPORTED_LOCATION
                ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/tgfx.a)
        set(SOURCE_DIRS
                "src/main/cpp/egl"
                "src/main/cpp/player"
                "src/main/cpp/layer"
                "src/main/cpp/utils"
                "src/main/cpp/bean"
                "src/main/cpp/tgfx")
else()
        set(SOURCE_DIRS
                "src/main/cpp/egl"
                "src/main/cpp/player"
                "src/main/cpp/layer"
                "src/main/cpp/utils"
                "src/main/cpp/bean")
endif()

# 使用 foreach 循环来收集所有源文件
foreach(dir ${SOURCE_DIRS})
        file(GLOB_RECURSE SOURCES ${dir}/*.cpp)
        list(APPEND ALL_SOURCES ${SOURCES})
endforeach()

add_library(ymat SHARED src/main/cpp/ymat.cpp ${ALL_SOURCES})

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
add_library(parson
        STATIC
        src/main/cpp/utils/parson.c)

if(USE_TGFX)
        target_link_libraries(
                ymat
                # List libraries link to the target library
                tgfx
                android
                GLESv3
                EGL
                jnigraphics
                parson
                log)
else()
        target_link_libraries(
                ymat
                # List libraries link to the target library
                android
                GLESv3
                EGL
                jnigraphics
                parson
                log)
endif()
