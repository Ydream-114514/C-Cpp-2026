#pragma once
#include "common.h"
#include "game.h"

void DrawButton(const Button&);
void Draw_text_align(Text);
bool IsButtonClicked(const Button&);
Vector2 BoardToScreen(int,int);
void DrawStone(Vector2,float,bool);
void printBoard(GameState &);