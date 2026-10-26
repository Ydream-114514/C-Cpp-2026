#include "ui.h"
#include "game.h"
#include "player.h"

int main(){
    InitWindow(WIN_W, WIN_H, "五子棋");
    SetTargetFPS(60);
    GameState g;
    initGame(g);
    while(!WindowShouldClose()){
        bool buttonHandled=0;
        if(IsButtonClicked(restartBtn)){
            initGame(g);
            buttonHandled=1;
        }else if(!~g.gameOver&&IsButtonClicked(resignBtn)){
            cerr<<g.turn<<endl;
            g.gameOver=typeid(g.p[1]).name()==typeid(Player).name();
            g.score[g.gameOver]++;
            buttonHandled=1;
        }
        if(!buttonHandled&&!~g.gameOver){
            auto result=g.p[g.turn]->turn(g.board);
            Handle_result(g,result);
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        printBoard(g);
        EndDrawing();
    }
    ExitGame(g);
    CloseWindow();
    return 0;
}