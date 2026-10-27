#pragma once
#include "common.h"
#include "qishou.h"

struct GameState{
    vector<vector<int>> board;
    int turn;
    int color[2];
    int score[2]={0,0};
    int lastr,lastc;
    int moveCount;
    int gameOver;
    shared_ptr<Qishou> p[2]={nullptr,nullptr};
    stack<pair<int,int>> st;
};

bool checkWin(const vector<vector<int>>&,int,int,int);
void initGame(GameState &);
void ExitGame(GameState &);
void Handle_result(GameState &,Action &);