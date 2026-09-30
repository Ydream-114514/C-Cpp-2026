#include<bits/stdc++.h>
using namespace std;
class Animal{
public:
    virtual ~Animal()=default;
    virtual void jiao() const=0;
};
class Dog:public Animal{
public:
    void jiao()const override{
        cout<<"大狗叫叫叫"<<endl;
    }
};
class Cat:public Animal{
public:
    void jiao()const override{
        cout<<"哈基米南北绿豆"<<endl;
    }
};
class bird:public Animal{
public:
    void jiao()const override{
        cout<<"咕咕嘎嘎!"<<endl;
    }
};
class Wolfdog:public Animal{
public:
    void jiao()const override{
        cout<<"5瓦"<<endl;
    }
};
class Zoo{
    vector<unique_ptr<Animal> > lis;
public:
    void add_animal(unique_ptr<Animal> a){
        lis.push_back(move(a));
    }
    void call_animal()const{
        for(const auto& it:lis)
            it->jiao();
    }
};
int main(){
    Zoo a;
    a.add_animal(make_unique<Dog>());
    a.add_animal(make_unique<Cat>());
    a.call_animal();
    cout<<endl;
    a.add_animal(make_unique<bird>());
    a.add_animal(make_unique<Wolfdog>());
    a.call_animal();
    return 0;
}