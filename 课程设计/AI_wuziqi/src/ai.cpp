#include "common.h"
#include "ai.h"

const Score_type Win_score     =1e8;
const Score_type Full_Four     =1e6;
const Score_type Full_Three    =1e5;
const Score_type Full_Two      =1e4;
const Score_type Not_Full_Four =3e5;
const Score_type Not_Full_Three=4e4;
const Score_type Not_Full_Two  =2e3;
const Score_type Die_Four      =4e2;
const Score_type Die_Three     =3e2;
const Score_type Die_Two       =3e1;

static const int dirs[4][2]={{0,1},{1,0},{1,1},{1,-1}};

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
bool AI::patternScore(int cnt,int lcnt,int rcnt)const{
    if(cnt>=5) return Win_score;
    if(lcnt>rcnt) swap(lcnt,rcnt);
    switch(cnt){
        case 4:return lcnt?Full_Four:(rcnt?Not_Full_Four:Die_Four);
        case 3:return lcnt?(rcnt>=2?Full_Three:Not_Full_Three):(rcnt?Not_Full_Three:Die_Three);
        case 2:return lcnt>=2?Full_Two:(lcnt==1?(rcnt>=3?Full_Two:Not_Full_Two):(min(rcnt,3)+1)*Die_Two);
        default:return 0;
    }
};
Score_type AI::calc(const vector<vector<int>>& mp,int color)const{
    Score_type score=0;
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
                while(in(rr,rc)&&mp[rr][rc]==Chess_status::Empty){
                    ++rcnt;
                    rr+=dr;
                    rc+=dc;
                }
                int lcnt=0;
                int lr=r-dr, lc=c-dc;
                while(in(lr,lc)&&mp[lr][lc]==Chess_status::Empty){
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
Score_type AI::local_score(const vector<vector<int>>&mp,int r,int c,int me)const{
    Score_type total=0;
    for(auto& [dr,dc]:dirs){
        int cnt=1,lcnt=0,rcnt=0;
        for(int s=1;s<=4;++s){
            int nr=r+dr*s,nc=c+dc*s;
            if(!in(nr,nc)) break;
            if(mp[nr][nc]==me) ++cnt;
            else{
                if(mp[nr][nc]==Chess_status::Empty) ++rcnt;
                break;
            }
        }
        for(int s=1;s<=4;++s){
            int nr=r-dr*s,nc=c-dc*s;
            if(!in(nr,nc)) break;
            if(mp[nr][nc]==me) ++cnt;
            else{
                if(mp[nr][nc]==Chess_status::Empty) ++lcnt;
                break;
            }
        }
        total+=patternScore(cnt,lcnt,rcnt);
    }
    return total;
}
bool AI::check(const vector<vector<int>>&mp,int x,int y)const{
    if(mp[x][y]!=Chess_status::Empty) return 0;
    for(int k=-pace;k<=pace;++k)
        for(int l=-pace;l<=pace;++l)
            if(in(x+k,y+l)&&mp[x+k][y+l]!=Chess_status::Empty)
                return 1;
    return 0;
}
vector<tuple<int,int,Score_type>> AI::collection(vector<vector<int>>&mp,int me,bool flag)const{
    vector<tuple<int,int,Score_type>> st;
     for(int i=0;i<row;++i)
        for(int j=0;j<col;++j)
            if(check(mp,i,j)){
                mp[i][j]=me;
                st.emplace_back(i,j,local_score(mp,i,j,me)*2.f+get_score(mp)*0.5f);
                mp[i][j]=Chess_status::Empty;
            }
    int k=min(K,(int)st.size());
    static const auto cmp1=[](const tuple<int,int,Score_type> &a,const tuple<int,int,Score_type> &b){
        return get<2>(a)>get<2>(b);
    };
    static const auto cmp2=[](const tuple<int,int,Score_type> &a,const tuple<int,int,Score_type> &b){
        return get<2>(a)<get<2>(b);
    };
    if(flag){
        nth_element(st.begin(),st.begin()+k,st.end(),cmp1);
        st.resize(k);
        sort(st.begin(),st.end(),cmp1);
    }else{
        nth_element(st.begin(),st.begin()+k,st.end(),cmp2);
        st.resize(k);
        sort(st.begin(),st.end(),cmp2);
    }
    return st;
}
Score_type AI::alpha_beta(vector<vector<int>>&mp,Score_type alpha,Score_type beta,int dep,bool is_max)const{
    if(dep==depth) return get_score(mp);
    int me=is_max?color:3-color;
    auto st=collection(mp,me,is_max);
    if(is_max){
        for(auto [i,j,ignore]:st){
            mp[i][j]=me;
            if(checkWin(mp,i,j,me)){
                mp[i][j]=Chess_status::Empty;
                return Win_score-dep;
            }else alpha=max(alpha,alpha_beta(mp,alpha,beta,dep+1,0));
            mp[i][j]=Chess_status::Empty;
            if(alpha>=beta) return alpha;
        }
        return alpha;
    }else{
        for(auto [i,j,ignore]:st){
            mp[i][j]=me;
            if(checkWin(mp,i,j,me)){
                mp[i][j]=Chess_status::Empty;
                return dep-Win_score;
            }else beta=min(beta,alpha_beta(mp,alpha,beta,dep+1,1));
            mp[i][j]=Chess_status::Empty;
            if(alpha>=beta) return beta;
        }
        return beta;
    }
}
Action AI::a_b_search(const vector<vector<int>>&board)const{
    auto mp=board;
    auto st=collection(mp,color,1);
    int x=-1,y=-1;
    Score_type max_score=-1e18;
    for(auto [i,j,ignore]:st){
        mp[i][j]=color;
        if(checkWin(mp,i,j,color)){
            mp[i][j]=Chess_status::Empty;
            return {ACT_MOVE,i,j};
        }
        Score_type score=alpha_beta(mp,-1e18,1e18,0,0);
        mp[i][j]=Chess_status::Empty;
        if(score>max_score){
            max_score=score;
            x=i;y=j;
        }
    }
    if(x==-1) return Rand_step(board);
    return{ACT_MOVE,x,y};
}

struct model{
    vector<int> model;
    vector<pair<int,vector<int>>> pos;
};

bool AI::check_model(const vector<vector<int>> &mp,int r,int c,int dr,int dc,const vector<int> &_model,int color,bool rev)const{
    int len=_model.size();
    if(in(r,c)&&in(r+(len-1)*dr,c+(len-1)*dc)){
        if(rev){
            for(int i=0;i<len;++i)
                if(mp[r+i*dr][c+i*dc]!=_model[len-i-1]*color)
                    return 0;
            return 1;
        }else{
            for(int i=0;i<len;++i)
                if(mp[r+i*dr][c+i*dc]!=_model[i]*color)
                    return 0;
            return 1;
        }
    }
    return 0;
}
model One_step_kill_model[]={
    {{0,1,1,1,1},{{0,{}}}},
    {{1,0,1,1,1},{{1,{}}}},
    {{1,1,0,1,1},{{2,{}}}},
    {{1,1,1,0,1},{{3,{}}}},
    {{1,1,1,1,0},{{4,{}}}},
};
Action AI::check_Four(const vector<vector<int>>&mp,int color)const{
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c)
                for(auto &it:One_step_kill_model)
                    if(check_model(mp,r,c,dr,dc,it.model,color,0)){
                        int cnt=it.pos[0].first;
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
    return {ACT_JUMP,-1,-1};
}
model Two_steps_kill_model[]={
    {{0,0,1,1,1,0},{{1,{}}}},
    {{0,1,1,0,1,0},{{3,{}}}},
    {{0,1,1,1,0,0},{{4,{}}}},
    {{0,1,0,1,1,0},{{2,{}}}},
};
Action AI::check_live_Three(vector<vector<int>>&mp,int color)const{
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c){
                if(check_model(mp,r,c,dr,dc,{0,0,1,1,1,0,0},color,0)){
                    int mr=r+dr*1,mc=c+dc*1;
                    int yr=r+dr*5,yc=c+dc*5;
                    mp[mr][mc]=color;
                    auto score1=get_score(mp);
                    mp[mr][mc]=Chess_status::Empty;
                    mp[yr][yc]=color;
                    auto score2=get_score(mp);
                    mp[yr][yc]=Chess_status::Empty;
                    return score1>score2?Action{ACT_MOVE,mr,mc}:Action{ACT_MOVE,yr,yc};
                }
            }
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c)
                for(auto &it:Two_steps_kill_model){
                    if(check_model(mp,r,c,dr,dc,it.model,color,0)){
                        int cnt=it.pos[0].first;
                        return {ACT_MOVE,r+dr*cnt,c+dc*cnt};
                    }
                }
    return {ACT_JUMP,-1,-1};
}
model Two_steps_kill_model1[]={
    {{0,0,1,1,1,0},{{0,{}},{1,{}},{5,{}}}},
    {{0,1,1,0,1,0},{{0,{}},{3,{}},{5,{}}}},
    {{0,1,1,1,0,0},{{0,{}},{4,{}},{5,{}}}},
    {{0,1,0,1,1,0},{{0,{}},{2,{}},{5,{}}}},
};
vector<Action> AI::check_live_Three1(vector<vector<int>>&mp,int color)const{
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c){
                if(check_model(mp,r,c,dr,dc,{0,0,1,1,1,0,0},color,0)){
                    int mr=r+dr*1,mc=c+dc*1;
                    int yr=r+dr*5,yc=c+dc*5;
                    vector<Action> res;
                    res.emplace_back(ACT_MOVE,mr,mc);
                    res.emplace_back(ACT_MOVE,yr,yc);
                    return res;
                }
            }
    for(const auto &[dr,dc]:dirs)
        for(int r=0;r<row;++r)
            for(int c=0;c<col;++c)
                for(auto &it:Two_steps_kill_model1){
                    if(check_model(mp,r,c,dr,dc,it.model,color,0)){
                        vector<Action> res;
                        for(auto &[cnt,ignore]:it.pos)
                            res.emplace_back(ACT_MOVE,r+dr*cnt,c+dc*cnt);
                        return res;
                    }
                }
    return {};
}
model vcf_model[]={
    {{0,0,1,1,1},{{0,{1}},{1,{0}}}},
    {{0,1,0,1,1},{{0,{2}},{2,{0}}}},
    {{0,1,1,0,1},{{0,{3}},{3,{0}}}},
    {{1,0,0,1,1},{{1,{2}},{2,{1}}}},
    {{1,0,1,0,1},{{1,{3}},{3,{1}}}},
    {{1,1,0,0,1},{{2,{3}},{3,{2}}}},
    {{1,0,1,1,0},{{1,{4}},{4,{1}}}},
    {{1,1,0,1,0},{{2,{4}},{4,{2}}}},
    {{1,1,1,0,0},{{3,{4}},{4,{3}}}},
};
Action AI::vcf(vector<vector<int>>&mp,int dep,int color)const{
    auto res=check_Four(mp,color);
    if(res.type!=ACT_JUMP) return res;
    if(dep==0) return {ACT_JUMP,-1,-1};
    for(int r=0;r<row;++r)
        for(int c=0;c<col;++c)
            for(auto &[dr,dc]:dirs)
                for(auto &it:vcf_model)
                    if(check_model(mp,r,c,dr,dc,it.model,color,0))
                    for(auto &[d1,pos1]:it.pos)
                        for(auto &d2:pos1){
                            int mr=r+dr*d1,mc=c+dc*d1;
                            int yr=r+dr*d2,yc=c+dc*d2;
                            mp[mr][mc]=color;
                            mp[yr][yc]=3-color;
                            res=check_Four(mp,3-color);
                            if(res.type!=ACT_JUMP){
                                mp[mr][mc]=mp[yr][yc]=Chess_status::Empty;
                                continue;
                            }
                            res=vcf(mp,dep-1,color);
                            mp[mr][mc]=mp[yr][yc]=Chess_status::Empty;
                            if(res.type!=ACT_JUMP) return {ACT_MOVE,mr,mc};
                        }
    return {ACT_JUMP,-1,-1};
}

