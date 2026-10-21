#pragma once
#include "qishou.h"

class Player : public Qishou{
public:
    Player(int row,int col,int color,string name):Qishou(row,col,color,name){}
    Action turn(const vector<vector<int>>&) const override;
    bool isThinking()const override{
        return 1;
    }
};