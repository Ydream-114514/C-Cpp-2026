//最短代码，没有之一
#include<bits/stdc++.h>
template<typename T>
class Array:public std::vector<T>{};
int main(){
	Array<int> a;
	a.push_back(1);
	for(auto i:a)
		std::cout<<i<<std::endl;
	return 0;
}