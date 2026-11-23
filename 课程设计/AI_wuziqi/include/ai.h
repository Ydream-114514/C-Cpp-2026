#pragma once
#include "common.h"
#include "qishou.h"
#include "Zobristhash.h"


class AI : public Qishou,public enable_shared_from_this<AI>{
private:
    mutable future<Action> future_;
    mutable bool thinking_ = false;
public:
    AI(QishouConfig *x):Qishou(x){}
    Action turn(const Board &)const override;
    bool isThinking()const override{return thinking_;}
    bool isplayer()const override{return 0;}
protected:
    virtual Action computeMove(Board)const=0;
};

