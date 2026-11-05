#include "ui.h"
#include "game.h"
#include "player.h"

int main(){
    InitWindow(WIN_W, WIN_H, "五子棋");
    SetTargetFPS(60);
    GameState g;
    GameConfig cfg;
    cfg.p[0]=PlayerType::AI;
    cfg.p[1]=PlayerType::Human;
    cfg.c[0]=make_shared<AIConfig>(BOARD_SIZE,BOARD_SIZE,-1,"AI",4,2,10,8,5);
    cfg.c[1]=make_shared<AIConfig>(BOARD_SIZE,BOARD_SIZE,-1,"Player");
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
                g.board[r][c]=Chess_status::Empty;
                tie(r,c)=g.st.top();
                g.st.pop();
                g.board[r][c]=Chess_status::Empty;
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
        printBoard(g,{returnBtn,resignBtn,restartBtn});
        EndDrawing();
    }
    ExitGame(g);
    CloseWindow();
    return 0;
}