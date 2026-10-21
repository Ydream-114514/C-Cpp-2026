#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define MAX_LEVEL 20
typedef struct Node{
	int val,level;
	struct Node **nxt;
}Node;
Node *new_Node(int val,int level){
    Node *tmp=(Node*)malloc(sizeof(Node));
    if(tmp==NULL){
        printf("Wrong");
        exit(0);
    }
    tmp->val=val;
    tmp->level=level;
    tmp->nxt=(Node**)malloc(sizeof(Node*)*(level+1));
    if(tmp->nxt==NULL){
        printf("Wrong");
        exit(0);
    }
    memset(tmp->nxt,0,sizeof(Node*)*(level+1));
    return tmp;
}
typedef struct List{
	Node *head;
    int level;
}List;
List *new_List(){
    List *tmp=(List*)malloc(sizeof(List));
    if(tmp==NULL){
        printf("Wrong");
        exit(0);
    }
    tmp->level=0;
    tmp->head=new_Node(-0x3f3f3f3f,MAX_LEVEL+1);
    return tmp;
}
int random_level(){
    int level=0;
    while(rand()%2&&level<MAX_LEVEL) ++level;
    return level;
}
void Append(List *lis,int v){
    Node *update[MAX_LEVEL+1];
    Node *p=lis->head,*q=NULL;
    for(int i=lis->level;~i;--i){
        q=p->nxt[i];
        while(q!=NULL&&q->val<v) p=q,q=p->nxt[i];
        update[i]=p;
    }
    if(q&&q->val==v) return;
    int level=random_level();
    if(level>lis->level){
        for(int i=lis->level+1;i<=level;++i)
            update[i]=lis->head;
        lis->level=level;
    }
    q=new_Node(v,level);
    for(int i=level;~i;--i){
        q->nxt[i]=update[i]->nxt[i];
        update[i]->nxt[i]=q;
    }
}
bool Find(List *lis,int v){
    Node *p=lis->head,*q=NULL;
    for(int i=lis->level;~i;--i){
        q=p->nxt[i];
        while(q!=NULL&&q->val<v) p=q,q=p->nxt[i];
    }
    return (q&&q->val==v);
}
void Del(List *lis,int v){
    Node *update[MAX_LEVEL+1];
    Node *p=lis->head,*q=NULL;
    for(int i=lis->level;~i;--i){
        q=p->nxt[i];
        while(q!=NULL&&q->val<v) p=q,q=p->nxt[i];
        update[i]=p;
    }
    if(!q||q->val!=v) return;
    for(int i=lis->level;~i;--i){
        if(update[i]->nxt[i]==q){
            update[i]->nxt[i]=q->nxt[i];
        }
    }
    while(lis->level>0&&lis->head->nxt[lis->level]==NULL) --lis->level;
    free(q->nxt);
    free(q);
}
void Delete(List *lis){
    Node *tmp=lis->head,*p=NULL;
    while(tmp){
        p=tmp->nxt[0];
        free(tmp->nxt);
        free(tmp);
        tmp=p;
    }
    lis->level=0;
}
void Print(List *lis){
    Node *tmp=lis->head->nxt[0];
    while(tmp){
        printf("%d ",tmp->val);
        tmp=tmp->nxt[0];
    }
    puts("");
}