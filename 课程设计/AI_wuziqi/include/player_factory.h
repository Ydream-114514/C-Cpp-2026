#pragma once
#include"game.h"

using PlayerFactory=function<shared_ptr<Qishou>(AIConfig&)>;

void registerPlayer(PlayerType t, PlayerFactory f);
shared_ptr<Qishou> createPlayer(PlayerType t, shared_ptr<AIConfig> x);

template<class T>
struct PlayerRegistrar{
    explicit PlayerRegistrar(PlayerType t){
        registerPlayer(t,[](AIConfig &c){return std::make_shared<T>(c);});
    }
};