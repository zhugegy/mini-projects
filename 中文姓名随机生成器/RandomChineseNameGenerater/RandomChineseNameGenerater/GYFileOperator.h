#pragma once

#include "GYString.h"
//#include <stdio.h>
#include "GYDynamicArray.h"

typedef int charnumber_t;

//文件操作类，暂时没写检查（new，fopen的空指针检查）
class CGYFileOperator
{
public:
#define MAX_STRING_SIZE 2048
public:
  //CGYFileOperator();
  CGYFileOperator(const char * pszFileName);
  virtual ~CGYFileOperator();

public:
  //返回文件的大小（字节数）
  charnumber_t get_file_size_byte();
  //文件映射到内存
  int map_file_to_memory(char ** pszMemDest, charnumber_t nStartPoint = 0,
    charnumber_t nCharNum = -1, bool isEndWith0 = true);
private:
  CGYString m_StrFileName;

  //如果文件不存在（第一次运行时），建立文件
  int create_file_in_disk();
public:

  //提取文件内容，放入动态数组
  int split_file_content_into_dynamic_array(
    CGYDynamicArray<CGYString> &storageArray, char chDelimiter = ' ', 
    bool isEndWith0 = true);
};

