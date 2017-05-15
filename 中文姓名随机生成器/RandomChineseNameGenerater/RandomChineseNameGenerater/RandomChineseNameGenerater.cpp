// RandomChineseNameGenerater.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GYString.h"
#include "GYFileOperator.h"
#include "GYDynamicArray.h"
#include <time.h>
#include <stdlib.h>


int main()
{
  srand((unsigned int)time(0));


  CGYFileOperator objFileOperatorFamilyName("family_name.txt");
  CGYFileOperator objFileOperatorGivenName("given_name.txt");

  CGYDynamicArray<CGYString> objFamilyNameArray;
  objFileOperatorFamilyName.split_file_content_into_dynamic_array(
    objFamilyNameArray, '#');

  CGYDynamicArray<CGYString> objGivenNameArray;
  objFileOperatorGivenName.split_file_content_into_dynamic_array(
    objGivenNameArray, '#');


  int nSize1 = objFamilyNameArray.get_array_element_count();
  int nSize2 = objGivenNameArray.get_array_element_count();

  while (true)
  {
    printf("%s %s", *objFamilyNameArray[rand() % nSize1], 
                     *objGivenNameArray[rand() % nSize2]);
    getchar();
  }

  return 0;
}

