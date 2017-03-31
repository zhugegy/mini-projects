// MazeGeneration.cpp : Defines the entry point for the console application.
//

//生成随机迷宫

#include "stdafx.h"
#include <stdlib.h>
#include <time.h>

//设定路径复杂度(空格密度) 建议设置为0.0到0.6之间
#define PATH_DENSITY 0.45

//设定迷宫大小（不一定必须是正方形）
#define MAZE_WIDTH 32
#define MAZE_HEIGHT 32

//设定入口的坐标，默认为左上角(1, 1)
#define MAZE_ENTRANCE_COOR_X 1
#define MAZE_ENTRANCE_COOR_Y 1
//设定出口的坐标，默认为右下角
#define MAZE_EXIT_COOR_X MAZE_WIDTH - 2
#define MAZE_EXIT_COOR_Y MAZE_HEIGHT - 2 

//地形标示符
#define MAZE_PATH 0
#define MAZE_WALL 1
#define MAZE_ENTRANCE 2
#define MAZE_EXIT 3

//设定路径生成容忍度(大于此值则放弃此次路径生成)
#define PATH_GENERATION_TOLERANCE 5

//设定最大循环次数（退出死循环）
#define MAX_LOOP_TIME 600

int generate_maze(char chMaze[][MAZE_WIDTH]);

//打通道路
int generate_path(char chMaze[][MAZE_WIDTH], int nStartCoorX, int nStartCoorY,
  int nEndCoorX, int nEndCoorY);

//获得一个随机的空地的坐标（用于生成随机路径，作为随机路径的起点）
int get_a_random_path_coordinate(char chMaze[][MAZE_WIDTH], int *pnCoorX,
  int *pnCoorY);

//打印迷宫
int print_maze(char chMaze[][MAZE_WIDTH]);

float get_path_density(char chMaze[][MAZE_WIDTH]);

int main()
{
  char chMaze[MAZE_HEIGHT][MAZE_WIDTH] = { 0 };
 
  srand((unsigned int)time(0));

  while (true)
  {
    generate_maze(chMaze);
    print_maze(chMaze);
    system("pause");
  }

  return 0;
}

int generate_maze(char chMaze[][MAZE_WIDTH])
{
  int i = 0;
  int j = 0;

  int nTempStartCoorX = 0;
  int nTempStartCoorY = 0;
  int nTempEndCoorX = 0;
  int nTempEndCoorY = 0;

  //先全部初始化为墙
  for (i = 0; i < MAZE_HEIGHT; i++)
  {
    for (j = 0; j < MAZE_WIDTH; j++)
    {
      chMaze[i][j] = MAZE_WALL;
    }
  }

  //生成起点到终点的通路
  generate_path(chMaze, MAZE_ENTRANCE_COOR_X, MAZE_ENTRANCE_COOR_Y,
    MAZE_EXIT_COOR_X, MAZE_EXIT_COOR_Y);

  //debug //print_maze(chMaze);

  //生成随机路径
  while (get_path_density(chMaze) < PATH_DENSITY)
  {
    get_a_random_path_coordinate(chMaze, &nTempStartCoorX, &nTempStartCoorY);

    nTempEndCoorX = (rand() % (MAZE_WIDTH - 2)) + 1;
    nTempEndCoorY = (rand() % (MAZE_HEIGHT - 2)) + 1;

    generate_path(chMaze, nTempStartCoorX, nTempStartCoorY, nTempEndCoorX,
      nTempEndCoorY);

    //debug //print_maze(chMaze);
  }

  //标记出入口点
  chMaze[MAZE_ENTRANCE_COOR_Y][MAZE_ENTRANCE_COOR_X] = MAZE_ENTRANCE;
  chMaze[MAZE_EXIT_COOR_Y][MAZE_EXIT_COOR_X] = MAZE_EXIT;

  return 0;
}

