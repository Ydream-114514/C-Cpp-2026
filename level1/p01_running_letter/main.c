#include<stdio.h>
#include<windows.h>
using namespace std;
void Print(int x){
    system("cls");
    for(int i=0;i<x;++i)
        printf(" ");
    printf("x");
}
int main(){
    int x=0,dx[]={1,-1},op=1;
    while(1){
        Print(x);
        x=x+dx[op];
        op^=(x==0||x==80);
        Sleep(50);
    }
    return 0;
}
