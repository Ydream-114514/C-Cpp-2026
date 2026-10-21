#include "ui.h"

void DrawButton(const Button& btn){
    DrawRectangleRec(btn.pos, btn.Back_ground);
    DrawRectangleLinesEx(btn.pos, 1.5f, btn.Back_ground_outline);
    Vector2 textSize=MeasureTextEx(GetFontDefault(),btn.name.c_str(),btn.Fontsize,1);
    Vector2 textPos={
        btn.pos.x+(btn.pos.width-textSize.x)/2,
        btn.pos.y+(btn.pos.height-textSize.y)/2
    };
    DrawText(btn.name.c_str(),(int)textPos.x,(int)textPos.y,btn.Fontsize,btn.text_color);
}
void Draw_text_align(Text t){
    Vector2 textSize=MeasureTextEx(GetFontDefault(),t.t.c_str(),t.Fontsize,1);
    float x=t.pos.x;
    if(t.alix==ALIGN_CENTER) x=t.pos.x-textSize.x/2;
    if(t.alix==ALIGN_RIGHT) x=t.pos.x-textSize.x;
    float y=t.pos.y;
    if(t.aliy==ALIGN_CENTER) y=t.pos.y-t.Fontsize/2;
    if(t.aliy==ALIGN_RIGHT) y=t.pos.y-t.Fontsize;
    DrawText(t.t.c_str(),x,y,t.Fontsize,t.text_color);
}
bool IsButtonClicked(const Button& btn){
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        && CheckCollisionPointRec(GetMousePosition(), btn.pos);
}
Vector2 BoardToScreen(int row,int col){
    return {(float)(MARGIN_LEFT+col*CELL_SIZE),(float)(MARGIN_UP+row*CELL_SIZE)};
}
void DrawStone(Vector2 c, float r, bool isBlack) {
    // 1. 外描边（比主体大 1.5~2 像素）
    DrawCircleV(c,r+1.5f,isBlack?BLACK:DARKGRAY);
    // 2. 主体（渐变）
    if(isBlack) DrawCircleGradient(c,r,Color{60,60,60,255},Color{10,10,10,255});
    else DrawCircleGradient(c,r,Color{255,255,255,255},Color{205,205,205,255});
    // 3. 高光（左上偏移）
    // DrawCircleV({c.x-r*0.35f,c.y-r*0.35f},r*0.28f,isBlack?Color{180,180,180,120}:Color{255,255,255,240});
}
void printBoard(GameState &g){
    // 画横线和竖线
    for(int i=0;i<BOARD_SIZE;i++){
        Vector2 start=BoardToScreen(i,0);
        Vector2 end=BoardToScreen(i,BOARD_SIZE-1);
        DrawLineV(start,end,BLACK); // 画横线
        start=BoardToScreen(0,i);
        end=BoardToScreen(BOARD_SIZE-1,i);
        DrawLineV(start,end,BLACK); // 画竖线
    }
    // 画天元和星位的小点
    vector<int> ct({3,BOARD_SIZE/2,BOARD_SIZE-4});
    for(auto i:ct)
        for(auto j:ct)
            DrawCircleV(BoardToScreen(i,j),4,BLACK);
    for(int r=0;r<BOARD_SIZE;r++){
        for(int c=0;c<BOARD_SIZE;c++){
            if (g.board[r][c]==EMPTY) continue;
            Vector2 pos=BoardToScreen(r,c);
            DrawStone(pos,CELL_SIZE*0.45f,g.board[r][c]==Chess_Color_BLACK); // 棋子半径为格子大小的 0.45 倍
        }
    }
    if(g.lastr>=0){
        Vector2 p=BoardToScreen(g.lastr,g.lastc);
        DrawCircleLines(p.x,p.y,CELL_SIZE*0.45,RED);
    }
    DrawText(("Your color is "+(string)(g.color[0]==Chess_Color_BLACK?"BLACK":"WHITE")).c_str(), MARGIN_LEFT, WIN_H - 90, 20, DARKGRAY);
    DrawText(("It's "+g.p[g.turn]->name+"'s turn.").c_str(), MARGIN_LEFT, WIN_H - 60, 20, DARKGRAY);
    if(~g.gameOver){
        switch(g.gameOver){
            case 0:case 1:
                DrawText((g.p[g.gameOver]->name+" win").c_str(), MARGIN_LEFT, WIN_H - 30, 20, DARKGRAY);
                break;
            case 2:
                DrawText("pingju", MARGIN_LEFT, WIN_H - 30, 20, DARKGRAY);
                break;
        }
    }
    DrawButton(restartBtn);
    DrawButton(resignBtn);
    Draw_text_align({g.p[0]->name,{WIN_W/2-60,30},BLACK,ALIGN_RIGHT,ALIGN_CENTER,40});
    Draw_text_align({g.p[1]->name,{WIN_W/2+60,30},BLACK,ALIGN_LEFT,ALIGN_CENTER,40});
    Draw_text_align({":",{WIN_W/2,30},BLACK,ALIGN_CENTER,ALIGN_CENTER,40});
    Draw_text_align({to_string(g.score[0]),{WIN_W/2-10,30},BLACK,ALIGN_RIGHT,ALIGN_CENTER,30});
    Draw_text_align({to_string(g.score[1]),{WIN_W/2+10,30},BLACK,ALIGN_LEFT,ALIGN_CENTER,30});
    if(!~g.gameOver){
        if(g.turn==0){
            if(g.p[0]->isThinking())
                Draw_text_align({g.p[0]->name+" is thinking...",{WIN_W/2-60,65},ORANGE,ALIGN_RIGHT,ALIGN_CENTER,20});
        }else{
            if(g.p[1]->isThinking())
                Draw_text_align({g.p[1]->name+" is thinking...",{WIN_W/2+60,65},ORANGE,ALIGN_LEFT,ALIGN_CENTER,20});
        }
    }
}