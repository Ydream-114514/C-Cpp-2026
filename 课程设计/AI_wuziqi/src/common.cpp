#include "common.h"

mt19937 rng((unsigned)chrono::steady_clock::now().time_since_epoch().count());

int randInt(int lo,int hi){return uniform_int_distribution<>(lo,hi)(rng);}
bool rand_float(double eps){return uniform_real_distribution<>(0,1)(rng)<eps;}