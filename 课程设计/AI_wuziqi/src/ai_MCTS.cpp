#include "ai_MCTS.h"
#include "player_factory.h"

static PlayerRegistrar<AI_MCTS,AI_searchConfig> reg_ai_MCTS(PlayerType::AI_MCTS);

namespace{
    bool check(const Board &mp,int x,int y){
        if(mp[x][y]!=Chess_status::Empty) return 0;
        for(int k=-2;k<=2;++k)
            for(int l=-2;l<=2;++l)
                if(mp.in(x+k,y+l)&&mp[x+k][y+l]!=Chess_status::Empty)
                    return 1;
        return 0;
    }
    struct Node{
        Node *fa=nullptr;
        int vis=0,r,c;
        bool isexpand=0;
        int tot=0;
        int val=0;
        double C=1.0,U=0;
        vector<unique_ptr<Node>> son;
        Node(){};
        Node(Node *fa,int r,int c):fa(fa),r(r),c(c){}
        void expansion(const Board &mp){
            son.clear();
            int i=0;
            for(const auto &row:mp){
                for(int j=0;j<(int)row.size();++j)
                    if(check(mp,i,j))
                        son.emplace_back(make_unique<Node>(this,i,j));
                ++i;
            }
            tot=(int)son.size();
        }
        Node *get_son(){
            if(son.empty()) return nullptr;
            for(auto &it:son)
                if(it->vis==0)
                    return it.get();
            double logn=log((double)max(1,vis));
            for(auto &it:son)
                it->U=-1.0*it->val/it->vis+C*sqrt(logn/it->vis);
            return max_element(son.begin(),son.end(),[](const unique_ptr<Node>&a,const unique_ptr<Node>&b){return a->U<b->U;})->get();
        }
    };

    bool winAt(const Board &mp,int r,int c,int color){
        static const int dirs[4][2]={{0,1},{1,0},{1,1},{1,-1}};
        for(const auto &[dr,dc]:dirs){
            int cnt=1;
            for(int s=1;s<5;++s){
                int nr=r+dr*s,nc=c+dc*s;
                if(!mp.in(nr,nc)||mp[nr][nc]!=color) break;
                ++cnt;
            }
            for(int s=1;s<5;++s){
                int nr=r-dr*s,nc=c-dc*s;
                if(!mp.in(nr,nc)||mp[nr][nc]!=color) break;
                ++cnt;
            }
            if(cnt>=5) return true;
        }
        return false;
    }

    int existingWinner(const Board &mp){
        int r=0;
        for(const auto &row:mp){
            for(int c=0;c<(int)row.size();++c)
                if(row[c]!=Chess_status::Empty&&winAt(mp,r,c,row[c]))
                    return row[c];
            ++r;
        }
        return 0;
    }

    pair<int,int> boardSize(const Board &mp){
        int R=0,C=0;
        for(const auto &row:mp){
            ++R;
            C=max(C,(int)row.size());
        }
        return {R,C};
    }

    pair<int,int> findWinningMove(Board &mp,int who,int R,int C){
        for(int r=0;r<R;++r)
            for(int c=0;c<C;++c){
                if(mp[r][c]!=Chess_status::Empty||!check(mp,r,c)) continue;
                mp[r][c]=who;
                bool win=winAt(mp,r,c,who);
                mp[r][c]=Chess_status::Empty;
                if(win) return {r,c};
            }
        return {-1,-1};
    }
}

int Simulation(Board &mp,int color,int max_dep=60){
    int winner=existingWinner(mp);
    if(winner) return winner==color?1:-1;

    auto [R,C]=boardSize(mp);
    vector<char> inCan(R*C,0);
    vector<pair<int,int>> can;
    for(int r=0;r<R;++r)
        for(int c=0;c<C;++c)
            if(check(mp,r,c)){
                can.emplace_back(r,c);
                inCan[r*C+c]=1;
            }

    int side=color;
    for(int dep=0;dep<max_dep;++dep){
        if(can.empty()) return 0;

        int idx=randInt(0,(int)can.size()-1);
        auto [x,y]=can[idx];
        can[idx]=can.back();
        can.pop_back();
        mp[x][y]=side;

        for(int dr=-2;dr<=2;++dr)
            for(int dc=-2;dc<=2;++dc){
                int nr=x+dr,nc=y+dc;
                if(mp.in(nr,nc)&&mp[nr][nc]==Chess_status::Empty&&!inCan[nr*C+nc]){
                    inCan[nr*C+nc]=1;
                    can.emplace_back(nr,nc);
                }
            }

        if(winAt(mp,x,y,side)) return side==color?1:-1;
        side=3-side;
    }
    return 0;
}
pair<int,int> AI_MCTS::MCTS(Board &mp)const{
    auto root=make_unique<Node>(nullptr,-1,-1);
    root->isexpand=true;
    root->expansion(mp);
    if(root->son.empty()) return {-1,-1};

    auto start=chrono::steady_clock::now();
    for(int i=0;;++i){
        if((i&63)==0 &&
           chrono::steady_clock::now()-start>chrono::milliseconds(2000))
            break;

        Board tmp=mp;
        int cr=color;
        auto rt=root.get();
        vector<Node*> path{rt};

        while(rt->isexpand&&!rt->son.empty()){
            rt=rt->get_son();
            tmp[rt->r][rt->c]=cr;
            cr=3-cr;
            path.emplace_back(rt);
        }

        if(!rt->isexpand){
            rt->isexpand=true;
            bool terminal=winAt(tmp,rt->r,rt->c,3-cr);
            if(!terminal) rt->expansion(tmp);
        }

        int val=Simulation(tmp,cr);
        for(auto it=path.rbegin();it!=path.rend();++it){
            (*it)->vis++;
            (*it)->val+=val;
            val=-val;
        }
    }

    auto res=max_element(root->son.begin(),root->son.end(),
        [](const unique_ptr<Node>&a,const unique_ptr<Node>&b){return a->vis<b->vis;})->get();
    return {res->r,res->c};
}
Action AI_MCTS::computeMove(Board mp)const{
    auto res=suansha(mp);
    if(res.type!=ACT_JUMP) return res;
    auto [R,C]=boardSize(mp);
    if(mp.count()==0) return {ACT_MOVE,R/2,C/2};

    auto win=findWinningMove(mp,color,R,C);
    if(win.first!=-1) return {ACT_MOVE,win.first,win.second};
    auto block=findWinningMove(mp,3-color,R,C);
    if(block.first!=-1) return {ACT_MOVE,block.first,block.second};

    auto [r,c]=MCTS(mp);
    if(mp.in(r,c)&&mp[r][c]==Chess_status::Empty) return {ACT_MOVE,r,c};

    for(int i=0;i<R;++i)
        for(int j=0;j<C;++j)
            if(mp[i][j]==Chess_status::Empty) return {ACT_MOVE,i,j};
    return {ACT_JUMP,-1,-1};
}