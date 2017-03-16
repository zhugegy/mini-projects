// FantasticMatrix.cpp : Defines the entry point for the console application.
//奇数阶幻方问题（九宫格），要求每一行、每一列、两条对角线的和相同。

#include "stdafx.h"

#include <windows.h>
#include <stdlib.h>

#define MATRIX_WIDTH 33    //默认矩阵宽度
#define COLOR_DEFUALT 0x1E    //蓝底黄色
#define COLOR_NUNMBER 2   //颜色的数量
#define MATRIX_DEFUALT_VALUE 0    //矩阵被填充前默认的数值
#define SLEEP_DURATION 5    //每次时间间隔
#define MAX_STR_LEN 30    //最大字符串长度，用于打印

//打印元素
int print_number(int nCoordX, int nCoordY, const int nNum, int nColor,
  int nNumWith);

//寻找下一个可用填充点(正常斜行返回1，路径改变返回0)
int look_for_next_slot(int *pnMatrix, int nMatrixWidth, int *pnCurrentCoorX,
  int *pnCurrentCoorY);

//获取数字宽度，用于打印
int get_number_width(int nMatrixSize);

int main()
{
  int nMatrixWidth = MATRIX_WIDTH;
  int nPrintColor = COLOR_DEFUALT;
  int nColorTop = COLOR_DEFUALT + (COLOR_NUNMBER - 1) * 0x10;
  int nNumberWidth = 0;

  int nMatrixSize = 0;
  int *pnMatrix = NULL;
  int i = 0;

  int nCoorX = 0;
  int nCoorY = 0;
  int nCurrentNum = 0;

  //计算矩阵面积
  nMatrixSize = nMatrixWidth * nMatrixWidth;

  //计算打印宽度
  nNumberWidth = get_number_width(nMatrixSize);

  //给坐标赋初值
  nCoorX = nMatrixWidth / 2;
  nCoorY = nMatrixWidth - 1;

  //给当前数字赋初值
  nCurrentNum = 1;

  //申请堆空间
  pnMatrix = (int *)malloc(sizeof(int) * nMatrixSize);

  if (!pnMatrix)
  {
    _tprintf(_T("申请堆空间失败。\r\n"));
    exit(1);
  }

  //初始化堆
  for (i = 0; i < nMatrixSize; i++)
  {
    *(pnMatrix + i) = MATRIX_DEFUALT_VALUE;
  }

  //开始填表
  while (true)
  {
    Sleep(SLEEP_DURATION);

    //赋值
    *(pnMatrix + nCoorY * nMatrixWidth + nCoorX) = nCurrentNum;

    //打印
    print_number(nCoorX * nNumberWidth, nCoorY, nCurrentNum, nPrintColor,
      nNumberWidth);

    nCurrentNum++;

    if (nCurrentNum > nMatrixSize)
    {
      break;
    }

    //寻找下一个填充点
    switch (look_for_next_slot(pnMatrix, nMatrixWidth, &nCoorX, &nCoorY))
    {
    case 0:
      //若斜行受阻，发生路径改变，则打印的背景颜色变化一下
      nPrintColor = nPrintColor + 0x10;
      if (nPrintColor > nColorTop)
      {
        nPrintColor = COLOR_DEFUALT;
      }
      break;
    case 1:
      //正常情况，什么都不做（坐标已经在look_for_next_solt函数中被改变）
      break;
    default:
      //返回-1的情况，出现逻辑错误
      _tprintf(_T("debug:逻辑错误"));
      break;
    }

  }

  //释放堆空间
  free((void *)pnMatrix);

  //打印几个回车换行，防止系统暂停信息盖住幻方信息
  for (i = 0; i < nMatrixWidth; i++)
  {
    _tprintf(_T("\r\n"));
  }

  system("pause");

  return 0;
}

//打印元素
int print_number(int nCoordX, int nCoordY, const int nNum, int nColor, 
  int nNumWith)
{
  COORD cPos;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbiConsoleInfo;
  WORD wSavedAttributes;
  TCHAR szFormat[MAX_STR_LEN] = { 0 };    //其实可以考虑作为函数参数，以减少运算量
  int i = 0;

  cPos.X = nCoordX;
  cPos.Y = nCoordY;
  SetConsoleCursorPosition(hConsole, cPos);

  /* Save current attributes */
  GetConsoleScreenBufferInfo(hConsole, &csbiConsoleInfo);
  wSavedAttributes = csbiConsoleInfo.wAttributes;

  SetConsoleTextAttribute(hConsole, nColor);

  _stprintf(szFormat, _T("%%%dd"), nNumWith);

  _tprintf(szFormat, nNum);

  /* Restore original attributes */
  SetConsoleTextAttribute(hConsole, wSavedAttributes);

  return 0;
}

//寻找下一个可用填充点(正常斜行返回1，路径改变返回0)
int look_for_next_slot(int *pnMatrix, int nMatrixWidth, int *pnCurrentCoorX, 
  int *pnCurrentCoorY)
{
  int CoordXStorage = *pnCurrentCoorX;
  int CoordYStorage = *pnCurrentCoorY;

  //看能否正常向左下方斜行
  if (CoordXStorage - 1 >= 0 && CoordYStorage + 1 < nMatrixWidth &&
    *(pnMatrix + (CoordYStorage + 1) * nMatrixWidth + CoordXStorage - 1) == 
    MATRIX_DEFUALT_VALUE)
  {
    *pnCurrentCoorX = CoordXStorage - 1;
    *pnCurrentCoorY = CoordYStorage + 1;

    return 1;
  }

  //不能正常走的情况，这几个情况的顺序重要，不能改变

  //不能正常走的情况1：左下方已被占用，则上移一格
  if (CoordXStorage - 1 >= 0 && CoordYStorage + 1 < nMatrixWidth &&
    *(pnMatrix + (CoordYStorage + 1) * nMatrixWidth + CoordXStorage - 1) !=
    MATRIX_DEFUALT_VALUE)
  {
    *pnCurrentCoorX = CoordXStorage;
    *pnCurrentCoorY = CoordYStorage - 1;

    return 0;
  }

  //不能正常走的情况2：左下方的X坐标和Y坐标同时越界（最左下角的各自），则上移一格
  if (CoordYStorage + 1 >= nMatrixWidth && CoordXStorage - 1 < 0)
  {
    *pnCurrentCoorX = CoordXStorage;
    *pnCurrentCoorY = CoordYStorage - 1;

    return 0;
  }

  //不能正常走的情况3：Y坐标越界,则左移一格，并且让Y坐标归0
  if (CoordYStorage + 1 >= nMatrixWidth)
  {
    *pnCurrentCoorX = CoordXStorage - 1;
    *pnCurrentCoorY = 0;

    return 0;
  }

  //不能正常走的情况4：X坐标越界,则下移一格，并且让X坐标变为最大
  if (CoordXStorage - 1 < 0)
  {
    *pnCurrentCoorX = nMatrixWidth - 1;
    *pnCurrentCoorY = CoordYStorage + 1;

    return 0;
  }

  return -1;
}

//获取数字宽度，用于打印
int get_number_width(int nMatrixSize)
{
  int nCount = 1;

  while (nMatrixSize >= 10)
  {
    nMatrixSize = nMatrixSize / 10;
    nCount++;
  }

  return nCount;
}