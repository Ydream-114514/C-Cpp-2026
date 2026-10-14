#include<bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll Pow[8]={1,10,100,1000,10000,100000,1000000,10000000};
class xxx{
	vector<ll> a;
	static const int base=1e8;
	void Get(int x){
		a.clear();
		do{
			a.push_back(x%base);
			x/=base;
		}while(x);
	}
	void Get(string s){
		a.clear();
		while(s.length()%8) s='0'+s;
		int len=s.length();
		a.resize(len,0);
		for(int i=0;i<len;++i){
			a[(len-i-1)/8]+=(s[i]-48)*Pow[(len-i-1)%8];
		}
		cut();
	}
	void cut(){
		while(a.size()!=1&&a.back()==0)
			a.pop_back();
	}
	void copy(const xxx &x){
		a.clear();
		a.insert(a.begin(),x.a.begin(),x.a.end());
	}
	public:
	xxx(){Get(0);}
	xxx(int x){Get(x);}
	xxx(char *s){Get(s);}
	xxx(string s){Get(s);}
	xxx(const xxx &x){copy(x);}
	void read(){
		string s;
		cin>>s;
		Get(s);
	}
	void write(string s=""){
		printf("%lld",a.back());
		for(int i=a.size()-2;i>=0;--i)
			printf("%08lld",a[i]);
		printf("%s",s.c_str());
	}
	bool operator<(const xxx &x)const{
		if(a.size()!=x.a.size()) return a.size()<x.a.size();
		for(int i=a.size()-1;i>=0;--i)
			if(a[i]!=x.a[i]) return a[i]<x.a[i];
		return false;
	}
	bool operator==(const xxx &x)const{
		return a==x.a;
	}
	bool operator<=(const xxx &x)const{
		return *this==x||*this<x;
	}
	xxx &operator=(const xxx &x){
		this->copy(x);
		return *this;
	}
	xxx operator*(const xxx &x)const{
		xxx res(0);
		res.a.resize(a.size()+x.a.size());
		for(size_t i=0;i<a.size();++i)
			for(size_t j=0;j<x.a.size();++j){
				res.a[i+j]+=a[i]*x.a[j];
				res.a[i+j+1]+=res.a[i+j]/base;
				res.a[i+j]%=base;
			}
		res.cut();
		return res;
	}
	xxx operator+(const xxx &x)const{
		xxx res(x);
		size_t len=max(a.size(),x.a.size())+1;
		res.a.resize(len);
		ll c=0;
		for(size_t i=0;i<a.size();++i){
			res.a[i]+=a[i]+c;
			c=res.a[i]/base;
			res.a[i]%=base;
		}
		if(c) res.a[a.size()]+=c;
		res.cut();
		return res;
	}
	xxx operator/(const int &x)const{
		xxx res(*this);
		ll yu=0;
		for(size_t i=res.a.size();i>0;--i){
			res.a[i-1]+=yu*base;
			yu=res.a[i-1]%x;
			res.a[i-1]/=x;
		}
		res.cut();
		return res;
	}
	xxx operator-(const int &x)const{
		xxx res(*this);
		res.a[0]-=x;
		int i=0;
		while(res.a[i]<0){
			res.a[i]+=base;
			res.a[++i]--;
		}
		res.cut();
		return res;
	}
};
xxx ksm(xxx a,int m){
	xxx res(1),k(a);
	for(;m;m>>=1){
		if(m&1) res=res*k;
		k=k*k;
	}
	return res;
}
xxx a;
int m;
int main(){
	scanf("%d",&m);
	a.read();
	xxx l=0,r=2;
	while(ksm(r,m)<a){
		l=r;
		r=r*2;
	}
	while(l<r){
		xxx mid=(l+r+1)/2;
		if(ksm(mid,m)<=a) l=mid;
		else r=mid-1;
	}
	l.write();
	return 0;
}