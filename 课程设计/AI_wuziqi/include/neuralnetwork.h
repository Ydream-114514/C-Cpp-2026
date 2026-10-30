#pragma once
#include "common.h"
#include "ai.h"

typedef vector<vector<float>> neural_matrix;
neural_matrix new_matrix(int,int);

class neural_network:public AI{
    const vector<int> layer_sizes={row*col+1,row*col*30,row*col*20,row*col};
    const int L=layer_sizes.size()-1;
    const int epochs=100;
    const float p=0.1;
    const float momentum=0.9f;
    mutable vector<neural_matrix> W=vector<neural_matrix>(L),B=vector<neural_matrix>(L);
    mutable vector<neural_matrix> Z=vector<neural_matrix>(L),A=vector<neural_matrix>(L+1),dZ=vector<neural_matrix>(L),dW=vector<neural_matrix>(L);
    mutable vector<neural_matrix> sum_dW=vector<neural_matrix>(L),sum_dB=vector<neural_matrix>(L),tA=vector<neural_matrix>(L),tW=vector<neural_matrix>(L),dA=vector<neural_matrix>(L);
    mutable vector<neural_matrix> vW=vector<neural_matrix>(L), vB=vector<neural_matrix>(L);
    mutable vector<neural_matrix> D=vector<neural_matrix>(L);

    void init_network()const;
    void init_buffers()const;
    void forward(const neural_matrix&,bool)const;
    void backward(const neural_matrix&)const;
    void update_params(float)const;
    Action computeMove(vector<vector<int>>)const;
public:
    int predict(const neural_matrix&)const;
    void load_data(string,vector<neural_matrix>&,vector<neural_matrix>&)const;
    void training()const;
    void save_model(const string&)const;
    bool load_model(const string&)const;
};
