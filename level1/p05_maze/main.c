#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<conio.h>
#define KEY_DOWN(VK_NONAME) (GetAsyncKeyState(VK_NONAME) & 0x8000 ? 1 : 0)
#define KEY_UP(VK_NONAME) (GetAsyncKeyState(VK_NONAME) & 0x8000 ? 0 : 1)
#define height 110
#define width 110
int maze[height][width];
int dx[]={0,1,0,-1},dy[]={1,0,-1,0};
char pic[][2]={"#"," ","P","O"};
void Setpos(int x,int y){
	COORD pos;
	pos.X=x;
	pos.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}
void Hide() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);  
	CONSOLE_CURSOR_INFO CursorInfo;  
	GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息  
	CursorInfo.bVisible = 0; //隐藏控制台光标  
	SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态   
}
void output(int x,int y,char *s){
	Setpos(x,y);
	printf("%s",s);
}
void Init(){
	Hide();
	SetConsoleTitleA("Maze");
	srand(time(0));
}
void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);  
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}
void dfs(int x,int y,int h,int w){
	maze[x][y]=1;
	int dirs[]={0,1,2,3};
    shuffle(dirs,4);
	for(int j=0;j<4;++j){
		int i=dirs[j];
		int tx=x+dx[i]*2,ty=y+dy[i]*2;
		if(0<=tx&&tx<h-1&&0<=ty&&ty<w-1&&!maze[tx][ty]){
			maze[x+dx[i]][y+dy[i]]=1;
			dfs(tx,ty,h,w);
		}
	}
}
void get_new_maze(int h,int w){
	memset(maze,0,sizeof(maze));
	dfs(1,1,h,w);
	maze[1][1]=2;
	maze[h-2][w-2]=3;
}
void print_map(int X,int Y){
	system("cls");
	for(int i=0;i<Y;++i){
		for(int j=0;j<X;++j){
			output(j,i,pic[maze[j][i]]);
		}
	}
}
void start(){
	int X=21,Y=21;
	get_new_maze(X,Y);
	print_map(X,Y);
	int player[]={1,1};
	while(1){
		int dx=0,dy=0;
		if(KEY_DOWN(VK_UP)){
			if(player[1]!=1)
				dy=-1;
		}
		if(KEY_DOWN(VK_LEFT)){
			if(player[0]!=1)
				dx=-1;
		}
		if(KEY_DOWN(VK_DOWN)){
			if(player[1]!=Y-2)
				dy=1;
		}
		if(KEY_DOWN(VK_RIGHT)){
			if(player[0]!=X-2)
				dx=1;
		}
		if(maze[player[0]+dx][player[1]+dy]==0) continue;
		if(dx==0&&dy==0) continue;
		output(player[0],player[1],pic[1]);
		player[0]+=dx;
		player[1]+=dy;
		output(player[0],player[1],pic[2]);
		if(player[0]==X-2&&player[1]==Y-2){
			output(0,Y,"win!!!");
			Sleep(5000);
			system("cls");
			return;
		}
		Sleep(10);
	}
}
int main(){
	Init();
	while(1){
		printf("press space to start a new game\n");
		while(!(_kbhit()&&_getch()==32)) Sleep(50);
		start();
	}
	return 0;
}
