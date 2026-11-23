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

class Board{
    vector<vector<int>> mp;
    int r,c;
public:
    Board()=delete;
    Board(int r,int c):mp(r,vector<int>(c,Chess_status::Empty)),r(r),c(c){}
    bool in(int x,int y)const{
        return x>=0&&x<r&&y>=0&&y<c;
    }
    pair<int, int> get_size() const {
        return make_pair(r, c);
    }
    int count()const{
        int cnt=0;
        for(auto &i:mp)
            for(auto &j:i)
                if(j!=Chess_status::Empty)
                    cnt++;
        return cnt;
    }
    vector<int> &operator[](int x){
        return mp[x];
    }
    const vector<int> &operator[](int x)const{
        return mp[x];
    }
    void clear(){
        for(auto &it:mp)
            fill(it.begin(),it.end(),Chess_status::Empty);
    }
    auto begin(){
        return mp.begin();
    }
    const auto begin()const{
        return mp.begin();
    }
    auto end(){
        return mp.end();
    }
    const auto end()const{
        return mp.end();
    }
    bool isfull()const{
        return count()==r*c;
    }
};