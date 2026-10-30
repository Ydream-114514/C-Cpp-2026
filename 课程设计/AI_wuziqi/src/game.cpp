#include "game.h"
#include "player.h"
#include "ai.h"
#include "ai_random.h"

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
shared_ptr<Qishou> createPlayer(PlayerType t,int color,const string& name){
    switch(t){
        case PlayerType::Human:
            return make_shared<Player>(BOARD_SIZE,BOARD_SIZE,color,name);
        case PlayerType::AI:
            return make_shared<AI>(BOARD_SIZE,BOARD_SIZE,color,name);
        case PlayerType::AI_random:
            return make_shared<AI_random>(BOARD_SIZE,BOARD_SIZE,color,name);
    }
    return nullptr;
}

void initGame(GameState &g,GameConfig &cfg){
    g.cfg=cfg;
    g.board=vector<vector<int>>(BOARD_SIZE,vector<int>(BOARD_SIZE,Chess_status::Empty));
    bool blackFirst=randInt(0,1);
    g.color[0]=blackFirst?Chess_status::White:Chess_status::Black;
    g.color[1]=blackFirst?Chess_status::Black:Chess_status::White;
    g.turn=blackFirst;
    g.lastr=g.lastc=-1;
    g.moveCount=0;
    g.gameOver=-1;
    g.st=stack<pair<int,int>>();
    g.p[0]=createPlayer(cfg.p0,g.color[0],cfg.name0);
    g.p[1]=createPlayer(cfg.p1,g.color[1],cfg.name1);
    
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