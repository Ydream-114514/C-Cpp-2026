#pragma once
#include<bits/stdc++.h>
using namespace std;

const int BOARD_SIZE=15;
const int CELL_SIZE=40;        // 每个格子 40 像素
const int MARGIN_UP=80;
const int MARGIN_DOWN=100;
const int MARGIN_LEFT=40;
const int MARGIN_RIGHT=40;     // 棋盘边缘留白
const int WIN_W=CELL_SIZE*(BOARD_SIZE-1)+MARGIN_LEFT+MARGIN_RIGHT;
const int WIN_H=CELL_SIZE*(BOARD_SIZE-1)+MARGIN_UP+MARGIN_DOWN;
extern mt19937 rng;
int randInt(int,int);
bool rand_float(double);
typedef double Score_type;

enum Chess_status{
    Empty=0,
    Black=1,
    White=2
};

enum class PlayerType{
    Human,
    AI_random,
    AI_search,
    AI_NN,
    AI_MCTS
};

enum ActionType{
    ACT_MOVE,
    ACT_RESIGN,
    ACT_RESTART,
    ACT_JUMP
};

struct Action{
    ActionType type;
    int row;
    int col;
    Action()=default;
    Action(ActionType t,int r,int c):type(t),row(r),col(c){}
};

class Board:public vector<vector<int>>{
    int r,c;
public:
    Board()=delete;
    Board(int r,int c):vector<vector<int>>(r,vector<int>(c,Chess_status::Empty)),r(r),c(c){}
    bool in(int x,int y)const{
        return x>=0&&x<r&&y>=0&&y<c;
    }
    pair<int, int> get_size() const {
        return make_pair(r, c);
    }
    int count()const{
        int cnt=0;
        for(auto &i:*this)
            for(auto &j:i)
                if(j!=Chess_status::Empty)
                    cnt++;
        return cnt;
    }
    void clear(){
        for(auto &it:*this)
            fill(it.begin(),it.end(),Chess_status::Empty);
    }
    bool isfull()const{
        return count()==r*c;
    }
    bool check_win(int r,int c,int color)const{
        static const int dirs[4][2]={{0,1},{1,0},{1,1},{1,-1}};
        for(const auto &[dr,dc]:dirs){
            int cnt=1;
            for(int s=1;s<5;++s){
                int nr=r+dr*s,nc=c+dc*s;
                if(!in(nr,nc)||(*this)[nr][nc]!=color) break;
                ++cnt;
            }
            for(int s=1;s<5;++s){
                int nr=r-dr*s,nc=c-dc*s;
                if(!in(nr,nc)||(*this)[nr][nc]!=color) break;
                ++cnt;
            }
            if(cnt>=5) return true;
        }
        return false;
    }
};