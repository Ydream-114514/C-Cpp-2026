#include<bits/stdc++.h>
#include <windows.h>
#include <psapi.h>
using namespace std;
using namespace std::chrono;
typedef long long ll;
const int k=8,base=100000000;
const int maxn = 4e4+10;
ll res[maxn];
char ss[maxn]="1415926";
int n;
int main(){
	scanf("%d",&n);
	n++;
	auto t1 = high_resolution_clock::now();
	int len=10*(n+10)/3+10,N=(n+k-1)/k;
	vector<ll> s(len+1,20000000ll);
	ll *q=res;
	for(int i=0;i<=N;++i,++q){
		for(ll j=len,x;j;--j){
			x=s[j]*base+*q;
			s[j]=x%(j<<1|1);
			*q=x/(j<<1|1)*j;
		}
		*q+=s[0]*base;
		s[0]=*q%base;
		*q/=base;
	}
	auto t2 = high_resolution_clock::now();
	auto ms = duration_cast<milliseconds>(t2 - t1).count();
	for(int i=1;i<=N;++i)
		sprintf(ss+(i<<3)-1,"%08lld",res[i]);
	printf("3.");
	ss[n]=0;
	printf("%s\n",ss);
	printf("Time: %lld ms\n", ms);
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	printf("Peak Working Set: %zu bytes\n", pmc.PeakWorkingSetSize);
	system("pause");
	return 0;
}