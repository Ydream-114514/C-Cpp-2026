#pragma once
#include "Zobristhash.h"
#include "ai.h"

struct AI_searchConfig:QishouConfig{
    int depth;      // alpha-beta 搜索层数（ply）
    int pace;       // 候选点邻域半径
    int K;         // 每层保留的候选点个数
    int vcfDepth_;  // VCF 连续冲四搜索深度
    int vctDepth_;  // VCT 连续冲三搜索深度
    AI_searchConfig()=default;
    AI_searchConfig(int row,int col,int color,string name,
             int depth=4,int pace=2,int K=10,int vcfDepth_=8,int vctDepth_=6)
        :QishouConfig(row,col,color,name),
         depth(depth),pace(pace),K(K),vcfDepth_(vcfDepth_),vctDepth_(vctDepth_){}
};
class AI_search:public AI,private ZobristHash{
    int depth,pace;
    int K,vcfDepth_,vctDepth_;
public:
    AI_search(AI_searchConfig *x):AI(x),ZobristHash(x->row,x->col){
        depth=x->depth;
        pace=x->pace;
        K=x->K;
        vcfDepth_=x->vcfDepth_;
        vctDepth_=x->vctDepth_;
    }
private:
    bool check_model(const Board&,int,int,int,int,const vector<int>&,int,bool)const;
    Score_type patternScore(int,int,int)const;
    Score_type calc(const Board&,int)const;
    Score_type get_score(const Board&)const;
    Score_type local_score(const Board&,int,int,int)const;
    bool check(const Board&,int,int)const;
    vector<tuple<int,int,Score_type>> collection(Board&,int,bool)const;
    Score_type alpha_beta(Board&,Score_type,Score_type,int,bool)const;
    Action a_b_search(const Board&)const;
    Action search(Board&)const;
    Action vcf(Board&,int,int)const;
    Action vct(Board&,int,int,unsigned long long)const;
    bool check_db_thr(const Board&,int,int,int)const;
    Action double_three(Board&,int)const;
    Action check_Four(const Board&,int)const;
    Action check_live_Three(Board&,int)const;
    vector<Action> check_live_Three1(Board&,int)const;
protected:
    Action Rand_step(const Board&)const;
    Action suansha(Board&)const;
    virtual Action computeMove(Board)const;
};

bool checkWin(const Board&,int,int,int);