#pragma once
#include "common.h"
#include "game.h"
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

inline const Button resignBtn={
    "resigned",
    {WIN_W-120.f,WIN_H-MARGIN_DOWN+CELL_SIZE/2.f,100.f,30.f},
    LIGHTGRAY,BLACK,BLACK,
    20
};

inline const Button restartBtn={
    "restart",
    {WIN_W-240.f,WIN_H-MARGIN_DOWN+CELL_SIZE/2.f,100.f,30.f},
    LIGHTGRAY,BLACK,BLACK,
    20
};

inline const Button returnBtn={
    "return",
    {WIN_W-360.f,WIN_H-MARGIN_DOWN+CELL_SIZE/2.f,100.f,30.f},
    LIGHTGRAY,BLACK,BLACK,
    20
};

void DrawButton(const Button&);
void Draw_text_align(Text);
bool IsButtonClicked(const Button&);
Vector2 BoardToScreen(int,int);
void DrawStone(Vector2,float,bool);
void printBoard(GameState &);