#include "ai.h"

Action AI::turn(const Board &board)const{
    if(!thinking_){
        thinking_=1;
        auto self=shared_from_this();
        future_=async(launch::async,[self,board](){
            return self->computeMove(board);
        });
        return {ACT_JUMP,-1,-1};
    }
    if(future_.wait_for(0ms)==future_status::ready){
        auto result=future_.get();
        thinking_=0;
        return result;
    }
    return {ACT_JUMP,-1,-1};
}