// model vct_model[]={
//     {{0,0,1,0,1,0},{{1,{0,3,5}},{3,{1,5}}}},
//     {{0,1,0,1,0,0},{{2,{0,4}},{4,{0,2,5}}}},
//     {{0,0,1,1,0,0},{{1,{0,4,5}},{3,{1,5}}}},
// };

Action AI::vct(vector<vector<int>>&mp,int dep,int color)const{
    if(dep==0) return {ACT_JUMP,-1,-1};
    auto res=check_Four(mp,3-color);
    if(res.type!=ACT_JUMP){
        auto [ignore,r,c]=res;
        mp[r][c]=color;
        res=check_Four(mp,color);
        if(res.type!=ACT_JUMP){
            auto [ignore,mr,mc]=res;
            mp[mr][mc]=3-color;
            res=vct(mp,dep-1,color);
            mp[mr][mc]=Chess_status::Empty;
            if(res.type!=ACT_JUMP){
                mp[r][c]=Chess_status::Empty;
                return {ACT_MOVE,r,c};
            }
        }
        auto res1=check_live_Three1(mp,color);
        if(res1.empty()){
            mp[r][c]=Chess_status::Empty;
            return {ACT_JUMP,-1,-1};
        }
        for(auto [ignore,mr,mc]:res1){
            mp[mr][mc]=3-color;
            res=vct(mp,dep-1,color);
            mp[mr][mc]=Chess_status::Empty;
            if(res.type==ACT_JUMP){
                mp[r][c]=Chess_status::Empty;
                return {ACT_JUMP,-1,-1};
            }
        }
        mp[r][c]=Chess_status::Empty;
        return {ACT_MOVE,r,c};
    }
    res=check_Four(mp,color);
    if(res.type!=ACT_JUMP) return res;
    res=check_live_Three(mp,color);
    if(res.type!=ACT_JUMP) return res;
    for(int r=0;r<row;++r)
        for(int c=0;c<col;++c){
            if(mp[r][c]!=Chess_status::Empty) continue;
            if(!check(mp,r,c)) continue;
            mp[r][c]=color;
            res=check_Four(mp,color);
            if(res.type!=ACT_JUMP){
                auto [ignore,mr,mc]=res;
                mp[mr][mc]=3-color;
                res=vct(mp,dep-1,color);
                mp[mr][mc]=mp[r][c]=Chess_status::Empty;
                if(res.type!=ACT_JUMP) return {ACT_MOVE,r,c};
            }
            mp[r][c]=Chess_status::Empty;
        }
    res=check_live_Three(mp,3-color);
    if(res.type!=ACT_JUMP) return {ACT_JUMP,-1,-1};
    for(int r=0;r<row;++r)
        for(int c=0;c<col;++c){
            if(mp[r][c]!=Chess_status::Empty) continue;
            if(!check(mp,r,c)) continue;
            mp[r][c]=color;
            auto res1=check_live_Three1(mp,color);
            if(res1.empty()){
                mp[r][c]=Chess_status::Empty;
                continue;
            }
            bool flag=1;
            for(auto [ignore,mr,mc]:res1){
                mp[mr][mc]=3-color;
                res=vct(mp,dep-1,color);
                mp[mr][mc]=Chess_status::Empty;
                if(res.type==ACT_JUMP){
                    mp[r][c]=Chess_status::Empty;
                    flag=0;
                    break;
                }
            }
            mp[r][c]=Chess_status::Empty;
            if(flag) return {ACT_MOVE,r,c};
        }
    return {ACT_JUMP,-1,-1};
}
model db_thr_model[]={
    {{0,0,0,1,1,0},{{1,{}}}},
    {{0,0,0,1,1,0},{{2,{}}}},
    {{0,0,1,1,0,0},{{1,{}}}},
    {{0,0,1,0,1,0},{{1,{}}}},
    {{0,0,1,0,1,0},{{3,{}}}},
    {{0,1,0,0,1,0},{{2,{}}}},
};
bool AI::check_db_thr(const vector<vector<int>>&mp,int r,int c,int color)const{
    for(int i=0;i<3;++i)
        for(auto &it:db_thr_model){
            int pos=it.pos[0].first;
            int len=it.model.size();
            if(check_model(mp,r-pos*dirs[i][0],c-pos*dirs[i][1],dirs[i][0],dirs[i][1],it.model,color,0)||
            check_model(mp,r-(len-pos-1)*dirs[i][0],c-(len-pos-1)*dirs[i][1],dirs[i][0],dirs[i][1],it.model,color,1))
                for(int j=i+1;j<4;++j)
                    for(auto &jt:db_thr_model){
                        int jpos=jt.pos[0].first;
                        int jlen=jt.model.size();
                        if(check_model(mp,r-jpos*dirs[j][0],c-jpos*dirs[j][1],dirs[j][0],dirs[j][1],jt.model,color,0)||
                        check_model(mp,r-(jlen-jpos-1)*dirs[j][0],c-(jlen-jpos-1)*dirs[j][1],dirs[j][0],dirs[j][1],jt.model,color,1))
                            return 1;
                    }
        }
    return 0;
}
Action AI::double_three(vector<vector<int>>&mp,int color)const{
    for(int r=0;r<row;++r)
        for(int c=0;c<col;++c)
            if(mp[r][c]==Chess_status::Empty&&check_db_thr(mp,r,c,color))
                return {ACT_MOVE,r,c};
    return {ACT_JUMP,-1,-1};
}

