#pragma once
#include "common.h"
#include "ai.h"

enum class PlayerType{Human,AI,AI_random};

struct GameConfig{
    PlayerType p[2]={PlayerType::AI,PlayerType::Human};
    shared_ptr<AIConfig> c[2];
};

struct GameState{
    vector<vector<int>> board;
    int turn;
    int color[2];
    int score[2]={0,0};
    int lastr,lastc;
    int moveCount;
    int gameOver;
    GameConfig cfg;
    shared_ptr<Qishou> p[2]={nullptr,nullptr};
    stack<pair<int,int>> st;
};

bool checkWin(const vector<vector<int>>&,int,int,int);
void initGame(GameState &,GameConfig &);
void ExitGame(GameState &);
void Handle_result(GameState &,Action &);