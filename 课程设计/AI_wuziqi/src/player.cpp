#include "common.h"
#include "player.h"

Action Player::turn(const vector<vector<int>>& board) const{
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        Vector2 mousePos=GetMousePosition(); // 获取鼠标当前位置 (float 类型)[reference:5]
        // 将像素坐标转换回最近的棋盘交叉点
        int c=(mousePos.x-MARGIN_LEFT)/CELL_SIZE+0.5f;
        int r=(mousePos.y-MARGIN_UP)/CELL_SIZE+0.5f;
        // 检查坐标是否有效且在棋盘内
        if(in(r,c)&&board[r][c]==Chess_status::Empty) return {ACT_MOVE,r,c};
        else return {ACT_JUMP,-1,-1};
    }else return {ACT_JUMP,-1,-1};
}