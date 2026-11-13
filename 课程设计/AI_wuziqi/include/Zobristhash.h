#pragma once
#include "common.h"

class ZobristHash{
    vector<vector<unsigned long long>> zobrist_table[2];
    mutable map<unsigned long long,pair<int,int>> hash_map;
public:
    ZobristHash(int r,int c){initZobristHash(r,c);}
protected:
    void initZobristHash(int,int);
    unsigned long long computeHash(const vector<vector<int>>&)const;
    bool Find(unsigned long long)const;
    void Insert(unsigned long long,int,int)const;
    pair<int,int> Get(unsigned long long)const;
    unsigned long long Add(unsigned long long,int,int,int)const;
};