//
// Created by Cangwang on 2024/4/3.
//
#pragma once
#include <string>

using namespace std;
namespace ymat {
    class ImgSource {
    public:
        string srcPath;
        string desPath;
        string path;
    };

    class VideoSource {
    public:
        string srcPath;
        string content;
        string name;
    };
}