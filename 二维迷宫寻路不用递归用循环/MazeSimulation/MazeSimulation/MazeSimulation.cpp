// MazeSimulation.cpp : Defines the entry point for the console application.
//

/*
*用循环来代替递归，解决走迷宫问题。
*方法就是模拟“势”的概念，可以用电势来类比：把四通八达的迷宫通道想象为一个电路，把通道上
*的每个空格都想象为一块电阻。小人每次走过一个空格时，就让空格上的阻力（电阻）变大一点。
*这样小人（电势）每次面临选择时，就会选择更易通过的那个空格（电阻越低电流越大），也就是
*倾向于选择走比较新的路，而不会再走旧路。
*/

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//迷宫字符(为了迷宫数组的视觉效果更直观，用字母代替数字)：
//X通路，M无效通路，Y终点, P玩家
#define X -5
#define M -5  /*设置为-5的原因：因为此迷宫规模较小，规模越大的迷宫，这个初始数就要越
              小,例如-10000。此数值设置得过大的征兆：游戏运行中，空地突然变成了墙。
              如果出现此情况，应立即停止游戏，把此数值调小一些。
              嫌麻烦的话，可以直接调至-99999999（或者整形支持的最小数 + 1）。*/ 
#define WALL 1
#define Y -10  //终点的值必须比-5小

//迷宫宽度、高度
#define MAZE_WIDTH 18
#define MAZE_HEIGHT 18

//(小人移动)方向
#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

#define OPTIMIZATION_1_FLAG

//画迷宫
int draw_maze(int nMaze[][MAZE_WIDTH], int nPlayerCoordX, int nPlayerCoordY);

//返回整形数组中最小的数的序号
int find_minimun_number_from_array(int nArray[], int nArrayCount);

/*走迷宫优化，如果三面是墙，就把这个值置为WALL+1，虽然界面还是显示为空路，但实际上阻力
已经很大，不可能再次来这里了。*/
int is_player_sorrounded_by_wall(int nArray[], int nArrayCount);

int main()
{
  //迷宫(整形)数组
  //1墙； 0(X/M)通路/无效通路; 2(Y)终点; 100(P)玩家
  int nMaze[MAZE_HEIGHT][MAZE_WIDTH] =
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, X, 1, 1, 1, 1, M, 1, M, M, X, X, X, X, 1, 1, 1, 1 },
    { 1, X, 1, M, 1, 1, M, 1, 1, 1, X, 1, 1, X, X, X, X, 1 },
    { 1, X, 1, M, 1, M, M, M, 1, 1, X, 1, 1, 1, 1, 1, X, 1 },
    { 1, X, 1, M, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, 1, X, 1 },
    { 1, X, 1, M, 1, M, 1, 1, X, 1, 1, 1, 1, 1, 1, 1, X, 1 },
    { 1, X, 1, M, M, M, M, M, X, 1, M, 1, M, M, M, X, X, 1 },
    { 1, X, 1, 1, 1, M, 1, 1, X, 1, M, 1, 1, 1, 1, X, 1, 1 },
    { 1, X, 1, 1, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, X, 1, 1 },
    { 1, X, X, 1, 1, 1, 1, 1, 1, 1, X, 1, 1, M, M, X, M, 1 },
    { 1, 1, X, X, X, 1, 1, 1, M, M, X, 1, 1, 1, 1, X, 1, 1 },
    { 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, 1, 1, X, 1, 1 },
    { 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, X, X, X, 1, 1 },
    { 1, 1, M, 1, X, X, X, X, X, X, X, 1, 1, X, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, M, 1, 1, 1, 1, 1, 1, 1, X, M, M, M, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, M, 1, 1, 1, 1, X, 1, 1, 1, 1 },
    { 1, 1, Y, X, X, X, X, X, X, X, X, X, X, X, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  };

  //小人目前坐标
  int nCurrentCoordX = 1;
  int nCurrentCoordY = 1;

  //用来保存周围四个格子数值的数组，从0到3是“上左下右”的顺序
  int nSurrounding[4] = { 0 };

  //如果不到终点，就用循环让小人一直走下去，Y代表终点的值
  while (nMaze[nCurrentCoordY][nCurrentCoordX] != Y) 
  {
    nSurrounding[0] = nMaze[nCurrentCoordY - 1][nCurrentCoordX];  //上值
    nSurrounding[1] = nMaze[nCurrentCoordY][nCurrentCoordX - 1];  //左值
    nSurrounding[2] = nMaze[nCurrentCoordY + 1][nCurrentCoordX];  //下值
    nSurrounding[3] = nMaze[nCurrentCoordY][nCurrentCoordX + 1];  //右值

    //每次从此处离开，此坐标的值都增加1（初始值为-5）
    nMaze[nCurrentCoordY][nCurrentCoordX] += 1;

    //优化(可以注释掉优化部分，不影响最终结果)
    if (is_player_sorrounded_by_wall(nSurrounding, _countof(nSurrounding)))
    {
      nMaze[nCurrentCoordY][nCurrentCoordX] = WALL + 1;
    }

    //根据势，选择下一步往哪个方向走，并且更改当前坐标
    switch (find_minimun_number_from_array(nSurrounding,
      _countof(nSurrounding)))
    {
    case UP:
      nCurrentCoordY--;
      break;
    case LEFT:
      nCurrentCoordX--;
      break;
    case DOWN:
      nCurrentCoordY++;
      break;
    case RIGHT:
      nCurrentCoordX++;
      break;
    default:
      break;
    }

    //更新界面（更新小人位置）
    draw_maze(nMaze, nCurrentCoordX, nCurrentCoordY);

    Sleep(50);
  }
  
  return 0;
}

