#include "game.h"
#include "ui.h"

int main(){
    InitWindow(WIN_W, WIN_H, "五子棋");
    SetTargetFPS(60);
    GameState g;
    GameConfig cfg;
    cfg.p[0]=PlayerType::AI;
    cfg.p[1]=PlayerType::Human;
    cfg.c[0]=make_shared<AIConfig>(BOARD_SIZE,BOARD_SIZE,-1,"AI",4,2,10,8,6);
    cfg.c[1]=make_shared<AIConfig>(BOARD_SIZE,BOARD_SIZE,-1,"Player");
    initGame(g,cfg);
    while(!WindowShouldClose()){
        run(g);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        printBoard(g,{returnBtn,resignBtn,restartBtn});
        EndDrawing();
    }
    ExitGame(g);
    CloseWindow();
    return 0;
}