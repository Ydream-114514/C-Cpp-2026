#pragma once
#include "common.h"
#include "qishou.h"

class Player : public Qishou{
public:
    Player(QishouConfig x):Qishou(x){}
    Action turn(const vector<vector<int>>&) const override;
    bool isThinking()const override{return 1;}
    bool isplayer()const override{return 1;}
};
