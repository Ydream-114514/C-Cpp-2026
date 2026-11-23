#pragma once
#include "neuralnetwork.h"
#include "ai.h"

struct AI_NNConfig:QishouConfig{
    string model_file;
    AI_NNConfig(int row,int col,int color,string name,string model_file):QishouConfig(row,col,color,name),
                    model_file(model_file){}
};

class AI_NN:public AI,public neural_network{
    mutable future<Action> future_;
    mutable bool thinking_ = false;
    Action computeMove(Board)const override;
public:
    AI_NN(AI_NNConfig *x):AI(x),neural_network(x->row,x->col){
        load_model(x->model_file);
    }
};