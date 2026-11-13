#pragma once
#include "common.h"
#include "qishou.h"

using PlayerFactory=function<shared_ptr<Qishou>(QishouConfig*)>;

void registerPlayer(PlayerType t, PlayerFactory f);
shared_ptr<Qishou> createPlayer(PlayerType t,QishouConfig *x);

template<class T,class ConfigT=QishouConfig>
struct PlayerRegistrar{
    explicit PlayerRegistrar(PlayerType t){
        registerPlayer(t,[](QishouConfig *c) -> shared_ptr<Qishou> {
            ConfigT *cc=dynamic_cast<ConfigT*>(c);
            return std::make_shared<T>(cc);
        });
    }
};