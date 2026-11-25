#include "game.h"
#include "ui.h"
vector<Color> cl{RED,ORANGE,YELLOW,GREEN,BLUE,DARKBLUE,PURPLE};
int main(){
    InitWindow(WIN_W, WIN_H, "五子棋");
    SetTargetFPS(20);
    GameState g(Player_mode::AI_MCTS,Player_mode::Player);
    // int i=0,bandCount=cl.size();
    while(!WindowShouldClose()){
        g.run();
        BeginDrawing();
        // Color bottomColor = cl[(i++) % bandCount];
        ClearBackground(RAYWHITE);
        // ClearBackground(bottomColor);
        printBoard(g,{returnBtn,resignBtn,restartBtn});
        EndDrawing();
    }
    CloseWindow();
    return 0;
}