#include "Zobristhash.h"

void ZobristHash::initZobristHash(int row,int col){
    uniform_int_distribution<unsigned long long> dis(0,ULLONG_MAX);
    zobrist_table[0].resize(row,vector<unsigned long long>(col));
    zobrist_table[1].resize(row,vector<unsigned long long>(col));
    for(int r=0;r<row;++r)
        for(int c=0;c<col;++c){
            zobrist_table[0][r][c]=dis(rng);
            zobrist_table[1][r][c]=dis(rng);
        }
            
}
unsigned long long ZobristHash::computeHash(const vector<vector<int>>& board)const{
    unsigned long long hash_value=0;
    for(int r=0;r<(int)board.size();++r)
        for(int c=0;c<(int)board[0].size();++c){
            int s=board[r][c];
            if(s!=Chess_status::Empty){
                int color=s-1;
                hash_value^=zobrist_table[color][r][c];
            }
        }
    return hash_value;
}
bool ZobristHash::Find(unsigned long long hash_value)const{
    return hash_map.find(hash_value)!=hash_map.end();
}
void ZobristHash::Insert(unsigned long long hash_value,int r,int c){
    hash_map[hash_value]={r,c};
}
pair<int,int> ZobristHash::Get(unsigned long long hash_value)const{
    return hash_map.at(hash_value);
}
unsigned long long ZobristHash::Add(unsigned long long hash_value,int r,int c,int color)const{
    return hash_value^zobrist_table[color-1][r][c];
}