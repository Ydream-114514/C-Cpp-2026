#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

typedef vector<vector<float>> Matrix;
inline Matrix new_matrix(int n,int m){
    return Matrix(n,vector<float>(m,0.0f));
}

const int h = 8, w = 8;
const int CELL_W = 2;        // 每格 2 字符宽，方便鼠标点击
int startX = 2, startY = 4;  // 点阵左上角屏幕坐标

// 移动光标
void Setpos(int x, int y){
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 隐藏光标
void hide_cursor(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(hOut, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &info);
}

// 开启鼠标输入
void enable_mouse_input(){
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hIn, &mode);
    
    mode |= ENABLE_EXTENDED_FLAGS;
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    mode |= ENABLE_MOUSE_INPUT;
    mode |= ENABLE_WINDOW_INPUT;
    
    if(!SetConsoleMode(hIn, mode)){
        cerr << "SetConsoleMode 失败，错误码: " << GetLastError() << endl;
    }
}
// 绘制点阵
void draw_board(const Matrix &input, int cursorI = -1, int cursorJ = -1){
    // 上边框
    Setpos(startX, startY);
    cout << "+" << string(w * CELL_W, '-') << "+";

    for(int i = 0; i < h; ++i){
        Setpos(startX, startY + 1 + i);
        cout << "|";
        for(int j = 0; j < w; ++j){
            bool on = input[i][j] > 0.5f;
            bool isCursor = (i == cursorI && j == cursorJ);
            if(isCursor)
                cout << (on ? "@@" : "..");
            else
                cout << (on ? "##" : "  ");
        }
        cout << "|";
    }

    // 下边框
    Setpos(startX, startY + 1 + h);
    cout << "+" << string(w * CELL_W, '-') << "+";
}

// 屏幕坐标 -> 格子坐标
bool screen_to_cell(int sx, int sy, int &i, int &j){
    int rx = sx - (startX + 1);
    int ry = sy - (startY + 1);
    if(rx < 0 || ry < 0) return false;
    i = ry;
    j = rx / CELL_W;
    return (i >= 0 && i < h && j >= 0 && j < w);
}

// 拖动插值：从 (li, lj) 到 (i, j) 之间补齐
void draw_line(Matrix &input, int li, int lj, int i, int j, float val){
    int steps = max(abs(i - li), abs(j - lj));
    if(steps == 0){
        input[i][j] = val;
        return;
    }
    for(int s = 0; s <= steps; ++s){
        int ni = li + (i - li) * s / steps;
        int nj = lj + (j - lj) * s / steps;
        input[ni][nj] = val;
    }
}

// 打印点阵内容到控制台（用于调试）
void print_matrix(const Matrix &input){
    Setpos(startX, startY + h + 5);
    cout << "当前点阵：\n";
    for(int i = 0; i < h; ++i){
        cout << "  ";
        for(int j = 0; j < w; ++j)
            cout << (input[i][j] > 0.5f ? "# " : ". ");
        cout << "\n";
    }
    cout << "\n";
}

// 鼠标绘制主循环
void mouse_draw_loop(){
    Matrix input = new_matrix(h, w);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD rec;
    DWORD read;
    
    bool left_down = false, right_down = false;
    int last_i = -1, last_j = -1;
    
    system("cls");
    cout << "===== 鼠标绘制 8x8 点阵测试 =====\n";
    cout << "左键：画   右键：擦\n";
    cout << "回车：打印矩阵   R：清空   ESC：退出\n\n";
    draw_board(input);
    
    while(true){
        WaitForSingleObject(hIn, 50);
        
        DWORD events = 0;
        GetNumberOfConsoleInputEvents(hIn, &events);
        
        while(events-- > 0){
            ReadConsoleInput(hIn, &rec, 1, &read);
            
            if(rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown){
                int ch = rec.Event.KeyEvent.uChar.AsciiChar;
                if(ch == 13){ print_matrix(input); }
                else if(ch == 'r' || ch == 'R'){
                    for(int i=0;i<h;++i)
                        for(int j=0;j<w;++j)
                            input[i][j] = 0.0f;
                    draw_board(input);
                }
                else if(ch == 27){ return; }
            }
            else if(rec.EventType == MOUSE_EVENT){
                MOUSE_EVENT_RECORD m = rec.Event.MouseEvent;
                int sx = m.dwMousePosition.X;
                int sy = m.dwMousePosition.Y;
                
                bool left_pressed  = (m.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
                bool right_pressed = (m.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;
                
                int i, j;
                if(screen_to_cell(sx, sy, i, j)){
                    if(left_pressed){
                        if(left_down && last_i >= 0)
                            draw_line(input, last_i, last_j, i, j, 1.0f);
                        else
                            input[i][j] = 1.0f;
                        left_down = true; last_i = i; last_j = j;
                        draw_board(input, i, j);
                    } else if(right_pressed){
                        if(right_down && last_i >= 0)
                            draw_line(input, last_i, last_j, i, j, 0.0f);
                        else
                            input[i][j] = 0.0f;
                        right_down = true; last_i = i; last_j = j;
                        draw_board(input, i, j);
                    } else {
                        draw_board(input, i, j);
                        last_i = -1; last_j = -1;
                    }
                } else {
                    draw_board(input);
                    last_i = -1; last_j = -1;
                }
                
                if(!left_pressed)  left_down = false;
                if(!right_pressed) right_down = false;
            }
        }
    }
}

int main(){
    SetConsoleOutputCP(CP_UTF8);
    hide_cursor();
    enable_mouse_input();
    mouse_draw_loop();
    Setpos(0, startY + h + 15);
    cout << "已退出。\n";
    return 0;
}