/*走迷宫优化，如果三面是墙，就把这个值置为WALL+1，虽然界面还是显示为空路，但实际上阻力
已经很大，不可能再次来这里了。*/
int is_player_sorrounded_by_wall(int nArray[], int nArrayCount)
{
  int i = 0;
  int nCount = 0;

  for (i = 0; i < nArrayCount; i++)
  {
    if (nArray[i] >= WALL)
    {
      nCount++;
    }
  }

  if (nCount == 3)
  {
    return 1;
  }

  return 0;
}

//返回整形数组中最小的数的序号
int find_minimun_number_from_array(int nArray[], int nArrayCount)
{
  int i = 0;
  int nCurrentMin = 0;
  int nCurrentSerial = 0;

  //初始化最小值为数列第0项
  nCurrentMin = nArray[0];

  for (i = 0; i < nArrayCount; i++)
  {
    if (nCurrentMin > nArray[i])
    {
      nCurrentMin = nArray[i];
      nCurrentSerial = i;
    }
  }

  return nCurrentSerial;
}

//画迷宫
int draw_maze(int nMaze[][MAZE_WIDTH], int nPlayerCoordX, int nPlayerCoordY)
{
  int i = 0;
  int j = 0;

  system("cls");

  /*for (i = 0; i < MAZE_HEIGHT; i++)
  {
    for (j = 0; j < MAZE_WIDTH; j++)
    {
      if (i == nPlayerCoordY && j == nPlayerCoordX)
      {
        printf("♂");
        continue;
      }

      switch (nMaze[i][j])
      {
      case WALL:
        printf("■");
        break;
      case Y:
        printf("★");
        break;
      //case -5: //case X: //case M:
      //-5 到 -1 都是空地
      default:
        printf("  ");
        break;
        break;
      }
    }

    printf("\r\n");
  }*/

  for (i = 0; i < 324; i++)
  {
    if (i == nPlayerCoordX + nPlayerCoordY * 18)
    {
      printf("♂");
      continue;
    }

    if (i % 18 == 0)
    {
      printf("\r\n");

    }

    switch ((int) *((int *)nMaze + i))
    {
    case WALL:
      printf("■");
      break;
    case Y:
      printf("★");
      break;
      //case -5: //case X: //case M:
      //-5 到 -1 都是空地
    default:
      printf("  ");
      break;
    }
  }

  return 0;
}

