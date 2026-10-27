#include "game.h"
#include "player.h"
#include "ai.h"

bool checkWin(const vector<vector<int>>& board, int row, int col, int player){
    static const int dirs[4][2] ={{0,1},{1,0},{1,1},{1,-1}};
    for(const auto& [dr,dc] : dirs){
        int count=1;
        for(int step=1;step<5;++step){ 
            int r=row+dr * step;
            int c=col+dc * step;
            if(r<0 || r >= BOARD_SIZE || c<0 || c >= BOARD_SIZE) break;
            if(board[r][c]==player) ++count;
            else break;
        }
        for(int step=1;step<5;++step){
            int r=row-dr * step;
            int c=col-dc * step;
            if(r<0 || r >= BOARD_SIZE || c<0 || c >= BOARD_SIZE) break;
            if(board[r][c]==player) ++count;
            else break;
        }
        if (count>=5) return true;
    }
    return false;
}

void initGame(GameState &g){
    g.board=vector<vector<int>>(BOARD_SIZE,vector<int>(BOARD_SIZE,EMPTY));
    bool blackFirst=randInt(0,1);
    g.color[0]=blackFirst?Chess_Color_WHITE:Chess_Color_BLACK;
    g.color[1]=blackFirst?Chess_Color_BLACK:Chess_Color_WHITE;
    g.turn=blackFirst;
    g.lastr=g.lastc=-1;
    g.moveCount=0;
    g.gameOver=-1;
    g.p[0]=make_shared<AI>(BOARD_SIZE,BOARD_SIZE,g.color[0],"AI");
    g.p[1]=make_shared<Player>(BOARD_SIZE,BOARD_SIZE,g.color[1],"Player");
    g.st=stack<pair<int,int>>();
}
void ExitGame(GameState &g){
    g.p[0].reset();
    g.p[1].reset();
}
void Handle_result(GameState &g,Action &result){
    switch(result.type){
        case ACT_MOVE:
            g.board[result.row][result.col]=g.color[g.turn];
            g.moveCount++;
            g.lastr=result.row;
            g.lastc=result.col;
            g.st.emplace(result.row,result.col);
            if(checkWin(g.board,result.row,result.col,g.color[g.turn])){
                g.gameOver=g.turn;
                g.score[g.turn]++;
            }else if(g.moveCount==BOARD_SIZE*BOARD_SIZE){
                g.gameOver=2;
            }else g.turn^=1;
            break;
        default:break;
    }
}