#include<stdio.h>
int check(int n){
    for(int i=2;i*i<=n;++i)
        if(n%i==0)
            return 0;
    return 1;
}
int main(){
    while(1){
        int n;
        printf(">>>");
        scanf("%d",&n);
        if(check(n)) printf("Number %d is prime\n",n);
        else printf("Number %d is not prime\n",n);
    }
    
    return 0;
}
