#include<stdio.h>
#include<windows.h>
void dfs(int n,char a,char b,char c){
    if(n==1){
        printf("%c -> %c\n",a,c);
        return;
    }
    dfs(n-1,a,c,b);
    printf("%c -> %c\n",a,c);
    dfs(n-1,b,a,c);
}
int main(){
    dfs(3,'A','B','C');
    return 0;
}
