// PiratesAndGolds.cpp : Defines the entry point for the console application.
//

/*
海盗分金博弈
游戏规则：半数或以上就可以通过，否则被扔到海里。

拉票情况(自己肯定投票给自己，统计需要拉的票数):
|当前存活人数 |需要拉票数 |
|----------- |---------- |
|1 |0 |
|2 |0 |
|3 |1 |
|4 |1 |
|5 |2 |
|6 |2 |
|7 |3 |
|8 |3 |
......
公式(C语言整形除法)： 
当前存活人数为奇数时: 当前存活人数 / 2
当前存活人数为偶数时: 当前存活人数 / 2 - 1

算法模拟
5人的情况

5
100

5   4
0   100

5   4   3
1   0   99

5   4   3   2
0   1   0   99

5   4   3   2   1
1   0   1   0   98

*/

#include "stdafx.h"
#include <windows.h>

#define PIRATE_NUMBER 208
#define GOLD_SUM 100

//用于标记此海盗投票情况，默认为default，被拉票后标记为此海盗将获得的金子数
#define VOTE_TAG_DEFAULT -1

/*在假想模拟中必死的海盗，即使不会获得金币，但是还是会无条件支持前面的人，因为如果不支持
前面的人，轮到他当海盗头目时，他必死。*/
#define VOTE_TAG_VOTE_FOR_LIVING -2   

//用于打印彩色
#define GREEN 0x2F
#define RED 0xCF

//#define __DEBUG_MODE

/*根据目前存活人数，求出需要拉的票数（票数不包括自己的那一票）*/
int get_needed_vote_number(int nCurrentSurvivals);

//重置投票情况统计数组(海盗头领自己投票情况不统计，因为必是同意)
int reset_vote_status(int nVoteStatus[], int nCurrentSurvivals);

//（帮助海盗头领）找到目前所持金子最少的海盗
int find_the_poorest_pirate(int nGoldPossession[], int nVoteStatus[],
  int nCurrentSurvivals);

//重新分配财富
int redistribute_golds(int nGoldPossession[], int nVoteStatus[],
  int nCurrentSurvivals);

//打印当前分配结果
int print_current_gold_possession(int nGoldPossession[], int nVoteStatus[],
  int nCurrentSurvivals);

//打印彩色字体
int print_with_color(const char* chString, int nColor);

//统计反对票数量
int get_negative_vote_number(int nVoteStatus[], int nCurrentSurvivals);

//统计为了各种原因（比如不想当头目，因为当头目必死，想要生存）一定会投出的赞成票的数量
int get_granted_positive_vote_number(int nVoteStatus[], int nCurrentSurvivals);

//重置所有海盗状态为冷漠
int reset_vote_status_to_indifference(int nVoteStatus[], int nCurrentSurvivals);

