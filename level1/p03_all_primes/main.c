#include<stdio.h>
#include<windows.h>
const int maxn = 1e3+10;
int prime[maxn],tot,vis[maxn];
void f(int n){
    for(int i=2;i<=n;++i){
        if(!vis[i]) prime[++tot]=i;
        for(int j=1;j<=tot&&i*prime[j]<=n;++j){
            vis[i*prime[j]]=1;
            if(i%prime[j]==0) break;
        }
    }
}
int main(){
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    f(1000);
    for(int i=1;i<=tot;++i)
        printf("%d\n",prime[i]);
    QueryPerformanceCounter(&end);
    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    printf("Time comsuming: %.6f s\n", elapsed);
    system("pause");
    return 0;
}
