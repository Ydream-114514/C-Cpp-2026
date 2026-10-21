#pragma once
#include "common.h"

class Qishou{
protected:
    int row,col,color;
    bool in(int x,int y)const{return x>=0&&x<row&&y>=0&&y<col;}
public:
    string name;
    Qishou()=delete;
    Qishou(int row,int col,int color,string name):row(row),col(col),color(color),name(name){}
    virtual Action turn(const vector<vector<int>>&) const=0;
    virtual bool isThinking()const=0;
    virtual ~Qishou()=default;
};