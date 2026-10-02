#include<bits/stdc++.h>
#include<windows.h>
using namespace std;

const int maxn=310;
const int h=41,w=71;
const int POP_SIZE  =100;  // 种群大小
const double CROSS_RATE=0.8;// 交叉率
const double MUT_RATE  =0.01;// 变异率
const int MAX_GEN   =5000; // 最大代数
const int TOURNAMENT_K=3;  // 锦标赛选择规模
const int MAX_LEN=10*(h+w);

int dx[]={0,1,0,-1},dy[]={1,0,-1,0};
int maze[maxn][maxn],fits[maxn][maxn];

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> bit_dist(0,3);
uniform_int_distribution<> path_length(h+w-3,MAX_LEN);
uniform_real_distribution<> prob_dist(0.0,1.0);

void divide(int r1,int c1,int r2,int c2){
    // 区域太小，停止分割
    if(r2-r1<2||c2-c1<2) return;

    int height=r2-r1;
    int width =c2-c1;

    // 决定水平分割还是垂直分割
    bool horizontal;
    if(width>height) horizontal=0;// 宽 > 高，垂直切
    else if(height>width) horizontal=1; // 高 > 宽，水平切
    else horizontal=(prob_dist(gen)<0.5);

    if(horizontal){
        // 在 [r1+1,r2-1] 中找偶数行作为墙
        vector<int> cand;
        for(int r=r1+1;r<=r2-1;++r)
            if(r%2==0) cand.push_back(r);
        if(cand.empty()) return;
        int wr=cand[uniform_int_distribution<>(0,(int)cand.size()-1)(gen)];

        // 在 [c1,c2] 中找奇数列作为潜在缺口
        vector<int> gaps;
        for(int c=c1;c<=c2;++c)
            if(c % 2 == 1) gaps.push_back(c);
        if(gaps.empty()) return;
        shuffle(gaps.begin(),gaps.end(),gen);

        // 留 1~2 个缺口，制造环
        int num_gaps=min((int)gaps.size(),1+(int)(prob_dist(gen)<0.5));

        // 先整行放墙
        for(int c=c1;c<=c2;++c) maze[wr][c]=0;
        // 再开缺口
        for(int k=0;k<num_gaps;++k) maze[wr][gaps[k]]=1;

        // 递归处理上下两个子区域
        divide(r1,c1,wr-1,c2);
        divide(wr+1,c1,r2,c2);
    }else{
        // 垂直分割
        vector<int> cand;
        for(int c=c1+1;c<=c2-1;++c)
            if(c%2==0) cand.push_back(c);
        if(cand.empty()) return;
        int wc=cand[uniform_int_distribution<>(0,(int)cand.size()-1)(gen)];

        vector<int> gaps;
        for(int r=r1;r<=r2;++r)
            if(r%2==1) gaps.push_back(r);
        if(gaps.empty()) return;
        shuffle(gaps.begin(),gaps.end(),gen);

        int num_gaps=min((int)gaps.size(),1+(int)(prob_dist(gen)<0.5));

        for(int r=r1;r<=r2;++r) maze[r][wc]=0;
        for(int k=0;k<num_gaps;++k) maze[gaps[k]][wc]=1;

        divide(r1,c1,r2,wc-1);
        divide(r1,wc+1,r2,c2);
    }
}

void get_recursive_division_maze(){
    // 全部初始化为墙
    memset(maze,0,sizeof(maze));
    // 内部全部打通
    for(int i=1;i<h-1;++i)
        for(int j=1;j<w-1;++j)
            maze[i][j]=1;

    // 递归分割
    divide(1,1,h-2,w-2);
    // 标记起点和终点
    maze[1][1]=2;
    maze[h-2][w-2]=3;
}
void bfs(){
	queue<pair<int,int> > q;
	q.push({h-2,w-2});
	memset(fits,0x3f,sizeof(fits));
	fits[h-2][w-2]=0;
	while(!q.empty()){
		int x=q.front().first,y=q.front().second;
		q.pop();
		vector<int> dirs({0,1,2,3});
		for(auto i:dirs){
			int tx=x+dx[i],ty=y+dy[i];
			if(0<=tx&&tx<h-1&&0<=ty&&ty<w-1&&maze[tx][ty]&&fits[tx][ty]==0x3f3f3f3f){
				fits[tx][ty]=fits[x][y]+1;
				q.push({tx,ty});
			}
		}
	}
}
void Init_maze(){
	get_recursive_division_maze();
	bfs();
}

typedef vector<int> Ind;

Ind init_Ind(){
    Ind a(path_length(gen));
	int lst=-1;
    for(auto &it:a){
		while(it=bit_dist(gen),(it==1&&lst==3)||(it==3&&lst==1)||(it==2&&lst==0)||(it==0&&lst==2));
		lst=it;
	}
    return a;
}

int fitness(const Ind &a){
	int x=1,y=1,score=h*w*2,step=0;
	for(const auto &it:a){
		int tx=x+dx[it],ty=y+dy[it];
		if(0<=tx&&tx<h-1&&0<=ty&&ty<w-1&&maze[tx][ty]) x=tx,y=ty,step++;
		else break;
		if(maze[x][y]==3){
			score+=500000;
			break;
		}
		if(maze[x][y]==2) score-=10;
	}
	score-=fits[x][y]*20+step*2;
	return score;
}

