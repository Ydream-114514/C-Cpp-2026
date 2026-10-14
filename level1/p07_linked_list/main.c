#include<stdio.h>
#include<stdlib.h>
typedef struct Node{
	int val;
	struct Node *nxt;
}Node;
Node *new_Node(int val){
	Node *tmp=(Node *)malloc(sizeof(Node));
	if(tmp==NULL){
		puts("Fail");
		exit(0);
	}
	tmp->val=val;
	tmp->nxt=NULL;
	return tmp;
}
typedef struct{
	Node *head;
}List;
List *new_List(){
	List *tmp=(List *)malloc(sizeof(List));
	if(tmp==NULL){
		puts("Fail");
		exit(0);
	}
	tmp->head=NULL;
	return tmp;
}
void Append(List *lis,int v){
	if(lis->head==NULL){
		lis->head=new_Node(v);
		return;
	}
	Node *tmp=lis->head;
	while(tmp->nxt!=NULL) tmp=tmp->nxt;
	tmp->nxt=new_Node(v);
}
void Reverse(List *lis){
	Node *p=lis->head,*p_front=NULL,*p_nxt=lis->head->nxt;
	while(p_nxt!=NULL){
		p->nxt=p_front;
		p_front=p;
		p=p_nxt;
		p_nxt=p_nxt->nxt;
	}
	p->nxt=p_front;
	lis->head=p;
}
int Find_value(List *lis,int v){
	Node *tmp=lis->head;
	int cnt=0;
	while(tmp!=NULL&&tmp->val!=v) tmp=tmp->nxt,++cnt;
	return tmp?cnt:-1;
}
int Find_next(List *lis,int v){
	Node *tmp=lis->head;
	int cnt=0;
	while(tmp!=NULL&&tmp->val!=v) tmp=tmp->nxt,++cnt;
	tmp=tmp->nxt;
	while(tmp!=NULL&&tmp->val!=v) tmp=tmp->nxt,++cnt;
	return tmp?cnt:-1;
}
void Print(List *lis){
	Node *tmp=lis->head;
	while(tmp!=NULL){
		printf("%d ",tmp->val);
		tmp=tmp->nxt;
	}
	puts("");
}
void Del(List *lis){
	if(lis->head==NULL){
		puts("Wrong!!!");
		exit(0);
	}
	if(lis->head->nxt==NULL){
		free(lis->head);
		lis->head=NULL;
		return;
	}
	Node *tmp=lis->head;
	while(tmp->nxt->nxt!=NULL) tmp=tmp->nxt;
	free(tmp->nxt);
}
void Delete(List *lis){
	Node *tmp=lis->head,*p=NULL;
	while(tmp->nxt->nxt!=NULL){
		p=tmp->nxt;
		free(tmp);
		tmp=p;
	}
	lis->head=NULL;
}
int main(){
	List *x=new_List();
	for(int i=0;i<10;++i)
		Append(x,i);
	Print(x);
	Reverse(x);
	Print(x);
	Append(x,11);
	Print(x);
	Reverse(x);
	Print(x);
	int p=Find_value(x,5);
	printf("Find 5: %d\n",p);
	int q=Find_next(x,5);
	printf("Find next 5: %d\n",q);
	Delete(x);
	system("pause");
	return 0;
}
