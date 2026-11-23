#pragma once
#include "ai_search.h"

typedef double Sc;

class AI_MCTS:public AI_search{
    int N=10000;
public:
    AI_MCTS(AI_searchConfig *x):AI_search(x){}
    pair<int,int> MCTS(Board&)const;
    Action computeMove(Board)const;
};