Ind select(const vector<Ind>& p,const vector<int>& fit){
	uniform_int_distribution<> idx_dist(0,p.size()-1);
	int best=idx_dist(gen);
	for(int i=1;i<TOURNAMENT_K;++i){
		int id=idx_dist(gen);
		if(fit[best]<fit[id])
			best=id;
	}
	return p[best];
}

void crossover(Ind &a,Ind &b){
	if(prob_dist(gen)<CROSS_RATE){
		uniform_int_distribution<> A(0,a.size()-1),B(0,b.size()-1);
		int p1=A(gen),p2=B(gen);
		Ind tmp1(a.begin()+p1,a.end()),tmp2(b.begin()+p2,b.end());
		a.erase(a.begin()+p1,a.end());
		b.erase(b.begin()+p2,b.end());
		a.insert(a.end(),make_move_iterator(tmp2.begin()),make_move_iterator(tmp2.end()));
		b.insert(b.end(),make_move_iterator(tmp1.begin()),make_move_iterator(tmp1.end()));
		if(a.size()>MAX_LEN) a.resize(MAX_LEN);
		if(b.size()>MAX_LEN) b.resize(MAX_LEN);
	}
}

void mutate(Ind &a){
	for(auto &it:a)
		if(prob_dist(gen)<MUT_RATE)
			it=bit_dist(gen);
}

void print(const Ind &p){
	int copy_maze[maxn][maxn];
	memcpy(copy_maze,maze,sizeof(maze));
	int x=1,y=1;
	for(const auto &it:p){
		int tx=x+dx[it],ty=y+dy[it];
		if(copy_maze[x][y]==3) break;
		if(0<=tx&&tx<h-1&&0<=ty&&ty<w-1&&copy_maze[tx][ty]){
			x=tx;
			y=ty;
			if(copy_maze[x][y] != 3) copy_maze[x][y]=4;
		} else{
			break;
		}
	}
	char mp[]="# ST*";
	for(int i=0;i<h;++i){
		for(int j=0;j<w;++j)
			putchar(mp[copy_maze[i][j]]);
		putchar('\n');
	}
	putchar('\n');
}

bool check(const Ind &a){
    int x=1,y=1;
	for(const auto &it:a){
		int tx=x+dx[it],ty=y+dy[it];
		if(0<=tx&&tx<h-1&&0<=ty&&ty<w-1&&maze[tx][ty]) x=tx,y=ty;
		else return 0;
		if(maze[x][y]==3) return 1;
	}
    return 0;
}

int main(){
	SetConsoleOutputCP(CP_UTF8);
	clock_t start=clock();
	Init_maze();
    // 初始化种群
    vector<Ind> pop(POP_SIZE);
    for(auto& ind:pop)
		ind=init_Ind();
    Ind global_best;
    int global_best_fit=-1;
    int gen_id=0;
    cout<<"初始迷宫："<<endl;
    print(Ind());
    cout<<"初始种群评估中..."<<endl;
    for(;!(check(global_best)||gen_id+1>=MAX_GEN);++gen_id){
        // 评估当前种群
        vector<int> fits(POP_SIZE);
        for(int i=0;i<POP_SIZE;++i)
            fits[i]=fitness(pop[i]);
        // 当前代最优
        int best_idx=max_element(fits.begin(),fits.end())-fits.begin();
        if(fits[best_idx]>global_best_fit){
            global_best_fit=fits[best_idx];
            global_best=pop[best_idx];
        }
        // 生成下一代
        vector<Ind> new_pop;
        new_pop.reserve(POP_SIZE);
        // 精英保留：直接保留当前代最优个体
		// 按适应度排序，取前 6 名
		vector<int> order(POP_SIZE);
		iota(order.begin(),order.end(),0);
		sort(order.begin(),order.end(),[&](int a,int b){return fits[a]>fits[b];});
		for(int k=0,cnt=0;cnt<6;++k){
            if(new_pop.empty()||new_pop.back()!=pop[order[k]]){
                new_pop.push_back(pop[order[k]]);
                cnt++;
            }
        }
        while(new_pop.size()<POP_SIZE){
            Ind p1=select(pop,fits);
            Ind p2=select(pop,fits);
            Ind c1=p1;
            Ind c2=p2;
            crossover(c1,c2);
            mutate(c1);
            mutate(c2);
            new_pop.push_back(c1);
            if(new_pop.size()<POP_SIZE){
                new_pop.push_back(c2);
            }
        }
        pop=move(new_pop);
		if((gen_id+1)%50==0){
			cout<<"代数："<<gen_id+1<<endl;
			cout<<"适应度："<<global_best_fit<<endl;
			print(global_best);
		}
    }
    // 最后再评估一次，更新历史最优
    vector<int> fits(POP_SIZE);
    for(int i=0;i<POP_SIZE;++i)
        fits[i]=fitness(pop[i]);
    int best_idx=max_element(fits.begin(),fits.end())-fits.begin();
    if(fits[best_idx] > global_best_fit){
        global_best_fit=fits[best_idx];
        global_best=pop[best_idx];
    }
    double elapsed_ms=(double)(clock()-start)/CLOCKS_PER_SEC*1000.0;
    cout<<"达到终止条件，停止进化。"<<endl;
    cout<<"总代数："<<gen_id+1<<endl;
    cout<<"最优解: "<<endl;
	if(global_best.empty()){
		cout<<"未找到有效路径。"<<endl;
	}else{
		print(global_best);
		cout<<"适应度："<<global_best_fit<<endl;
		cout<<"运行时间："<<elapsed_ms<<" ms"<<endl;
	}
	system("pause");
    return 0;
}