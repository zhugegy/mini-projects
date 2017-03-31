// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
 /* short int *psnMZ = (short int *)0x00400000;
  if (*psnMZ != 'ZM')
  {
    return 0;
  }

  int *ptr = (int *)0x0040003C;
  int nPEOffset = *ptr;
  nPEOffset = nPEOffset + 0x00400000;
  ptr = (int *)nPEOffset;
  if (*ptr != 'EP')
  {
    return 0;
  }

  nPEOffset = nPEOffset + 0x28;
  ptr = (int *)nPEOffset;
  int nEntryPoint = *ptr + 0x00400000;

  printf("%p\r\n", nEntryPoint);*/

  /*
  原理（公式）：
  n = (0x00400000 - (int)ary) / sizeof(short int);
  
  short int wTest = wAry[(0x00400000 - (int)wAry) / sizeof(short int)]
  */

  int dwAry[1];
  short int wAry[1];
  int nOffset = 0;
  int nNewAddress = 0;
  int nSig = 0;
  int nEntryPoint = 0;

  if (wAry[(0x0400000 - (int) wAry) / sizeof(short int)] != 'ZM')
  {
    printf("error");
    return 0;
  }

  nOffset = dwAry[(0x040003c - (int)dwAry) / sizeof(int)];
  nNewAddress = 0x00400000 + nOffset;
  
  nSig = dwAry[(nNewAddress - (int)dwAry) / sizeof(int)];

  if (dwAry[(nNewAddress - (int)dwAry) / sizeof(int)] != 'EP')
  {
    printf("error");
    return 0;
  }

  nEntryPoint = nNewAddress + 0x28;
  nEntryPoint = dwAry[(nEntryPoint - (int)dwAry) / sizeof(int)];
  nEntryPoint = nEntryPoint + 0x00400000;

  printf("My entry pint is %p\r\n", nEntryPoint);

  return 0;
}

