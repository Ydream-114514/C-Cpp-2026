#include "common.h"
#include "ai.h"

static constexpr int One_step_kill=3;
static constexpr int One_step_kill_model[][8]={
    {5,0,1,1,1,1,0},
    {5,1,0,1,1,1,1},
    {5,1,1,0,1,1,2},
};

static constexpr int Two_steps_kill=4;
static constexpr int Two_steps_kill_model[][9]={
    {7,0,0,1,1,1,0,0,1},
    {6,0,0,1,1,1,0,1},
    {6,0,1,1,1,0,0,4},
    {6,0,1,1,0,1,0,3},
};

bool checkWin(const vector<vector<int>>&,int,int,int);

Action AI::turn(const vector<vector<int>> &board)const{
    if(!thinking_){
        thinking_=1;
        auto self=shared_from_this();
        future_=async(launch::async,[self,board](){
            return self->computeMove(board);
        });
        return {ACT_JUMP,-1,-1};
    }
    if(future_.wait_for(0ms)==future_status::ready){
        auto result=future_.get();
        thinking_=0;
        return result;
    }
    return {ACT_JUMP,-1,-1};
}
bool AI::check_model(const vector<vector<int>> &mp,const int &r,const int &c,const int &dr,const int &dc,const int *_model,int color,bool rev)const{
    int len=_model[0];
    if(in(r+len*dr,c+len*dc)){
        if(rev){
            for(int i=0;i<len;++i)
                if(mp[r+i*dr][c+i*dc]!=_model[len-i]*color)
                    return 0;
            return 1;
        }else{
            for(int i=0;i<len;++i)
                if(mp[r+i*dr][c+i*dc]!=_model[i+1]*color)
                    return 0;
            return 1;
        }
    }
    return 0;
}
Score_type AI::calc(const vector<vector<int>>& mp,int color)const{
    Score_type score=0;
    auto patternScore=[&](int cnt,int lcnt,int rcnt) -> Score_type {
        if(cnt>=5) return Win_score;
        switch(cnt){
            case 4:return (lcnt&&rcnt)?Full_Four:Not_Full_Four;
            case 3:return (lcnt>=1&&rcnt>=1)?Full_Three:Not_Full_Three;
            case 2:return (lcnt+rcnt>=3)?Full_Two:Not_Full_Two;
            default:return 0;
        }
    };
    static const int dirs[4][2]={{0,1},{1,0},{1,1},{1,-1}};
    for(const auto& [dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c){
                if(mp[r][c]!=color) continue;
                int pr=r-dr, pc=c-dc;
                if(in(pr,pc)&&mp[pr][pc]==color) continue;
                int cnt=0;
                int nr=r, nc=c;
                while(in(nr,nc)&&mp[nr][nc]==color){
                    ++cnt;
                    nr+=dr;
                    nc+=dc;
                }
                int rcnt=0;
                int rr=nr, rc=nc;
                while(in(rr,rc)&&mp[rr][rc]==EMPTY){
                    ++rcnt;
                    rr+=dr;
                    rc+=dc;
                }
                int lcnt=0;
                int lr=r-dr, lc=c-dc;
                while(in(lr,lc)&&mp[lr][lc]==EMPTY){
                    ++lcnt;
                    lr-=dr;
                    lc-=dc;
                }
                score+=patternScore(cnt,lcnt,rcnt);
            }
    return score;
}
Score_type AI::get_score(const vector<vector<int>>&mp)const{
    auto score1=calc(mp,color);
    auto score2=calc(mp,3-color);
    if(score1>=Win_score) return Win_score;
    if(score2>=Win_score) return -Win_score;
    return score1-score2*1.2f;
}
bool AI::check(const vector<vector<int>>&mp,int x,int y)const{
    if(mp[x][y]!=EMPTY) return 0;
    for(int k=-pace;k<=pace;++k)
        for(int l=-pace;l<=pace;++l)
            if(in(x+k,y+l)&&mp[x+k][y+l]!=EMPTY)
                return 1;
    return 0;
}
Action AI::Rand_step(const vector<vector<int>>&board)const{
    vector<pair<int,int>> c;
    for(int i=0;i<row;++i)
        for(int j=0;j<col;++j)
            if(check(board,i,j))
                c.emplace_back(i,j);
    auto [x,y]=c[randInt(0,c.size()-1)];
    return{ACT_MOVE,x,y};
}
Score_type AI::alpha_beta(vector<vector<int>>&mp,Score_type alpha,Score_type beta,int dep,bool is_max)const{
    if(dep==depth) return get_score(mp);
    vector<tuple<int,int,Score_type>> st;
    if(is_max){
        for(int i=0;i<row;++i)
            for(int j=0;j<col;++j)
                if(check(mp,i,j)){
                    mp[i][j]=color;
                    st.emplace_back(i,j,get_score(mp));
                    mp[i][j]=EMPTY;
                }
        int k=min(K,(int)st.size());
        static const auto cmp=[](const tuple<int,int,Score_type> &a,const tuple<int,int,Score_type> &b){
            return get<2>(a)>get<2>(b);
        };
        nth_element(st.begin(),st.begin()+k,st.end(),cmp);
        sort(st.begin(),st.begin()+k,cmp);
        for(int l=0;l<k;++l){
            int i,j;
            tie(i,j,ignore)=st[l];
            mp[i][j]=color;
            if(checkWin(mp,i,j,color)){
                mp[i][j]=EMPTY;
                return Win_score-dep;
            }else alpha=max(alpha,alpha_beta(mp,alpha,beta,dep+1,0));
            mp[i][j]=EMPTY;
            if(alpha>=beta) return alpha;
        }
        return alpha;
    }else{
        for(int i=0;i<row;++i)
            for(int j=0;j<col;++j)
                if(check(mp,i,j)){
                    mp[i][j]=3-color;
                    st.emplace_back(i,j,get_score(mp));
                    mp[i][j]=EMPTY;
                }
        int k=min(K,(int)st.size());
        static const auto cmp=[](const tuple<int,int,Score_type> &a,const tuple<int,int,Score_type> &b){
            return get<2>(a)<get<2>(b);
        };
        nth_element(st.begin(),st.begin()+k,st.end(),cmp);
        sort(st.begin(),st.begin()+k,cmp);
        for(int l=0;l<k;++l){
            int i,j;
            tie(i,j,ignore)=st[l];
            mp[i][j]=3-color;
            if(checkWin(mp,i,j,3-color)){
                mp[i][j]=EMPTY;
                return dep-Win_score;
            }else beta=min(beta,alpha_beta(mp,alpha,beta,dep+1,1));
            mp[i][j]=EMPTY;
            if(alpha>=beta) return beta;
        }
        return beta;
    }
}
Action AI::a_b_search(const vector<vector<int>>&board)const{
    auto mp=board;
    vector<tuple<int,int,Score_type>> st;
    for(int i=0;i<row;++i)
        for(int j=0;j<col;++j)
            if(check(mp,i,j)){
                mp[i][j]=color;
                st.emplace_back(i,j,get_score(mp));
                mp[i][j]=EMPTY;
            }
    int k=min(K,(int)st.size());
    nth_element(st.begin(),st.begin()+k,st.end(),
    [](const tuple<int,int,Score_type> &a,const tuple<int,int,Score_type> &b){
        return get<2>(a)>get<2>(b);
    });
    int x=-1,y=-1;
    Score_type max_score=-1e18;
    for(int l=0,i,j;l<k;++l){
        tie(i,j,ignore)=st[l];
        mp[i][j]=color;
        if(checkWin(mp,i,j,color)){
            mp[i][j]=EMPTY;
            return {ACT_MOVE,i,j};
        }
        Score_type score=alpha_beta(mp,-1e18,1e18,0,0);
        mp[i][j]=EMPTY;
        if(score>max_score){
            max_score=score;
            x=i;
            y=j;
        }
    }
    if(x==-1) return Rand_step(board);
    return{ACT_MOVE,x,y};
}
Action AI::search(vector<vector<int>>&mp)const{
    static const int dirs[4][2]={{0,1},{1,0},{1,1},{1,-1}};
    cerr<<"start check my one_step_kill"<<endl;
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c){
                for(int i=0;i<One_step_kill;++i){
                    if(check_model(mp,r,c,dr,dc,One_step_kill_model[i],color,0)){
                        int cnt=One_step_kill_model[i][One_step_kill_model[i][0]+1];
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                    if(check_model(mp,r,c,dr,dc,One_step_kill_model[i],color,1)){
                        int cnt=One_step_kill_model[i][0]-One_step_kill_model[i][One_step_kill_model[i][0]+1]-1;
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                }
            }
    cerr<<"not find my one_step_kill"<<endl;
    cerr<<"start check duishou's one_step_kill"<<endl;
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c){
                for(int i=0;i<One_step_kill;++i){
                    if(check_model(mp,r,c,dr,dc,One_step_kill_model[i],3-color,0)){
                        int cnt=One_step_kill_model[i][One_step_kill_model[i][0]+1];
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                    if(check_model(mp,r,c,dr,dc,One_step_kill_model[i],3-color,1)){
                        int cnt=One_step_kill_model[i][0]-One_step_kill_model[i][One_step_kill_model[i][0]+1]-1;
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                }
            }
    cerr<<"not find duishou's one_step_kill"<<endl;
    cerr<<"start check my two_steps_kill"<<endl;
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c){
                if(check_model(mp,r,c,dr,dc,Two_steps_kill_model[0],color,0)){
                    mp[r+dr][c+dc]=color;
                    Score_type score1=alpha_beta(mp,-1e18,1e18,0,0);
                    mp[r+dr][c+dc]=EMPTY;
                    mp[r+dr*5][c+dc*5]=color;
                    Score_type score2=alpha_beta(mp,-1e18,1e18,0,0);
                    mp[r+dr*5][c+dc*5]=EMPTY;
                    return score1>score2?Action{ACT_MOVE,r+dr,c+dc}:Action{ACT_MOVE,r+dr*5,c+dc*5};
                }
                for(int i=1;i<Two_steps_kill;++i){
                    if(check_model(mp,r,c,dr,dc,Two_steps_kill_model[i],color,0)){
                        int cnt=Two_steps_kill_model[i][Two_steps_kill_model[i][0]+1];
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                    if(check_model(mp,r,c,dr,dc,Two_steps_kill_model[i],color,1)){
                        int cnt=Two_steps_kill_model[i][0]-Two_steps_kill_model[i][Two_steps_kill_model[i][0]+1]-1;
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                }
            }
    cerr<<"not find my one_step_kill"<<endl;
    cerr<<"start check duishou's two_steps_kill"<<endl;
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c){
                if(check_model(mp,r,c,dr,dc,Two_steps_kill_model[0],3-color,0)){
                    mp[r+dr][c+dc]=color;
                    Score_type score1=alpha_beta(mp,-1e18,1e18,0,0);
                    mp[r+dr][c+dc]=EMPTY;
                    mp[r+dr*5][c+dc*5]=color;
                    Score_type score2=alpha_beta(mp,-1e18,1e18,0,0);
                    mp[r+dr*5][c+dc*5]=EMPTY;
                    return score1>score2?Action{ACT_MOVE,r+dr,c+dc}:Action{ACT_MOVE,r+dr*5,c+dc*5};
                }
                for(int i=1;i<Two_steps_kill;++i){
                    if(check_model(mp,r,c,dr,dc,Two_steps_kill_model[i],3-color,0)){
                        int cnt=Two_steps_kill_model[i][Two_steps_kill_model[i][0]+1];
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                    if(check_model(mp,r,c,dr,dc,Two_steps_kill_model[i],3-color,1)){
                        int cnt=Two_steps_kill_model[i][0]-Two_steps_kill_model[i][Two_steps_kill_model[i][0]+1]-1;
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                }
            }
    cerr<<"start alpha_beta_search"<<endl;
    return a_b_search(mp);
}
Action AI::computeMove(vector<vector<int>> board)const{
    int cnt=0;
    for(const auto &i:board)
        for(const auto &j:i)
            if(j==3-color)
                ++cnt;
    if(cnt==0){
        if(board[row/2][col/2]==EMPTY) return {ACT_MOVE,row/2,col/2};
        else{
            for(int dr=-1;dr<=1;++dr)
                for(int dc=-1;dc<=1;++dc){
                    int r=row/2+dr,c=col/2+dc;
                    if(in(r,c)&&board[r][c]==EMPTY)
                        return {ACT_MOVE,r,c};
                }
            for(int r=0;r<row;++r)
                for(int c=0;c<col;++c)
                    if(board[r][c]==EMPTY)
                        return {ACT_MOVE,r,c};
            return {ACT_RESIGN,-1,-1};
        }
    }else if(cnt==1) return Rand_step(board);
    else return search(board);
}