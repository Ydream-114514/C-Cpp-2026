#pragma once
#include "common.h"
#include "qishou.h"
class AI : public Qishou,public enable_shared_from_this<AI>{
    mutable future<Action> future_;
    mutable bool thinking_ = false;
    int depth=4,pace=2;
    int K=30;
public:
    AI(int row,int col,int color,string name):Qishou(row,col,color,name){}
    bool isThinking()const override{return thinking_;}
    Action turn(const vector<vector<int>> &)const override;
private:
    bool check_model(const vector<vector<int>>&,const int&,const int&,const int&,const int&,const int*,int,bool)const;
    Score_type calc(const vector<vector<int>>&,int)const;
    Score_type get_score(const vector<vector<int>>&)const;
    bool check(const vector<vector<int>>&,int,int)const;
    Action Rand_step(const vector<vector<int>>&)const;
    Score_type alpha_beta(vector<vector<int>>&,Score_type,Score_type,int,bool)const;
    Action a_b_search(const vector<vector<int>>&)const;
    Action search(vector<vector<int>>&)const;
    Action computeMove(vector<vector<int>>)const;
};