//打通道路
int generate_path(char chMaze[][MAZE_WIDTH], int nStartCoorX, int nStartCoorY,
  int nEndCoorX, int nEndCoorY)
{
  /*nTolerance用于检测不得不生成宽度为2的路的情况（需要避免此情况，如果过多，则说明路
  径无解，返回函数）*/
  int nTolerance = 0;

  /*nWhileLoopCount用于应对发生死循环的情况，此计数器超过一定值则强制跳出循环。*/
  int nWhileLoopCount = 0;

  int nDirection = -1;
  int nCurrentCoorX = nStartCoorX;
  int nCurrentCoorY = nStartCoorY;

  while (true)
  {
    if (nWhileLoopCount++ >= MAX_LOOP_TIME)
    {
      return -1;
    }

    if (nTolerance > PATH_GENERATION_TOLERANCE)
    {
      return -1;
    }

    if (nCurrentCoorX == nEndCoorX && nCurrentCoorY == nEndCoorY)
    {
      return 0;
    }

    nDirection = rand() % 4;

    switch (nDirection)
    {
    case 0:   //向上
      //如果整体趋势是要向下，则忽略
      if (nStartCoorY < nEndCoorY)
      {
        continue;
      }

      //如果到达迷宫边界，则忽略
      if (nCurrentCoorY <= 1)
      {
        continue;
      }

      //如果左侧形成宽度为2的路，则增加nTolerance，忽略
      if (nCurrentCoorX >= 1 && 
        chMaze[nCurrentCoorY][nCurrentCoorX - 1] == MAZE_PATH &&
        chMaze[nCurrentCoorY - 1][nCurrentCoorX -1] == MAZE_PATH)
      {
        nTolerance++;
        continue;
      }
      
      //如果右侧形成宽度为2的路，则增加nTolerance，忽略
      if (nCurrentCoorX <= MAZE_WIDTH - 2 &&
        chMaze[nCurrentCoorY][nCurrentCoorX + 1] == MAZE_PATH &&
        chMaze[nCurrentCoorY - 1][nCurrentCoorX + 1] == MAZE_PATH)
      {
        nTolerance++;
        continue;
      }

      //合法情况
      chMaze[nCurrentCoorY - 1][nCurrentCoorX] = MAZE_PATH;
      nCurrentCoorY--;
      nTolerance = 0;

      break;
    case 1:   //向下
      //如果整体趋势是要向上，则忽略
      if (nStartCoorY > nEndCoorY)
      {
        continue;
      }

      //如果到达迷宫边界，则忽略
      if (nCurrentCoorY >= MAZE_HEIGHT - 2)
      {
        continue;
      }

      //如果左侧形成宽度为2的路，则增加nTolerance，忽略
      if (nCurrentCoorX >= 1 &&
        chMaze[nCurrentCoorY][nCurrentCoorX - 1] == MAZE_PATH &&
        chMaze[nCurrentCoorY + 1][nCurrentCoorX - 1] == MAZE_PATH)
      {
        nTolerance++;
        continue;
      }

      //如果右侧形成宽度为2的路，则增加nTolerance，忽略
      if (nCurrentCoorX <= MAZE_WIDTH - 2 &&
        chMaze[nCurrentCoorY][nCurrentCoorX + 1] == MAZE_PATH &&
        chMaze[nCurrentCoorY + 1][nCurrentCoorX + 1] == MAZE_PATH)
      {
        nTolerance++;
        continue;
      }

      //合法情况
      chMaze[nCurrentCoorY + 1][nCurrentCoorX] = MAZE_PATH;
      nCurrentCoorY++;
      nTolerance = 0;

      break;
    case 2:   //向左
      //如果整体趋势是要向右，则忽略
      if (nStartCoorX < nEndCoorX)
      {
        continue;
      }

      //如果到达迷宫边界，则忽略
      if (nCurrentCoorX <= 1)
      {
        continue;
      }

      //如果上方形成宽度为2的路，则增加nTolerance，忽略
      if (nCurrentCoorY >= 1 &&
        chMaze[nCurrentCoorY - 1][nCurrentCoorX] == MAZE_PATH &&
        chMaze[nCurrentCoorY - 1][nCurrentCoorX - 1] == MAZE_PATH)
      {
        nTolerance++;
        continue;
      }

      //如果下方形成宽度为2的路，则增加nTolerance，忽略
      if (nCurrentCoorY <= MAZE_HEIGHT - 2 &&
        chMaze[nCurrentCoorY + 1][nCurrentCoorX] == MAZE_PATH &&
        chMaze[nCurrentCoorY + 1][nCurrentCoorX - 1] == MAZE_PATH)
      {
        nTolerance++;
        continue;
      }

      //合法情况
      chMaze[nCurrentCoorY][nCurrentCoorX - 1] = MAZE_PATH;
      nCurrentCoorX--;
      nTolerance = 0;
      
      break;
    case 3:   //向右
      //如果整体趋势是要向左，则忽略
      if (nStartCoorX > nEndCoorX)
      {
        continue;
      }

      //如果到达迷宫边界，则忽略
      if (nCurrentCoorX >= MAZE_WIDTH - 2)
      {
        continue;
      }

      //如果上方形成宽度为2的路，则增加nTolerance，忽略
      if (nCurrentCoorY >= 1 &&
        chMaze[nCurrentCoorY - 1][nCurrentCoorX] == MAZE_PATH &&
        chMaze[nCurrentCoorY - 1][nCurrentCoorX + 1] == MAZE_PATH)
      {
        nTolerance++;
        continue;
      }

      //如果下方形成宽度为2的路，则增加nTolerance，忽略
      if (nCurrentCoorY <= MAZE_HEIGHT - 2 &&
        chMaze[nCurrentCoorY + 1][nCurrentCoorX] == MAZE_PATH &&
        chMaze[nCurrentCoorY + 1][nCurrentCoorX + 1] == MAZE_PATH)
      {
        nTolerance++;
        continue;
      }

      //合法情况
      chMaze[nCurrentCoorY][nCurrentCoorX + 1] = MAZE_PATH;
      nCurrentCoorX++;
      nTolerance = 0;

      break;
    default:
      break;
    }
  }

  return -1;
}

