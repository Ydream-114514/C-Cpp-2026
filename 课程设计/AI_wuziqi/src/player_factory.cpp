#include "player_factory.h"

namespace {
    unordered_map<PlayerType, PlayerFactory>& registry(){
        static unordered_map<PlayerType, PlayerFactory> r;
        return r;
    }
}

void registerPlayer(PlayerType t,PlayerFactory f){
    registry()[t]=move(f);
}

shared_ptr<Qishou> createPlayer(PlayerType t,shared_ptr<AIConfig> x){
    auto it=registry().find(t);
    if(it==registry().end()||!x) return nullptr;
    return it->second(*x);
}