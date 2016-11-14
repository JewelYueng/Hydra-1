//
//  Board.cpp
//  Hydra
//
//  Created by Jewel on 2016/11/3.
//  Copyright © 2016年 Jewel. All rights reserved.
//

#include "Board.hpp"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;
using namespace forbiddenPoint;
using namespace boardSpace;
Board::Board(){
    initTypeTable();
    initPatternTable();
    initZobrist();
    memset(cell, 0, sizeof(cell));
    memset(IsLose, 0, sizeof(IsLose));
    memset(moveRecord, 0, sizeof(moveRecord));
    memset(hashTable, 0, sizeof(hashTable));
    setSize(20);
}

Board::~Board(){
    
}

void Board::initZobrist(){
    srand(time(NULL));
    for (int i = 0; i < MaxSize + 4; i++) {
        for (int j = 0; j < MaxSize + 4; j++) {
            zobrist[0][i][j] = random();
            zobrist[1][i][j] = random();
        }
    }
}

void Board::initPatternTable(){
    for (int key = 0; key < patternSize; key++) {
        patternTable[key][0] = lineType(0, key);
        patternTable[key][1] = lineType(1, key);
    }
}
void Board::initTypeTable(){
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 6; ++j) {
            for (int k = 0; k < 6; ++k) {
                for (int l = 0; l < 3; ++l) {
                    typeTable[i][j][k][l] = getType(i, j, k, l);
                }
            }
        }
    }
}
U64 Board::random(){
    return rand() ^ ((U64) rand() << 15) ^ ((U64) rand() << 30) ^ ((U64) rand() << 45) ^ ((U64) rand() << 60);
}

void Board::setSize(int size){
    this->size = size;
    startPoint = 4;
    endPoint = size + 4;
    for (int i = 0; i < MaxSize + 8; i++) {
        for (int j = 0; j < MaxSize + 8; j++) {
            if (i < 4 || i >= size + 4 || j < 4 || j >= size + 4) {
                cell[i][j].piece = outside;
            } else {
                cell[i][j].piece = empty;
            }
        }
    }
}

void Board::restart() {
    zobristKey = 0;
    memset(hashTable, 0, sizeof(hashTable));
    while (step) {
        delMove();
    }
}

void Board::moveTo(Position next){
    int x = next.x;
    int y = next.y;
    ++step;
    cell[x][y].piece = color(step);
    zobristKey ^= zobrist[step & 1][x][y];
    moveRecord[step] = next;
    
    for (int i = x - 2; i <= x + 2; i++){
        cell[i][y - 2].IsCand++;
        cell[i][y - 1].IsCand++;
        cell[i][y    ].IsCand++;
        cell[i][y + 1].IsCand++;
        cell[i][y + 2].IsCand++;
    }
    updateTypeTable(x, y);
}
void Board::delMove() {
    int x = moveRecord[step].x;
    int y = moveRecord[step].y;
    
    zobristKey ^= zobrist[step & 1][x][y];
    --step;
    cell[x][y].piece = empty;
    
    
    for (int i = x - 2; i <= x + 2; i++){
        cell[i][y - 2].IsCand--;
        cell[i][y - 1].IsCand--;
        cell[i][y    ].IsCand--;
        cell[i][y + 1].IsCand--;
        cell[i][y + 2].IsCand--;
    }
    updateTypeTable(x, y);
}
void Board::undo() {
    if (step >= 2) {
        delMove();
        delMove();
    }
}
void Board::updateTypeTable(int x, int y){
    int directionX,directionY;
    int key;
    
    for (int i = 0; i < 4; i++) {
        directionX = x + dx[i];
        directionY = y + dy[i];
        for (int j = 0; j < 4 && checkPos(directionX , directionY); directionX += dx[i],directionY += dy[i],j++) {
            key = getKey(directionX, directionY, i);
            cell[directionX][directionY].pattern[0][i] = patternTable[key][0];
            cell[directionX][directionY].pattern[1][i] = patternTable[key][1];
        }
        directionX = x - dx[i];
        directionY = y - dy[i];
        for (int k = 0; k < 4 && checkPos(directionX, directionY); directionX -= dx[i], directionY -= dy[i], ++k) {
            key = getKey(directionX, directionY, i);
            cell[directionX][directionY].pattern[0][i] = patternTable[key][0];
            cell[directionX][directionY].pattern[1][i] = patternTable[key][1];
        }
    }
}
int Board::getKey(int x, int y, int i){
    int key = 0;
    int a = x - dx[i] * 4;
    int b = y - dy[i] * 4;
    for (int k = 0; k < 9; a += dx[i], b += dy[i], k++) {
        if (k == 4) {
            continue;
        }
        key <<= 2;
        key ^= cell[a][b].piece;
    }
    return key;
}
int Board::lineType(int role, int key) {
    int line_left[9];
    int line_right[9];
    int i;
    
    for (i = 0; i < 9; i++) {
        if (i == 4) {
            line_left[i] = role;
            line_right[i] = role;
        } else {
            line_left[i] = key & 3;
            line_right[8 - i] = key & 3;
            key >>= 2;
        }
    }
    
    int p1 = shortLine(line_left);
    int p2 = shortLine(line_right);
    
    if (p1 == block3 && p2 == block3)
        return checkFlex3(line_left);
    
    if (p1 == block4 && p2 == flex4)
        return checkFlex4(line_left);
    
    return p1 > p2 ? p1 : p2;
}

