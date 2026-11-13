#include "game.h"
#include "ui.h"
#include "player_factory.h"

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

shared_ptr<Qishou> new_qishou(Player_mode x,int color){
    switch(x){
        case Player_mode::Player:
            _player.color=color;
            return createPlayer(PlayerType::Human,&_player);
        case Player_mode::AI_eazy:
            _AI_eazy.color=color;
            return createPlayer(PlayerType::AI_search,&_AI_eazy);
        case Player_mode::AI_hard:
            _AI_hard.color=color;
            return createPlayer(PlayerType::AI_search,&_AI_hard);
        case Player_mode::AI_nn:
            _AI_nn.color=color;
            return createPlayer(PlayerType::AI_NN,&_AI_nn);
        default:throw 0;
    }
}

void GameState::initGame(){
    board=vector<vector<int>>(BOARD_SIZE,vector<int>(BOARD_SIZE,Chess_status::Empty));
    bool blackFirst=randInt(0,1);
    tie(color[blackFirst],color[blackFirst^1])=make_pair(Chess_status::Black,Chess_status::White);
    turn=blackFirst;
    gameOver=lastr=lastc=-1;
    moveCount=0;
    st=stack<pair<int,int>>();
    p[0]=new_qishou(pp[0],color[0]);
    p[1]=new_qishou(pp[1],color[1]);
}
void GameState::ExitGame(){
    p[0].reset();
    p[1].reset();
}
void GameState::Handle_result(Action &result){
    switch(result.type){
        case ACT_MOVE:
            board[result.row][result.col]=color[turn];
            moveCount++;
            lastr=result.row;
            lastc=result.col;
            st.emplace(result.row,result.col);
            if(checkWin(board,result.row,result.col,color[turn])){
                gameOver=turn;
                score[turn]++;
            }else if(moveCount==BOARD_SIZE*BOARD_SIZE){
                gameOver=2;
            }else turn^=1;
            break;
        default:break;
    }
}
void GameState::run(){
    if(IsButtonClicked(restartBtn)){
        initGame();
    }else if(!~gameOver&&IsButtonClicked(resignBtn)){
        gameOver=p[0]->isplayer()?1:0;
        cerr<<gameOver<<endl;
        score[gameOver]++;
    }
    if(IsButtonClicked(returnBtn)){
        if(!st.empty()){
            gameOver=-1;
            auto [r,c]=st.top();
            st.pop();
            board[r][c]=Chess_status::Empty;
            if(st.empty()) lastc=lastr=-1;
            else{
                tie(r,c)=st.top();
                st.pop();
                board[r][c]=Chess_status::Empty;
                if(st.empty()) lastc=lastr=-1;
                else tie(lastr,lastc)=st.top();
            }
        }
    }
    if(!~gameOver){
        auto result=p[turn]->turn(board);
        Handle_result(result);
    }
}