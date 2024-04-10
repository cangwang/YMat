//
// Created by Cangwang on 2024/4/8.
//

#include "ymatcontroller.h"

YMatController::YMatController() {

}

YMatController::~YMatController() {

}

int YMatController::addYMat(const char *json) {
    auto player = shared_ptr<YMatPlayer>();
    player->parse(json);
    playerMap.insert(make_pair(id, player));
}
