#include<bits/stdc++.h>
using namespace std;
template<typename T>
class Array{
private:
    T *a;
    size_t n,cap;
public:
    Array():a(nullptr),n(0),cap(0){}
    Array(size_t val,const T v){
        cap=val<<1;
        n=val;
        a=new T[cap];
        fill(a,a+n,v);
    }
    ~Array(){
        delete[] a;
    }
    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;
    void push_back(T val){
        if(n==cap){
            cap=ceil(cap*1.2);
            T* tmp=new T[cap];
            for(size_t i=0;i<n;++i)
                tmp[i]=move(a[i]);
            delete[] a;
            a=tmp;
        }
        a[n++]=val;
        
    }
    void pop_back(){
        if(n>0) n--;
        else{
            printf("no elements in Array!!!\n");
            exit(-1);
        }
    }
    T& operator [](int val){
        if(val>=n){
            printf("out of range!!!\n");
            exit(-1);
        }
        return a[val];
    }
    const size_t size(){
        return n;
    }
};
int main(){
    Array<int> a;
    for(int i=1;i<=10;++i)
        a.push_back(i);
    for(int i=0;i<10;++i)
        printf("%d ",a[i]);
    printf("\n");
    a.pop_back();
    for(int i=0;i<a.size();++i)
        printf("%d ",a[i]);
    printf("\n");
    Array<double> b(10,114.514);
    for(int i=0;i<b.size();++i)
        printf("%lf ",b[i]);
    printf("\n");
    return 0;
}