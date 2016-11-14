//
//  Board.hpp
//  Hydra
//
//  Created by Jewel on 2016/11/3.
//  Copyright © 2016年 Jewel. All rights reserved.
//

#ifndef Board_hpp
#define Board_hpp

#include <stdio.h>
#include "ForbiddenPoint.hpp"
namespace boardSpace{
typedef unsigned long long U64;
const int win = 100;
const int flex4 = 90;
const int block4 = 80;
const int flex3 = 70;
const int block3 = 60;
const int flex2 = 50;
const int block2 = 40;
const int handcut = 0;
const int MaxSize = 20;
const int hashSize = 1 << 20;
const int patternSize = 65536;
//the state of each position
const int empty = 0;
const int black = 1;
const int white = 2;
const int outside = 3;
//four direction:
const int dx[4] = { 1, 0, 1, 1 };
const int dy[4] = { 0, 1, 1, -1 };

//struct Tsquare;
//typedef Tsquare *Psquare;


struct Position {
    int x;
    int y;
};
struct Point{
    Position pos;
    int value;
};

struct Cell {
    int piece;
    int IsCand;
    int pattern[2][4];
};
struct Hash{
    U64 key;
    int depth;
    int hashf;
    int val;
};
class Board{
public:
//    the total steps
    int step = 0;
    int size = 20;
//   current Zobrist value of the board
    U64 zobristKey = 0;
    U64 zobrist[2][MaxSize + 4][MaxSize + 4];
    int typeTable[10][6][6][3];
    Cell cell[MaxSize + 8][MaxSize + 8];
    int patternTable[patternSize][2];
    Position moveRecord[MaxSize * MaxSize];
//    record the lose point of the root
    bool IsLose[MaxSize + 4][MaxSize + 4];
    int startPoint, endPoint;
    Hash hashTable[hashSize];
    Position forbiddenPoint[MaxSize*MaxSize];
    Board();
    ~Board();
    void initTypeTable();
    void initPatternTable();
    void initZobrist();
    void setSize(int size);
    void moveTo(Position next);
    void delMove();
    void undo();
    void restart();
    void updateTypeTable(int x,int y);
    U64 random();
    int getKey(int x,int y,int i);
    int lineType(int role,int key);
    int shortLine(int *line);
    int checkFlex3(int *line);
    int checkFlex4(int *line);
    int getType(int len,int len2,int count,int block);
    int isFlexFour(Position pos,int role,int dir);
    int isFlexThree(Position pos,int role,int dir);
    bool isDoubleThree(int x,int y);
    bool isDoubleFour(int x,int y);
    bool isMorethanFive(int x,int y);
    
    int color(int step){
        return step & 1;
    }
    bool checkPos(int x,int y){
        if(cell[x][y].piece != outside) return true;
        else return false;
    }
    bool isType(Cell &c,int role,int type){
        return c.pattern[role][0] == type
        || c.pattern[role][1] == type
        || c.pattern[role][2] == type
        || c.pattern[role][3] == type;
    }
    bool isWin(){
        int role = color(step);
        Cell *c = &cell[moveRecord[step].x][moveRecord[step].y];
        return c->pattern[role][0] == win
        || c->pattern[role][1] == win
        || c->pattern[role][2] == win
        || c->pattern[role][3] == win;
    }
};
}
#endif /* Board_hpp */