int main()
{
  int nGoldPossession[PIRATE_NUMBER] = { 0 };
  int nVoteStatus[PIRATE_NUMBER] = { 0 };
  int nGoldSum = GOLD_SUM;
  int nVoteNumberNeeded = 0;
  int nBefriendTarget = 0;    //用于暂存当前想要拉的拢海盗的序号
  int nGoldDelivered = 0;

  /*标志位，用于标记是否有海盗为了生存无条件赞成海盗头目的决定（因为在假想模拟中他们如
  果当上海盗头目必死）。标记这个状态的原因，是因为这样的海盗多了，就会导致本来必死的老大
  可以活下来。而必死的老大一旦活了下来，他的小弟们就像有了一个保护伞，从原来无条件赞成
  头目决定的状态，返回了没金钱利益就投反对票的状态。这个标记用于实现这个心态转变（一旦
  有必死的老大活了下来，那么所有的为了生存而无条件赞成的人，都变成默认的“冷漠”状态（包
  括这个侥幸活下来的老大自己））。*/
  int bFlagAnyoneCrysForLiving = -1;    

  int i = 0;
  int j = 0;

  /*模拟博弈过程。因为i除了计数功能，还代表了目前存活的海盗数量，为了可读性，i的值从1开
  始。*/
  for (i = 1; i <= PIRATE_NUMBER; i++)
  {
    if (i == 203)
    {
      system("cls");
    }

    //重置投票统计
    reset_vote_status(nVoteStatus, i);

    /*探测是否有为了生存而无条件投赞成票的人，这是为了当有必死的老大活了下来，可以重置
    所有海盗的状态为“冷漠”。*/
    if (get_granted_positive_vote_number(nVoteStatus, i))
    {
      bFlagAnyoneCrysForLiving = 1;
    }
    else
    {
      bFlagAnyoneCrysForLiving = 0;
    }

    //重置发出的金币数目
    nGoldDelivered = 0;

    //先给自己投一票（此处无关博弈逻辑，是为了方便最后输出投票结果统计）
    nVoteStatus[i - 1] = GOLD_SUM;

    //获得需要拉票的数量
    nVoteNumberNeeded = get_needed_vote_number(i);

    //因为在假想模拟中必死，所以不想当头目，一定会投赞成票的海盗不需要被拉票。
    nVoteNumberNeeded -= get_granted_positive_vote_number(nVoteStatus, i);

    //开始拉票行为（策略：找到目前分得金子最少的人，让他的收益+1。）
    /*此处逻辑并不严谨，因为假定发出去的金币小于金币总数就可以继续拉票，而忽略了仅剩1枚
    金币但需要发出2枚及以上金币才可以拉票成功的情况。幸运的是，海盗分金的数学推理里，总
    是可以找到一个金币也没有的海盗进行贿赂。事实上，每次贿赂都是把一个海盗从0金币变为1
    金币。*/
    /*总之，如果改变现有投票逻辑（例如需要一半以上的海盗赞成，而不是现在的一半或者一半
    以上），此循环可能需要修改。*/
    while (nVoteNumberNeeded > 0 && nGoldDelivered < GOLD_SUM)
    {
      nBefriendTarget = find_the_poorest_pirate(nGoldPossession, nVoteStatus,
        i);

      //把拉票目标所得金币+1
      nVoteStatus[nBefriendTarget] = nGoldPossession[nBefriendTarget] + 1;
      nGoldDelivered += nVoteStatus[nBefriendTarget];


      //拉票成功
      nVoteNumberNeeded--;
    }

    /*如果拉得票不够，证明海盗头目必死，标记此海盗头目的投票策略为
    VOTE_TAG_VOTE_FOR_LIVING,这个求生的意愿是强烈的，以至于新一轮模拟开始时重置投票统
    计的过程中，此意愿不会被抹去（详见
    int reset_vote_status(int nVoteStatus[], int nCurrentSurvivals)
    函数）。*/
    if (nVoteNumberNeeded > 0)
    {
      nVoteStatus[i - 1] = VOTE_TAG_VOTE_FOR_LIVING;
    }

    //重新分配财富
    redistribute_golds(nGoldPossession, nVoteStatus, i);

#ifdef __DEBUG_MODE
    //调试模式，打印当前分配结果
    print_current_gold_possession(nGoldPossession, nVoteStatus, i);

#endif // __DEBUG_MODE

    //必死老大活下来的情况，重置所有海盗的状态为冷漠
    if (i != PIRATE_NUMBER && bFlagAnyoneCrysForLiving == 1
        && nVoteNumberNeeded == 0)
    {
      reset_vote_status_to_indifference(nVoteStatus, i);
    }
  }
  
  //打印最终结果
  print_current_gold_possession(nGoldPossession, nVoteStatus, PIRATE_NUMBER);

  return 0;
}

/*根据目前存活人数，求出需要拉的票数（票数不包括自己的那一票）*/
int get_needed_vote_number(int nCurrentSurvivals)
{
  //偶数情况
  if (nCurrentSurvivals % 2 == 0)
  {
    return nCurrentSurvivals / 2 - 1;
  }
  else
  {
    return nCurrentSurvivals / 2;
  }

  return -1;
}

//重置投票情况统计数组(海盗头领自己投票情况不统计，因为必是同意)
/*因为海盗在不影响自己利益的情况下，乐于看到别人死，所以他们的默认投票都是反对票（
VOTE_TAG_DEFAULT）。在假想模拟中必死的海盗，他们会无条件投赞成票（
VOTE_TAG_VOTE_FOR_LIVING），为了活命。*/
int reset_vote_status(int nVoteStatus[], int nCurrentSurvivals)
{
  int i = 0;
  
  for (i = 0; i < nCurrentSurvivals - 1; i++)
  {
    if (nVoteStatus[i] != VOTE_TAG_VOTE_FOR_LIVING)
    {
      nVoteStatus[i] = VOTE_TAG_DEFAULT;
    }
  }

  return 0;
}

//重置所有海盗状态为冷漠
int reset_vote_status_to_indifference(int nVoteStatus[], int nCurrentSurvivals)
{
  int i = 0;

  for (i = 0; i < nCurrentSurvivals - 1; i++)
  {
    nVoteStatus[i] = VOTE_TAG_DEFAULT;
  }

  return 0;
}

