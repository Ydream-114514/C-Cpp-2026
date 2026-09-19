#include<bits/stdc++.h>
#include<windows.h>
using namespace std;
void Print(int x,int y){
    system("cls");
    for(int i=0;i<y;++i)
        printf("\n");
    for(int i=0;i<x;++i)
        printf(" ");
    printf("x");
}
int main(){
    int x=0,y=0,dx=1;
    while(1){
        Print(x,y);
        x=x+dx;
        if(x==0||x==80) dx*=-1;
        Sleep(50);
    }
    return 0;
}