Action AI::suansha(vector<vector<int>>&mp)const{
    cerr<<"start my check_Four"<<endl;
    auto res=check_Four(mp,color);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find my check_Four"<<endl;

    cerr<<"start duishou check_Four"<<endl;
    res=check_Four(mp,3-color);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find duishou check_Four"<<endl;

    cerr<<"start my vct"<<endl;
    res=vct(mp,vctDepth_,color);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find my vct"<<endl;
    
    cerr<<"start my check_live_Three"<<endl;
    res=check_live_Three(mp,color);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find my check_live_Three"<<endl;

    cerr<<"start duishou check_live_Three"<<endl;
    res=check_live_Three(mp,3-color);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find duishou check_live_Three"<<endl;

    cerr<<"start my double_three"<<endl;
    res=double_three(mp,color);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find my double_three"<<endl;

    cerr<<"start my vcf"<<endl;
    res=vcf(mp,vcfDepth_,color);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find my vcf"<<endl;

    cerr<<"start duishou double_three"<<endl;
    res=double_three(mp,3-color);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find duishou double_three"<<endl;

    return {ACT_JUMP,-1,-1};
}
Action AI::search(vector<vector<int>>&mp)const{
    static int cnt=0;
    cerr<<"cnt="<<++cnt<<endl;
    cerr<<"start suansha"<<endl;
    auto res=suansha(mp);
    if(res.type!=ACT_JUMP) return res;
    cerr<<"not find suansha"<<endl;
    cerr<<"start alpha_beta_search"<<endl;
    res=a_b_search(mp);
    if(res.type==ACT_JUMP) return res;
    return res;
}
Action AI::computeMove(vector<vector<int>> mp)const{
    int cnt=0;
    for(const auto &i:mp)
        for(const auto &j:i)
            if(j==3-color)
                ++cnt;
    if(cnt==0) return {ACT_MOVE,row/2,col/2};
    else if(cnt==1){
        vector<Action> st;
        for(int dr=-1;dr<=1;++dr)
            for(int dc=-1;dc<=1;++dc){
                int r=row/2+dr,c=col/2+dc;
                if(in(r,c)&&mp[r][c]==Chess_status::Empty)
                    st.push_back({ACT_MOVE,r,c});
            }
        if(st.empty()||rand_float(0.2)) return Rand_step(mp);
        else return st[randInt(0,st.size()-1)];
    }else return search(mp);
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