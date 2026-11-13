#pragma once
#include "common.h"

// 矩阵类型：neural_matrix[i][j]
typedef vector<vector<float>> neural_matrix;

// 新建 n 行 m 列全零矩阵
neural_matrix new_matrix(int n, int m);

class neural_network {
    vector<int> layer_sizes;     // 各层神经元数
    const int L;                 // 权重层数 = layer_sizes.size()-1
    const int epochs = 100;
    const float p = 0.1;         // dropout 丢弃率
    const float momentum = 0.9f; // 动量


    // 网络参数与中间缓冲（mutable：训练函数为 const）
    mutable vector<neural_matrix> W, B;                     // 权重 / 偏置
    mutable vector<neural_matrix> Z, A, dZ, dW;             // 前向 / 反向中间量
    mutable vector<neural_matrix> sum_dW, sum_dB, tA, tW, dA;
    mutable vector<neural_matrix> vW, vB;                   // 动量
    mutable vector<neural_matrix> D;                        // dropout 掩码

    void init_network() const;
    void init_buffers() const;
    void forward(const neural_matrix&, bool = false) const;
    void backward(const neural_matrix&) const;
    void update_params(float) const;

public:
    neural_network(int r, int c)
        : layer_sizes({r*c+1,r*c*3,r*c*2,r*c}),
          L(layer_sizes.size()-1),
          Row(r),
          Col(c){
        init_network();
        init_buffers();
    }

    // 成员均为 RAII 容器，默认析构即可
    ~neural_network() = default;

    int predict(const neural_matrix&) const;
    void load_data(string, vector<neural_matrix>&, vector<neural_matrix>&) const;
    void training(string) const;
    void save_model(const string&) const;
    bool load_model(const string&) const;
protected:
    int Row,Col;
};