//（帮助海盗头领）找到目前所持金子最少的海盗
int find_the_poorest_pirate(int nGoldPossession[], int nVoteStatus[], 
  int nCurrentSurvivals)
{
  int nMinimumGold = GOLD_SUM + 1;
  int nThePoorestPirate = -1;
  int i = 0;

  for (i = 0; i < nCurrentSurvivals - 1; i++)
  {
    //如果这个海盗的持有金币比目前最小金币数还小，而且他的投票状态是“未被拉票”
    if (nGoldPossession[i] < nMinimumGold && nVoteStatus[i] == VOTE_TAG_DEFAULT)
    {
      nMinimumGold = nGoldPossession[i];
      nThePoorestPirate = i;
    }
  }

  return nThePoorestPirate;
}

//重新分配财富
int redistribute_golds(int nGoldPossession[], int nVoteStatus[], 
  int nCurrentSurvivals)
{
  int i = 0;
  int nGoldDelivered = 0;

  //1.把所有人的财富清零
  for (i = 0; i < nCurrentSurvivals; i++)
  {
    nGoldPossession[i] = 0;
  }

  //2.根据拉票统计，分配给海盗手下的金子
  for (i = 0; i < nCurrentSurvivals - 1; i++)
  {
    if (nVoteStatus[i] >= 0)
    {
      nGoldPossession[i] = nVoteStatus[i];
      nGoldDelivered += nGoldPossession[i];
    }
  }

  //3.剩下的金子，归老大所有
  nGoldPossession[nCurrentSurvivals - 1] = GOLD_SUM - nGoldDelivered;

  return 0;
}

//打印当前分配结果
int print_current_gold_possession(int nGoldPossession[], int nVoteStatus[],
  int nCurrentSurvivals)
{
  int i = 0;
  int nNegativeVote = 0;

  nNegativeVote = get_negative_vote_number(nVoteStatus, nCurrentSurvivals);

  //遍历
  for (i = 0; i < nCurrentSurvivals; i++)
  {
    printf("%3d号%5d金币 ", 
           i + 1,
           nGoldPossession[i]);

    nVoteStatus[i] == VOTE_TAG_DEFAULT ? 
                      print_with_color("反对", RED) : 
                      print_with_color("赞成", GREEN);

    if (i != nCurrentSurvivals - 1 
        && nVoteStatus[i] == VOTE_TAG_VOTE_FOR_LIVING)
    {
      printf(" 为了不当头目");
    }

    printf("\r\n");
  }

  printf("海盗数目：%d 赞成票%3d 反对票%3d\r\n"
    "编号越大战斗力越高，编号最大的是海盗头目。\r\n"
    "\r\n",
    nCurrentSurvivals, nCurrentSurvivals - nNegativeVote, nNegativeVote);

  nCurrentSurvivals - nNegativeVote >= nNegativeVote ?
                                     print_with_color("海盗头目存活", GREEN) :
                                     print_with_color("海盗头目死亡", RED);
  printf("\r\n");

  printf("=====================================\r\n"
         "\r\n");   //DEBUG模式下会多次打印

  return 0;
}

//统计反对票数量
int get_negative_vote_number(int nVoteStatus[], int nCurrentSurvivals)
{
  int i = 0;
  int nCount = 0;

  for (i = 0; i < nCurrentSurvivals - 1; i++)
  {
    if (nVoteStatus[i] == VOTE_TAG_DEFAULT)
    {
      nCount++;
    }
  }

  return nCount;
}

//统计为了各种原因（比如不想当头目，因为当头目必死，想要生存）一定会投出的赞成票的数量
int get_granted_positive_vote_number(int nVoteStatus[], int nCurrentSurvivals)
{
  int i = 0;
  int nCount = 0;

  for (i = 0; i < nCurrentSurvivals - 1; i++)
  {
    if (nVoteStatus[i] == VOTE_TAG_VOTE_FOR_LIVING)
    {
      nCount++;
    }
  }

  return nCount;
}

//打印彩色字体
int print_with_color(const char* chString, int nColor)
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbiConsoleInfo;
  WORD wSavedAttributes;

  /* Save current attributes */
  GetConsoleScreenBufferInfo(hConsole, &csbiConsoleInfo);
  wSavedAttributes = csbiConsoleInfo.wAttributes;

  //打印
  SetConsoleTextAttribute(hConsole, nColor);
  printf(chString);

  /* Restore original attributes */
  SetConsoleTextAttribute(hConsole, wSavedAttributes);

  return 0;
}