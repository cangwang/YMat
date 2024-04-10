//
// Created by Cangwang on 2024/4/8.
//

#include "ymatplayer.h"



ymat::YMatPlayer::YMatPlayer() {

}

ymat::YMatPlayer::~YMatPlayer() {

}

void ymat::YMatPlayer::parse(const char *json) {
    if (json) {
        auto yMatInfo = YMatConfig::parse(json);
    }
}
