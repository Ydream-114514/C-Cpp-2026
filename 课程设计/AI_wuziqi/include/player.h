#pragma once
#include "common.h"
#include "qishou.h"

struct PlayerConfig:QishouConfig{
    PlayerConfig(int row,int col,int color,string name):QishouConfig(row,col,color,name){}
};

class Player : public Qishou{
public:
    Player(PlayerConfig *x):Qishou(x){}
    Action turn(const vector<vector<int>>&) const override;
    bool isThinking()const override{return 1;}
    bool isplayer()const override{return 1;}
};
