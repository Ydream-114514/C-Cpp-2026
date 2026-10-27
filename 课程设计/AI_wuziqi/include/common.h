#pragma once
#include<bits/stdc++.h>
#include "raylib.h"
using namespace std;
const int BOARD_SIZE=19;
const int EMPTY=0;
const int Chess_Color_BLACK=1;
const int Chess_Color_WHITE=2;
const int CELL_SIZE=40;        // 每个格子 40 像素
const int MARGIN_UP=80;
const int MARGIN_DOWN=100;
const int MARGIN_LEFT=40;
const int MARGIN_RIGHT=40;     // 棋盘边缘留白
const int WIN_W=CELL_SIZE*(BOARD_SIZE-1)+MARGIN_LEFT+MARGIN_RIGHT;
const int WIN_H=CELL_SIZE*(BOARD_SIZE-1)+MARGIN_UP+MARGIN_DOWN;
extern mt19937 rng;
int randInt(int,int);
bool rand_float(double);
typedef double Score_type;

enum ActionType{
    ACT_MOVE,
    ACT_RESIGN,
    ACT_RESTART,
    ACT_JUMP
};
struct Action{
    ActionType type;
    int row;
    int col;
};
