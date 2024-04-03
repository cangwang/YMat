//
// Created by Cangwang on 2024/4/3.
//
#pragma once
#include <string>

using namespace std;
namespace ymat {
    class Source {
    public:
        string srcPath;
        string desPath;
        string path;
    };

    class ImgSource {
    public:
        shared_ptr<list<Source>> imgs;
    };

    class VSource {
    public:
        string srcPath;
        string content;
        string name;
    };

    class VideoSource {
    public:
        shared_ptr<list<VSource>> videos;
    };
}