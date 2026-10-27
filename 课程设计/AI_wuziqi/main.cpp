#include "ui.h"
#include "game.h"
#include "player.h"

int main(){
    InitWindow(WIN_W, WIN_H, "五子棋");
    SetTargetFPS(60);
    GameState g;
    GameConfig cfg;
    cfg.p0=PlayerType::AI;
    cfg.p1=PlayerType::Human;
    cfg.name0="AI";
    cfg.name1="Player";
    initGame(g,cfg);
    while(!WindowShouldClose()){
        bool buttonHandled=0;
        if(IsButtonClicked(restartBtn)){
            initGame(g,g.cfg);
            buttonHandled=1;
        }else if(!~g.gameOver&&IsButtonClicked(resignBtn)){
            cerr<<g.turn<<endl;
            g.gameOver=typeid(*g.p[1]).name()==typeid(Player).name();
            g.score[g.gameOver]++;
            buttonHandled=1;
        }
        if(IsButtonClicked(returnBtn)){
            if(g.st.size()>=2){
                g.gameOver=-1;
                auto [r,c]=g.st.top();
                g.st.pop();
                g.board[r][c]=EMPTY;
                tie(r,c)=g.st.top();
                g.st.pop();
                g.board[r][c]=EMPTY;
                if(g.st.empty()) g.lastc=g.lastr=-1;
                else tie(g.lastr,g.lastc)=g.st.top();
            }
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