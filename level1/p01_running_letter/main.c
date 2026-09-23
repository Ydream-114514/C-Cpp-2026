#include<stdio.h>
#include<windows.h>
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
void Setpos(int x,int y){
	COORD pos;
	pos.X=x;
	pos.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}
void Hide() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);  
	CONSOLE_CURSOR_INFO CursorInfo;  
	GetConsoleCursorInfo(handle, &CursorInfo); 
	CursorInfo.bVisible = 0;  
	SetConsoleCursorInfo(handle, &CursorInfo);
}
void output(int x,int y,const char *s){
	Setpos(x,y);
	printf("%s",s);
}
int main(){
    Hide();
    int x=0,px=0,dx=1,w=get_width();
    while(1){
        output(x,0,"x");
        output(px,0," ");
        px=x;
        x+=dx;
        if(x==0||x==w) dx*=-1;
        Sleep(50);
    }
    return 0;
}