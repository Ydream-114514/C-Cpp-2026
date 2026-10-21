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
const Score_type Win_score     =1e8;
const Score_type Full_Four     =1e6;
const Score_type Not_Full_Four =1e5;
const Score_type Full_Three    =1e4;
const Score_type Not_Full_Three=1e3;
const Score_type Full_Two      =1e2;
const Score_type Not_Full_Two  =10;

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
struct Button{
    string name;
    Rectangle pos;
    Color Back_ground;
    Color Back_ground_outline;
    Color text_color;
    int Fontsize;
};
enum Align{ 
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
};
struct Text{
    string t;
    Vector2 pos;
    Color text_color;
    Align alix,aliy;
    int Fontsize;
};

inline const Button restartBtn={
    "restart",
    {WIN_W-240.f,WIN_H-MARGIN_DOWN+CELL_SIZE/2.f,100.f,30.f},
    LIGHTGRAY,BLACK,BLACK,
    20
};
inline const Button resignBtn={
    "resigned",
    {WIN_W-120.f,WIN_H-MARGIN_DOWN+CELL_SIZE/2.f,100.f,30.f},
    LIGHTGRAY,BLACK,BLACK,
    20
};