#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
typedef struct Node Node;
typedef struct List List;
Node *new_Node();
List *new_List();
int random_level();
void Append();
bool Find();
void Del();
void Delete();
void Print();
int main(){
    srand(time(0));
	List *x=new_List();
	for(int i=0;i<10;++i)
		Append(x,i*10);
	Print(x);
	Append(x,11);
	Print(x);
	int p=Find(x,50);
	printf("Find 5: %d\n",p);
    Del(x,70);
    Print(x);
    Append(x,35);
    Print(x);
	Delete(x);
	return 0;
}
