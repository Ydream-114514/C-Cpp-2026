#pragma once
#include "ai_search.h"

using AI_MCTSConfig=AI_searchConfig;

class AI_MCTS:public AI_search{
    int time_limit=2000;
public:
    AI_MCTS(AI_searchConfig *x):AI_search(x){}
    int Simulation(Board&,int,int)const;
    pair<int,int> MCTS(Board&)const;
    Action computeMove(Board)const;
};