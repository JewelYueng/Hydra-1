//
//  ForbiddenPoint.cpp
//  Hydra
//
//  Created by Jewel on 2016/11/14.
//  Copyright © 2016年 Jewel. All rights reserved.
//

#include "ForbiddenPoint.hpp"
using namespace forbiddenPoint;
ChessAnalyzeData::ChessAnalyzeData()//构造
{
    adjsameNxt = 0;
    adjemptyNxt = 0;
    jumpsameNxt = 0;
    jumpemptyNxt = 0;
    jumpjumpsameNxt = 0;
    adjsamePre = 0;
    adjemptyPre = 0;
    jumpsamePre = 0;
    jumpemptyPre = 0;
    jumpjumpsamePre = 0;
}
//在p0点，direction代表方向上分析得到相关分析数据
void AnalysisLine(Psquare p0,int direction,ChessAnalyzeData* data)
{
    int s = direction;
    data->pNxt = data->pPrv = p0;
    //prv
    
    do
    {
        prvP(data->pPrv,1);
        data->adjsamePre++;
    } while (data->pPrv->z == p0->z);
    //记录adjsame前连续空位数
    while (data->pPrv->z == EMPTY_MOVE)
    {
        prvP(data->pPrv,1);
        data->adjemptyPre++;
    }
    if(data->adjemptyPre != 0)
    {
        //记录adjempty前连续同色棋子数
        while (data->pPrv->z == p0->z)
        {
            prvP(data->pPrv,1);
            data->jumpsamePre ++;
        }
        if (data->jumpsamePre != 0)
        {
            //记录jumpsame前的空位
            while (data->pPrv->z == EMPTY_MOVE)
            {
                prvP(data->pPrv,1);
                data->jumpemptyPre ++;
            }
            if (data->jumpemptyPre != 0)
            {
                //记录jumpempty前的连续同色棋子数
                while (data->pPrv->z == p0->z)
                {
                    prvP(data->pPrv,1);
                    data->jumpjumpsamePre++;
                }
            }
        }
    }
    //nxt
    do
    {
        nxtP(data->pNxt,1);
        data->adjsameNxt++;
    } while (data->pNxt->z == p0->z);
    //记录adjsame后连续空位数
    while (data->pNxt->z == EMPTY_MOVE)
    {
        nxtP(data->pNxt,1);
        data->adjemptyNxt++;
    }
    if(data->adjemptyNxt != 0)
    {
        //记录adjempty后连续同色棋子数
        while (data->pNxt->z == p0->z)
        {
            nxtP(data->pNxt,1);
            data->jumpsameNxt ++;
        }
        if (data->jumpsameNxt != 0)
        {
            //记录jumpsame后的空位
            while (data->pNxt->z == EMPTY_MOVE)
            {
                nxtP(data->pNxt,1);
                data->jumpemptyNxt ++;
            }
            if (data->jumpemptyNxt != 0)
            {
                //记录jumpempty后的连续同色棋子数
                while (data->pNxt->z == p0->z)
                {
                    nxtP(data->pNxt,1);
                    data->jumpjumpsameNxt++;
                }
            }
        }
    }
}

//禁手

//关键点禁手检测
int KeyPointForbiddenCheck(Psquare p0,int direction,int shift){
    int result = 0;
    ChessAnalyzeData checkData[4];
    
    int s = direction;
    Psquare p = p0;
    
    nxtP(p,shift);
    p->z = 1;
    
    result = ForbiddenCheck(checkData,p);
    p->z = 0;
    
    return result;
}
//禁手检测
int ForbiddenCheck(ChessAnalyzeData *checkData,Psquare p0)
{
    //禁手类型统计
    int CountThree = 0;
    int CountFour = 0;
    int adjsame[4];
    for(int i=0;i<4;i++){
        forbiddenPoint::AnalysisLine(p0,diroff[i],&checkData[i]);
        adjsame[i] = checkData[i].adjsamePre + checkData[i].adjsameNxt - 1;
        if (adjsame[i]==5)
        {
            return NO_FORBIDDEN;
        }
    }
    
    for (int i = 0; i < 4; i++)
    {
        
        int direction = diroff[i];
        //开始禁手分析
        
        if(adjsame[i] > 5 )//长链禁手
            return LONGFBD;
        
        if (adjsame[i] == 4) //四子相连
        {
            //活四冲四判断
            bool isFour = false;
            if(checkData[i].adjemptyNxt > 0)
            {
                //递归判断关键点是否可下
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
                    isFour = true;
            }
            
            if(checkData[i].adjemptyPre > 0)
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
                    isFour = true;
            }
            
            if(isFour)
                CountFour++;
        }
        else if(adjsame[i] == 3 )  //三子相连
        {
            //冲四检查
            if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 1)
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
                    CountFour++;
            }
            
            if(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre ==1)
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
                    CountFour++;
            }
            
            //活三检查
            bool isThree = false;
            
            if((checkData[i].adjemptyNxt > 2 ||(checkData[i].adjemptyNxt == 2 && checkData[i].jumpsameNxt == 0)) &&
               (checkData[i].adjemptyPre > 1 || (checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 0)))
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
                    isThree = true;
            }
            
            if((checkData[i].adjemptyPre > 2 ||(checkData[i].adjemptyPre == 2 && checkData[i].jumpsamePre == 0)) &&
               (checkData[i].adjemptyNxt > 1 || (checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 0)))
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
                    isThree = true;
            }
            
            if(isThree)
                CountThree++;
            
        }
        
        else if(adjsame[i] == 2) //两子相连
        {
            //冲四判断
            if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 2)
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
                    CountFour++;
            }
            
            if(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 2)
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
                    CountFour++;
            }
            
            //活三判断
            if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 1 && (checkData[i].jumpemptyNxt > 1
                                                                                  ||(checkData[i].jumpemptyNxt == 1 && checkData[i].jumpjumpsameNxt == 0)) && (checkData[i].adjemptyPre > 1
                                                                                                                                                             ||(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 0)))
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
                    CountThree++;
            }
            
            if(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 1 && (checkData[i].jumpemptyPre > 1
                                                                                  ||(checkData[i].jumpemptyPre == 1 && checkData[i].jumpjumpsamePre == 0)) && (checkData[i].adjemptyNxt > 1
                                                                                                                                                             ||(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 0)))
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
                    CountThree++;
            }
        }
        
        else if(adjsame[i] == 1) //单独一子
        {
            //冲四判断
            if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 3)
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
                    CountFour++;
            }
            
            if(checkData[i].adjemptyPre ==1 && checkData[i].jumpsamePre ==3)
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
                    CountFour++;
            }
            
            //活三判断
            if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt ==2 && (checkData[i].jumpemptyNxt >1 ||
                                                                                 (checkData[i].jumpemptyNxt == 1 && checkData[i].jumpjumpsameNxt == 0)) &&
               (checkData[i].adjemptyPre > 1 || (checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 0)))
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
                    CountThree++;
            }
            
            if(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre ==2 && (checkData[i].jumpemptyPre >1 ||
                                                                                 (checkData[i].jumpemptyPre == 1 && checkData[i].jumpjumpsamePre == 0)) &&
               (checkData[i].adjemptyNxt > 1 || (checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 0)))
            {
                if(forbiddenPoint::KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
                    CountThree++;
            }
        }
        //
        //检测禁手分析结果，适时返回结果，减少计算
        if(CountFour > 1)//四四禁手
        {
            return FOUR_FOUR_FBD;
        }
        if(CountThree > 1)//三三禁手
        {
            return THREE_THREE_FBD;
        }
    }
    
    return NO_FORBIDDEN;
}
