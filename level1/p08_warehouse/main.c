#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;
void Hide() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);  
	CONSOLE_CURSOR_INFO CursorInfo;  
	GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息  
	CursorInfo.bVisible = 0; //隐藏控制台光标  
	SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态   
}
string Plus(string a,string b){
	size_t len=max(a.length(),b.length());
	if(a.length()<b.length()) swap(a,b);
	int op=0;
	string res;
	reverse(a.begin(),a.end());
	reverse(b.begin(),b.end());
	for(size_t i=0;i<b.length();++i){
		int ax=a[i]-'0',bx=b[i]-'0';
		int sum=ax+bx+op;
		op=sum/10;
		res+=sum%10+'0';
	}
	for(size_t i=b.length();i<a.length();++i){
		int ax=a[i]-'0';
		int sum=ax+op;
		op=sum/10;
		res+=sum%10+'0';
	}
	if(op) res+=op+'0';
	reverse(res.begin(),res.end());
	return res;
}
string Minus(string a,string b){
	size_t len=max(a.length(),b.length());
	if(a.length()<b.length()) swap(a,b);
	int op=0;
	string res;
	reverse(a.begin(),a.end());
	reverse(b.begin(),b.end());
	for(size_t i=0;i<b.length();++i){
		int ax=a[i]-'0',bx=b[i]-'0';
		int sum=ax-bx-op;
		if(sum<0){
			sum+=10;
			op=1;
		}else op=0;
		res+=sum+'0';
	}
	for(size_t i=b.length();i<a.length();++i){
		int ax=a[i]-'0';
		int sum=ax-op;
		if(sum<0){
			sum+=10;
			op=1;
		}else op=0;
		res+=sum+'0';
	}
	while(res.length()>1&&res.back()=='0') res.pop_back();
	reverse(res.begin(),res.end());
	return res;
}
bool check(string a,string b){
	if(a.length()<b.length()) return false;
	if(a.length()>b.length()) return true;
	for(size_t i=0;i<a.length();++i)
		if(a[i]<b[i])
			return false;
	return true;
}
map<string,string> st;
void Init(){
	Hide();
	freopen("list.txt","r",stdin);
	int n;
	scanf("%d",&n);
	for(int i=1;i<=n;++i){
		string a,b;
		cin>>a>>b;
		st[a]=b;
	}
	fclose(stdin);
	freopen("CON","r",stdin);
}
void Exit(){
	freopen("list.txt","w",stdout);
	printf("%u\n",st.size());
	for(const auto &p : st)
		printf("%s %s\n", p.first.c_str(), p.second.c_str());
	fclose(stdout);
	exit(0);
}
void show_list(){
	size_t nl=0,cl=0;
	for(const auto &p : st){
		nl=max(nl,p.first.length());
		cl=max(cl,p.second.length());
	}
	system("cls");
	printf("显示存货列表\n");
	cout<<setfill('-')<<left<<setw(nl+1)<<"+"<<"+"<<right<<setw(cl+2)<<"+\n";
	cout<<setfill(' ')<<'|'<<left<<setw(nl)<<"name"<<'|'<<left<<setw(cl)<<"count"<<"|\n";
	cout<<setfill('-')<<left<<setw(nl+1)<<"+"<<"+"<<right<<setw(cl+2)<<"+\n";
	for(const auto &p:st){
		cout<<setfill(' ')<<'|'<<left<<setw(nl)<<p.first<<'|'<<left<<setw(cl)<<p.second<<"|\n";
		cout<<setfill('-')<<left<<setw(nl+1)<<"+"<<"+"<<right<<setw(cl+2)<<"+\n";
	}
	Sleep(3000);
	printf("按任意键返回主菜单\n");
	_getch();
}
void ruku(){
	system("cls");
	printf("入库\n");
	printf("请输入入库的物品名称：");
	string name,num;
	getline(cin,name);
	for(size_t i=0;i<name.length();++i)
		if(name[i]==' ')
			name[i]='_';
	printf("请输入入库的数量：");
	getline(cin,num);
	for(char c:num){
		if(c<'0'||c>'9'){
			printf("我chove输入数量给我输好的呀！！！\n");
			Sleep(5000);
			return;
		}
	}
	if(st.find(name)==st.end()) st[name]=num;
	else st[name]=Plus(st[name],num);
}
void chuku(){
	system("cls");
	printf("出库\n");
	printf("请输入出库的物品名称：");
	string name,num;
	getline(cin,name);
	for(size_t i=0;i<name.length();++i)
		if(name[i]==' ')
			name[i]='_';
	if(st.find(name)==st.end()){
		printf("我chove输入物品给我输好的呀！！！\n");
		Sleep(5000);
		return;
	}
	printf("请输入出库的数量：");
	getline(cin,num);
	for(char c:num){
		if(c<'0'||c>'9'){
			printf("我chove输入数量给我输好的呀！！！\n");
			Sleep(5000);
			return;
		}
	}
	if(check(st[name],num)==false){
		printf("我chove输入数量给我输好的呀！！！\n");
		Sleep(5000);
		return;
	}
	st[name]=Minus(st[name],num);
	if(st[name]=="0") st.erase(name);
}
void menu(){
	system("cls");
	Sleep(1000);
	printf("欢迎使用存货管理系统\n");
	Sleep(3000);
	printf("显示存货列表请按1\n");
	Sleep(3000);
	printf("入库请按2\n");
	Sleep(2000);
	printf("出库请按3\n");
	Sleep(2000);
	printf("退出请按4\n");
	Sleep(2000);
	printf("请输入您的选择：");
	string s;
	getline(cin,s);
	if(s=="1"){
		show_list();
		return;
	}else if(s=="2"){
		ruku();
	}else if(s=="3"){
		chuku();
	}else if(s=="4"){
		printf("退出程序\n");
		Exit();
	}else{
		system("cls");
		printf("输入错误，请重新输入\n");
	}
}
int main(){
	SetConsoleOutputCP(CP_UTF8);
	Init();
	while(1){
		menu();
	}
	return 0;
}
