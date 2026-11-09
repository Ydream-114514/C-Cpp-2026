#pragma once
#include "common.h"
#include "qishou.h"
#include "Zobristhash.h"

struct AIConfig:QishouConfig{
    int depth;      // alpha-beta 搜索层数（ply）
    int pace;       // 候选点邻域半径
    int K;         // 每层保留的候选点个数
    int vcfDepth_;  // VCF 连续冲四搜索深度
    int vctDepth_;  // VCT 连续冲三搜索深度
    AIConfig()=default;
    AIConfig(int row,int col,int color,string name,
             int depth=4,int pace=2,int K=10,int vcfDepth_=8,int vctDepth_=6)
        :QishouConfig(row,col,color,name),
         depth(depth),pace(pace),K(K),vcfDepth_(vcfDepth_),vctDepth_(vctDepth_){}
};
class AI : public Qishou,public enable_shared_from_this<AI>,private ZobristHash{
    mutable future<Action> future_;
    mutable bool thinking_ = false;
    int depth,pace;
    int K,vcfDepth_,vctDepth_;
public:
    AI(AIConfig x):Qishou(x){
        initZobristHash(x.row,x.col);
        depth=x.depth;
        pace=x.pace;
        K=x.K;
        vcfDepth_=x.vcfDepth_;
        vctDepth_=x.vctDepth_;
    }
    Action turn(const vector<vector<int>> &)const override;
    bool isThinking()const override{return thinking_;}
private:
    bool check_model(const vector<vector<int>>&,int,int,int,int,const vector<int>&,int,bool)const;
    Score_type patternScore(int,int,int)const;
    Score_type calc(const vector<vector<int>>&,int)const;
    Score_type get_score(const vector<vector<int>>&)const;
    Score_type local_score(const vector<vector<int>>&,int,int,int)const;
    bool check(const vector<vector<int>>&,int,int)const;
    vector<tuple<int,int,Score_type>> collection(vector<vector<int>>&,int,bool)const;
    Score_type alpha_beta(vector<vector<int>>&,Score_type,Score_type,int,bool)const;
    Action a_b_search(const vector<vector<int>>&)const;
    Action search(vector<vector<int>>&)const;
    Action vcf(vector<vector<int>>&,int,int)const;
    Action vct(vector<vector<int>>&,int,int,unsigned long long)const;
    bool check_db_thr(const vector<vector<int>>&,int,int,int)const;
    Action double_three(vector<vector<int>>&,int)const;
    Action check_Four(const vector<vector<int>>&,int)const;
    Action check_live_Three(vector<vector<int>>&,int)const;
    vector<Action> check_live_Three1(vector<vector<int>>&,int)const;
protected:
    Action Rand_step(const vector<vector<int>>&)const;
    Action suansha(vector<vector<int>>&)const;
    virtual Action computeMove(vector<vector<int>>)const;
};

bool checkWin(const vector<vector<int>>&,int,int,int);
