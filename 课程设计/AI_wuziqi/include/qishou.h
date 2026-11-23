#pragma once
#include "common.h"

struct QishouConfig{
    int row,col,color;
    string name;
    QishouConfig(){}
    QishouConfig(int row,int col,int color,string name):row(row),col(col),color(color),name(name){}
    virtual ~QishouConfig()=default;
};

class Qishou{
protected:
    int row,col,color;
public:
    string name;
    Qishou()=delete;
    Qishou(QishouConfig *x){
        row=x->row;
        col=x->col;
        color=x->color;
        name=x->name;
    };
    virtual Action turn(const Board&)const=0;
    virtual bool isThinking()const=0;
    virtual ~Qishou()=default;
    virtual bool isplayer()const=0;
};