//获得一个随机的空地的坐标（用于生成随机路径，作为随机路径的起点）
int get_a_random_path_coordinate(char chMaze[][MAZE_WIDTH], int *pnCoorX, 
  int *pnCoorY)
{
  int nRandomizedCoorX = 0;
  int nRandomizedCoorY = 0;

  while (true)
  {
    nRandomizedCoorX = (rand() % (MAZE_WIDTH - 2)) + 1;
    nRandomizedCoorY = (rand() & (MAZE_HEIGHT - 2)) + 1;

    if (chMaze[nRandomizedCoorY][nRandomizedCoorX] == MAZE_PATH)
    {
      *pnCoorX = nRandomizedCoorX;
      *pnCoorY = nRandomizedCoorY;
      return 0;
    }
  }

  return -1;
}

//打印迷宫
int print_maze(char chMaze[][MAZE_WIDTH])
{
  int i = 0;
  int j = 0;

  system("cls");

  for (i = 0; i < MAZE_HEIGHT; i++)
  {
    for (j = 0; j < MAZE_WIDTH; j++)
    {
      switch (chMaze[i][j])
      {
      case MAZE_PATH:
        printf("  ");
        break;
      case MAZE_WALL:
        printf("■");
        break;
      case MAZE_ENTRANCE:
        printf("→");
        break;
      case MAZE_EXIT:
        printf("☆");
        break;
      default:
        break;
      }
    }

    printf("\r\n");
  }

  printf("\r\n\r\n");

  return 0;
}

float get_path_density(char chMaze[][MAZE_WIDTH])
{
  int nPathCount = 0;
  int i = 0;
  int j = 0;
  float fPathDensity = 0.0f;

  for (i = 0; i < MAZE_HEIGHT; i++)
  {
    for (j = 0; j < MAZE_WIDTH; j++)
    {
      if (chMaze[i][j] == MAZE_PATH)
      {
        nPathCount++;
      }
    }
  }

  fPathDensity = (float)(((float)nPathCount) / (MAZE_WIDTH * MAZE_HEIGHT));

  return fPathDensity;
}