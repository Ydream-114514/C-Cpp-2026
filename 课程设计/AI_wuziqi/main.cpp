#include "game.h"
#include "ui.h"

int main(){
    InitWindow(WIN_W, WIN_H, "五子棋");
    SetTargetFPS(60);
    GameState g(Player_mode::AI_hard,Player_mode::Player);
    while(!WindowShouldClose()){
        g.run();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        printBoard(g,{returnBtn,resignBtn,restartBtn});
        EndDrawing();
    }
    CloseWindow();
    return 0;
}