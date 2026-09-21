#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<conio.h>
#define height 110
#define width 110
#define _WALL 0
#define _ROAD 1
#define _PLAYER 2
#define _BOX 3
#define _TARGET 4
int maze[height][width],_maze[height][width];
int level_score[10],level_open[10],TOT,lv=1,w;
int dx[]={0,1,0,-1},dy[]={1,0,-1,0};
char pic[][2]={"#"," ","P","O","X"};
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
void output(int x,int y,const char *s){
	Setpos(x,y);
	printf("%s",s);
}
void output(int x,int y,int n){
	Setpos(x,y);
	printf("%d",n);
}
void read_data(){
	if(!freopen("level/data","r",stdin)){
		printf("data file not found, please check the file path\n");
		Sleep(2000);
		exit(0);
	}else{
		scanf("%d",&TOT);
		fclose(stdin);
	}
	if(!freopen("players_data/data.txt","r",stdin)){
		memset(level_open,0,sizeof(int)*10);
	}else{
		for(int i=0;i<10;++i){
			scanf("%d%d",&level_open[i],&level_score[i]);
		}
	}
	fclose(stdin);
	freopen("CON","r",stdin);
}
int get_width(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int Width;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        Width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        return Width;
    } else {
        printf("Failed to get console info.\n");
		exit(0);
    }
    return 0;
}
void Init(){
	Hide();
	SetConsoleTitleA("Maze");
	srand(time(0));
	read_data();
	w=get_width();
}
void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);  
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}
void get_new_maze(int level,int *X,int *Y,int *px,int *py,int *min_steps){
	char s[15];
	sprintf(s,"level/level%d.txt",level);
	freopen(s,"r",stdin);
	scanf("%d %d %d",Y,X,min_steps);
	for(int j=0;j<*X;++j){
		for(int i=0;i<*Y;++i){
			scanf("%d",&maze[i][j]);
			_maze[i][j]=maze[i][j];
			if(maze[i][j]==_PLAYER){
				*px=i;
				*py=j;
			}else if(_maze[i][j]==_TARGET){
				_maze[i][j]=_ROAD;
			}
		}
	}
	fclose(stdin);
	freopen("CON","r",stdin);
}
int win(int X,int Y){
	for(int i=0;i<X;++i){
		for(int j=0;j<Y;++j){
			if(maze[i][j]==_TARGET&&_maze[i][j]!=_BOX)
				return 0;
		}
	}
	return 1;
}
int calc(int steps,int min_steps){
	if(steps>=min_steps*2) return 0;
	return 10000-(steps-min_steps)*(steps-min_steps)*10000.0/(min_steps*min_steps);
}
int start(int *level){
	int X,Y,px,py,steps=0,min_steps;
	get_new_maze(*level,&X,&Y,&px,&py,&min_steps);
	system("cls");
	for(int i=0;i<X;++i)
		for(int j=0;j<Y;++j)
			output(i,j,pic[maze[i][j]]);
	output(0,Y,"press w/a/s/d to move, r to restart, esc to exit");
	output(0,Y+1,"steps: ");
	output(7,Y+1,steps);
	int player[]={px,py};
	while(1){
		if(_kbhit()){
			int dx=0,dy=0;
			switch(_getch()){
				case 119://w
					if(player[1]!=1) dy=-1;
					else continue;
					break;
				case 97://a
					if(player[0]!=1) dx=-1;
					else continue;
					break;
				case 115://s
					if(player[1]!=Y-2) dy=1;
					else continue;
					break;
				case 100://d
					if(player[0]!=X-2) dx=1;
					else continue;
					break;
				case 114://r
					return 1;
				case 27://esc
					system("cls");
					return 0;
				default:
					continue;
			}
			int i=1;
			while(1){
				if(_maze[player[0]+dx*i][player[1]+dy*i]==_ROAD) break;
				if(_maze[player[0]+dx*i][player[1]+dy*i]==_WALL) goto end;
				i++;
			}
			_maze[player[0]+dx*i][player[1]+dy*i]=_BOX;
			output(player[0]+dx*i,player[1]+dy*i,pic[_BOX]);
			_maze[player[0]+dx][player[1]+dy]=_PLAYER;
			output(player[0]+dx,player[1]+dy,pic[_PLAYER]);
			_maze[player[0]][player[1]]=_ROAD;
			if(maze[player[0]][player[1]]==_TARGET) output(player[0],player[1],pic[_TARGET]);
			else output(player[0],player[1],pic[_ROAD]);
			player[0]+=dx;
			player[1]+=dy;
			steps++;
			output(7,Y+1,steps);
		}
		end:
		if(win(X,Y)){
			output(0,Y,"win!!!                                          ");
			output(0,Y+2,"your score: ");
			output(12,Y+2,calc(steps,min_steps));
			output(0,Y+3,"press any key to continue");
			Sleep(500);
			if(*level<TOT) level_open[*level++]=1;
			_getch();
			system("cls");
			return 0;
		}
		Sleep(10);
	}
}
void choose_level(int *lv){
	int pos=1;
	output(w/2-7,0,"level select");
	for(int i=0;i<TOT;++i){
		char s[15];
		sprintf(s,"  level  %d  ",i+1);
		if(!level_open[i]) output(w/2-7,i+1,"  (locked)  ");
		else output(w/2-7,i+1,s);
	}
	output(w/2-8,pos,"[");
	output(w/2+5,pos,"]");
	while(1){
		if(_kbhit()){
			switch(_getch()){
				case 119://w
					if(pos>1){
						output(w/2-8,pos," ");
						output(w/2+5,pos," ");
						pos--;
						output(w/2-8,pos,"[");
						output(w/2+5,pos,"]");
					}
					break;
				case 115://s
					if(pos<TOT){
						output(w/2-8,pos," ");
						output(w/2+5,pos," ");
						pos++;
						output(w/2-8,pos,"[");
						output(w/2+5,pos,"]");
					}
					break;
				case 13://enter
					if(level_open[pos-1]){
						*lv=pos;
						return;
					}
					break;
				default:
					break;
			}
		}
	}
}
void EXIT(){
	freopen("players_data/data.txt","w",stdout);
	for(int i=0;i<TOT;++i)
		printf("%d %d ",level_open[i],level_score[i]);
	fclose(stdout);
	exit(0);
}
void menu(){
	system("cls");
	int pos=0;
	output(w/2-7,0," start game ");
	output(w/2-7,1,"level select");
	output(w/2-7,2,"    exit    ");
	output(w/2-8,0,"[");
	output(w/2+5,0,"]");
	while(1){
		if(_kbhit()){
			switch(_getch()){
				case 119://w
					if(pos>0){
						output(w/2-8,pos," ");
						output(w/2+5,pos," ");
						pos--;
						output(w/2-8,pos,"[");
						output(w/2+5,pos,"]");
					}
					break;
				case 115://s
					if(pos<2){
						output(w/2-8,pos," ");
						output(w/2+5,pos," ");
						pos++;
						output(w/2-8,pos,"[");
						output(w/2+5,pos,"]");
					}
					break;
				case 13://enter
					if(pos==0){
						while(start(&lv));
						return;
					}else if(pos==1){
						choose_level(&lv);
						return;
					}else{
						system("cls");
						EXIT();
					}
					break;
				default:
					break;
			}
		}
	}
}
int main(){
	Init();
	while(1){
		menu();
	}
	return 0;
}
