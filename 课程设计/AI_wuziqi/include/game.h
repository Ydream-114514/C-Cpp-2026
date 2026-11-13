#pragma once
#include "common.h"
#include "player.h"
#include "ai_search.h"
#include "ai_nn.h"

enum class Player_mode{
    AI_eazy,
    AI_hard,
    AI_nn,
    Player
};


inline PlayerConfig _player(BOARD_SIZE,BOARD_SIZE,-1,"Player");
inline AI_searchConfig _AI_eazy(BOARD_SIZE,BOARD_SIZE,-1,"AI",4,2,10,8,0);
inline AI_searchConfig _AI_hard(BOARD_SIZE,BOARD_SIZE,-1,"AI",5,2,10,8,6);
inline AI_NNConfig _AI_nn(BOARD_SIZE,BOARD_SIZE,-1,"AI","model.txt");

class GameState{
public:
    vector<vector<int>> board;
    int turn;
    int color[2];
    int score[2]={0,0};
    int lastr,lastc;
    int moveCount;
    int gameOver;
    shared_ptr<Qishou> p[2]={nullptr,nullptr};
    Player_mode pp[2];
    stack<pair<int,int>> st;
private:
    void initGame();
    void ExitGame();
    void Handle_result(Action &result);
public:
    GameState(Player_mode p1,Player_mode p2){
        pp[0]=p1;
        pp[1]=p2;
        initGame();
    }
    ~GameState(){ExitGame();}
    void run();
};

bool checkWin(const vector<vector<int>>&,int,int,int);