int Board::checkFlex3(int *line) {
    int role = line[4];
    int type;
    for (int i = 0; i < 9; i++) {
        if (line[i] == empty) {
//            check if there can be flex4
            
            line[i] = role;
            type = checkFlex4(line);
//            recover line[i]
            line[i] = empty;
            if (type == flex4)
                return flex3;
        }
    }
    return block3;
}
int Board::checkFlex4(int *line){
    int role = line[4];
    int five = 0;
    int type,count;
    for (int i = 0; i < 4; i++) {
        if (line[i] == empty) {
            count = 0;
            for (int j = i - 1; j >= 0 && line[j] == role; j--){
                count++;
            }
            for (int k = i + 1; k < 9 && line[k] == role; k++) {
                count++;
            }
            if (count >= 4) {
                five ++;
            }
        }
    }
    if (five > 1) {
        return flex4;
    }
    return block4;
    
}
int Board::shortLine(int *line) {
    int kong = 0, block = 0;
    int len = 1, len2 = 1, count = 1;
    int k;
    
    int role = line[4];
    for (k = 5; k <= 8; k++) {
        if (line[k] == role) {
            if (kong + count > 4)
                break;
            ++count;
            ++len;
            len2 = kong + count;
        } else if (line[k] == empty) {
            ++len;
            ++kong;
        } else {
            if (len2 == kong + count)
                ++block;
            break;
        }
    }
    // 计算中间空格
    kong = len2 - count;
    for (k = 3; k >= 0; k--) {
        if (line[k] == role) {
            if (kong + count > 4)
                break;
            ++count;
            ++len;
            len2 = kong + count;
        } else if (line[k] == empty) {
            ++len;
            ++kong;
        } else {
            if (len2 == kong + count)
                ++block;
            break;
        }
    }
    return typeTable[len][len2][count][block];
}
int Board::getType(int len, int len2, int count, int block) {
    if (len >= 5 && count > 1) {
        if (count == 5) {
            return win;
        }
        if (len > 5 && len2 < 5 && block == 0) {
            switch (count) {
                case 2:
                    return flex2;
                case 3:
                    return flex3;
                case 4:
                    return flex4;
            }
        } else {
            switch (count) {
                case 2:
                    return block2;
                case 3:
                    return block3;
                case 4:
                    return block4;
            }
        }
    }
    return 0;
}

//bool Board::isDoubleFour(int x, int y){
//    Position pos;
//    pos.x = x;
//    pos.y = y;
//    if (cell[x+1][y+1].piece != empty){
//        return false;
//    }
//    moveTo(pos);
//    if (isWin()) {
//        delMove();
//        return false;
//    }
//    
//    int nFour = 0;
//    for (int i = 0; i < 4; i++) {
//        if (isFlexFour(pos, 0, i) == 2){
//            delMove();
//            return true;
//        }
//        if (isFlexFour(pos, 0, i) == 1) {
//            nFour ++;
//        }
//    }
//    if (nFour >= 2) {
//        delMove();
//        return true;
//    }
//    delMove();
//    return false;
//}
//
//bool Board::isDoubleThree(int x, int y){
//    Position pos;
//    pos.x = x;
//    pos.y = y;
//    if (cell[x+1][y+1].piece != empty){
//        return false;
//    }
//    moveTo(pos);
//    if (isWin()) {
//        delMove();
//        return false;
//    }
//    
//    int nThree = 0;
//    for (int i = 0; i < 4; i++) {
//        if (isFlexFour(pos, 0, i) == 2){
//            delMove();
//            return true;
//        }
//        if (isFlexFour(pos, 0, i) == 1) {
//            nThree ++;
//        }
//    }
//    if (nThree >= 2) {
//        delMove();
//        return true;
//    }
//    delMove();
//    return false;
//}
//bool Board::isMorethanFive(int x, int y){
//    Position pos;
//    pos.x = x;
//    pos.y = y;
//    moveTo(pos);
//    if (isWin()) {
//        return false;
//    }
//    if (cell[x+1][y+1].piece != empty)
//        return false;
//    int i, j;
//    bool overLine = false;
//    
//}
//int Board::isFlexFour(Position pos, int role, int dir){
//    
//}
//int Board::isFlexThree(Position pos, int role, int dir){
//    
//}
//