/*
更多迷宫地图配置，方便测试，可以直接覆盖main函数对应部分

###################默认配置#############################：
//迷宫(整形)数组
//1墙； 0(X/M)通路/无效通路; 2(Y)终点; 100(P)玩家
int nMaze[MAZE_HEIGHT][MAZE_WIDTH] =
{
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, X, 1, 1, 1, 1, M, 1, M, M, X, X, X, X, 1, 1, 1, 1 },
{ 1, X, 1, M, 1, 1, M, 1, 1, 1, X, 1, 1, X, X, X, X, 1 },
{ 1, X, 1, M, 1, M, M, M, 1, 1, X, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, 1, M, 1, 1, X, 1, 1, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, M, M, M, M, X, 1, M, 1, M, M, M, X, X, 1 },
{ 1, X, 1, 1, 1, M, 1, 1, X, 1, M, 1, 1, 1, 1, X, 1, 1 },
{ 1, X, 1, 1, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, X, X, 1, 1, 1, 1, 1, 1, 1, X, 1, 1, M, M, X, M, 1 },
{ 1, 1, X, X, X, 1, 1, 1, M, M, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, X, X, X, 1, 1 },
{ 1, 1, M, 1, X, X, X, X, X, X, X, 1, 1, X, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, M, 1, 1, 1, 1, 1, 1, 1, X, M, M, M, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, M, 1, 1, 1, 1, X, 1, 1, 1, 1 },
{ 1, 1, Y, X, X, X, X, X, X, X, X, X, X, X, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

//小人目前坐标
int nCurrentCoordX = 1;
int nCurrentCoordY = 1;

###################配置1#############################：
//迷宫(整形)数组
//1墙； 0(X/M)通路/无效通路; 2(Y)终点; 100(P)玩家
int nMaze[MAZE_HEIGHT][MAZE_WIDTH] =
{
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, Y, 1, 1, 1, 1, M, 1, M, M, X, X, X, X, 1, 1, 1, 1 },
{ 1, X, 1, M, 1, 1, M, 1, 1, 1, X, 1, 1, X, X, X, X, 1 },
{ 1, X, 1, M, 1, M, M, M, 1, 1, X, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, 1, M, 1, 1, X, 1, 1, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, M, M, M, M, X, 1, M, 1, M, M, M, X, X, 1 },
{ 1, X, 1, 1, 1, M, 1, 1, X, 1, M, 1, 1, 1, 1, X, 1, 1 },
{ 1, X, 1, 1, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, X, X, 1, 1, 1, 1, 1, 1, 1, X, 1, 1, M, M, X, M, 1 },
{ 1, 1, X, X, X, 1, 1, 1, M, M, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, X, X, X, 1, 1 },
{ 1, 1, M, 1, X, X, X, X, X, X, X, 1, 1, X, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, M, 1, 1, 1, 1, 1, 1, 1, X, M, M, M, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, M, 1, 1, 1, 1, X, 1, 1, 1, 1 },
{ 1, 1, X, X, X, X, X, X, X, X, X, X, X, X, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

//小人目前坐标
int nCurrentCoordX = 2;
int nCurrentCoordY = 16;

###################配置2#############################：
//迷宫(整形)数组
//1墙； 0(X/M)通路/无效通路; 2(Y)终点; 100(P)玩家
int nMaze[MAZE_HEIGHT][MAZE_WIDTH] =
{
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, X, 1, 1, 1, 1, M, 1, M, M, X, X, X, X, 1, 1, 1, 1 },
{ 1, X, 1, M, 1, 1, M, 1, 1, 1, X, 1, 1, X, X, X, X, 1 },
{ 1, X, 1, M, 1, M, M, M, 1, 1, X, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, 1, M, 1, 1, X, 1, 1, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, M, M, M, M, X, M, M, 1, M, M, M, X, X, 1 },
{ 1, X, 1, 1, 1, M, 1, 1, X, 1, M, 1, 1, 1, 1, X, 1, 1 },
{ 1, X, 1, 1, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, X, X, 1, 1, M, 1, 1, M, 1, X, 1, 1, M, M, X, M, 1 },
{ 1, 1, X, X, X, M, M, M, M, M, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, X, X, X, 1, 1 },
{ 1, 1, M, 1, X, X, X, X, X, X, X, 1, 1, X, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, M, 1, 1, M, 1, 1, 1, 1, X, M, M, M, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, M, 1, 1, 1, 1, X, 1, 1, 1, 1 },
{ 1, 1, Y, X, X, X, X, X, X, X, X, X, X, X, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

//小人目前坐标
int nCurrentCoordX = 1;
int nCurrentCoordY = 1;

###################配置3#############################：
//迷宫(整形)数组
//1墙； 0(X/M)通路/无效通路; 2(Y)终点; 100(P)玩家
int nMaze[MAZE_HEIGHT][MAZE_WIDTH] =
{
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, X, 1, 1, 1, 1, M, 1, M, M, X, X, X, X, 1, 1, 1, 1 },
{ 1, X, 1, M, 1, 1, M, 1, 1, 1, X, 1, 1, X, X, X, X, 1 },
{ 1, X, 1, M, 1, M, M, M, 1, 1, X, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, 1, M, 1, 1, X, 1, 1, 1, 1, 1, 1, 1, X, 1 },
{ 1, X, 1, M, M, M, M, M, X, M, M, 1, M, M, M, X, X, 1 },
{ 1, X, 1, 1, 1, M, 1, 1, X, 1, M, 1, 1, 1, 1, X, 1, 1 },
{ 1, X, 1, 1, 1, M, 1, 1, X, X, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, X, X, 1, 1, M, 1, 1, M, 1, X, 1, 1, M, M, X, M, 1 },
{ 1, 1, X, X, X, M, M, M, M, M, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, 1, 1, X, 1, 1 },
{ 1, 1, M, 1, X, 1, 1, 1, M, 1, X, 1, 1, X, X, X, 1, 1 },
{ 1, 1, M, 1, X, X, X, X, X, X, X, 1, 1, X, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, M, 1, 1, M, 1, 1, 1, 1, X, M, M, Y, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, M, 1, 1, 1, 1, X, 1, 1, 1, 1 },
{ 1, 1, X, X, X, X, X, X, X, X, X, X, X, X, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

//小人目前坐标
int nCurrentCoordX = 1;
int nCurrentCoordY = 1;
*/