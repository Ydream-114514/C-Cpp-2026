#include<bits/stdc++.h>
using namespace std;
struct Node{
	int val;
	Node *pos[2];
	Node(int v=0):val(v),pos{nullptr,nullptr}{}
};
struct List{
	Node *p[2];
	bool op;
	List():op(0){
		p[0]=p[1]=nullptr;
	}
	void Insert(int v){
		Node *tmp=new Node(v);
		if(!p[op]){
			p[op]=tmp;
			p[op^1]=tmp;
		}else{
			p[op^1]->pos[op^1]=tmp;
			tmp->pos[op]=p[op^1];
			p[op^1]=tmp;
		}
	}
	void Reverse(){
		op^=1;
	}
	int Find(int v){
		Node *pos=p[op];
		int cnt=0;
		while(pos){
			if(pos->val==v) return cnt;
			pos=pos->pos[op^1];
			cnt++;
		}
		return -1;
	}
	int Find_next(int v){
		Node *pos=p[op];
		int cnt=0,t=1;
		while(pos){
			if(pos->val==v){
				if(t) t=0;
				else return cnt;
			}
			pos=pos->pos[op^1];
			cnt++;
		}
		return -1;
	}
	void Print(){
		Node *pos=p[op];
		while(pos){
			printf("%d ",pos->val);
			pos=pos->pos[op^1];
		}
		printf("\n");
	}
	void Del(){
		p[op^1]->pos[op^1]=nullptr;
		free(p[op^1]);
	}
};
int main(){
	srand(time(0));
	List x;
	for(int i=0;i<10;++i)
		x.Insert(rand()%10);
	x.Print();
	x.Reverse();
	x.Print();
	int p=x.Find(5);
	printf("Find 5: %d\n",p);
	int q=x.Find_next(5);
	printf("Find next 5: %d\n",q);
	return 0;
}
