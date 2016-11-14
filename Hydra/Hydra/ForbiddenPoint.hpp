//
//  ForbiddenPoint.hpp
//  Hydra
//
//  Created by Jewel on 2016/11/14.
//  Copyright © 2016年 Jewel. All rights reserved.
//

#ifndef ForbiddenPoint_hpp
#define ForbiddenPoint_hpp

#include <stdio.h>
namespace forbiddenPoint{
typedef unsigned long DWORD;

#define EMPTY_MOVE		0
#define TURN_MOVE		1
#define OPPONENT_MOVE	2
#define OUTSIDE_MOVE	3
#define UP_DOWN				0
#define LEFTUP_RIGHTDOWN	1
#define LEFT_RIGHT			2
#define RIGHTUP_LEFTDOWN	3
#define nxtP(p,i) (p=(Psquare)(((char*)p)+(i*s)))
#define prvP(p,i) (p=(Psquare)(((char*)p)-(i*s)))
#define NO_FORBIDDEN 0
#define THREE_THREE_FBD 1
#define FOUR_FOUR_FBD 2
#define LONGFBD 3
struct Tsquare;
typedef Tsquare *Psquare;
typedef int Tsymbol;
extern int diroff[9],width,height,height2;
extern Psquare board,boardb,boardk;
struct Tsquare{
    Tsymbol z;   //0=empty, 1=black, 2=white, 3=outside
    short x,y;   //coordinates 0..width-1, 0..height-1
    
};
struct ChessAnalyzeData{
    int adjsameNxt;
    int adjemptyNxt;
    int jumpsameNxt;
    int jumpemptyNxt;
    int jumpjumpsameNxt;
    
    int adjsamePre;
    int adjemptyPre;
    int jumpsamePre;
    int jumpemptyPre;
    int jumpjumpsamePre;
    
    Psquare pNxt;
    Psquare pPrv;
    
    ChessAnalyzeData();
};
void AnalysisLine(Psquare p0,int direction,ChessAnalyzeData* data);
int KeyPointForbiddenCheck(Psquare p,int direction,int shift);
int ForbiddenCheck(ChessAnalyzeData *checkData,Psquare p0);
}
#endif /* ForbiddenPoint_hpp */
