#include "ai_nn.h"
#include "player_factory.h"

static PlayerRegistrar<AI_NN,AI_NNConfig> reg_ai_search(PlayerType::AI_NN);

Action AI_NN::computeMove(Board mp)const{
    neural_matrix input=new_matrix(1,row*col+1);
    for(int r=0;r<row;++r)
        for(int c=0;c<col;++c)
            input[0][r*col+c]=mp[r][c];
    input[0][row*col]=color;
    int val=predict(input);
    int r,c;
    r=val/col;
    c=val%col;
    return {ACT_MOVE,r,c};
}