#include "stdafx.h"
#include "GYString.h"
#include <string.h>

CGYString::CGYString()
{
  m_pString = NULL;
}


CGYString::CGYString(const char * pszStr)
{
  m_pString == NULL;

  set_string(pszStr);
}

CGYString::~CGYString()
{
  release_string();
}


bool CGYString::set_string(const char * pszStr)
{
  if (pszStr == NULL)
  {
    return false;
  }

  int nLength = strlen(pszStr);

  m_pString = new char[nLength + 1];

  if (m_pString == NULL)
  {
    return true;
  }

  strcpy(m_pString, pszStr);

  return true;
}


const char * CGYString::get_string_content()
{
  return m_pString;
}


int CGYString::get_string_length()
{
  return strlen(m_pString);
}


bool CGYString::release_string()
{
  if (m_pString != NULL)
  {
    delete[] m_pString;
    m_pString = NULL;
  }

  return false;
}


CGYString::CGYString(CGYString & obj)
{
  /*拷贝构造不需要释放当前的字符串，因为一定没有（参考拷贝构造发生的三种情况）*/
  //release_string();     
  set_string(obj.m_pString);
}


CGYString & CGYString::operator=(const char * pszStr)
{
  release_string();
  set_string(pszStr);

  return *this;
}


CGYString & CGYString::operator=(const CGYString & obj)
{
  release_string();
  set_string(obj.m_pString);

  return *this;
}


bool CGYString::append_string(const char * pszStr)
{
  int nNewLength = strlen(m_pString) + strlen(pszStr) + 1;

  char *pTmp = new char[nNewLength];

  if (pTmp == NULL)
  {
    return false;
  }

  strcpy(pTmp, m_pString);
  strcat(pTmp, pszStr);

  release_string();

  m_pString = pTmp;

  return true;
}


CGYString & CGYString::append(const char * pszStr)
{
  append_string(pszStr);

  return *this;
}


CGYString & CGYString::append(const CGYString & obj)
{
  append_string(obj.m_pString);

  return *this;
}

//简单的运算，设计为需要用一个CGYString类的对象接受
CGYString CGYString::operator+(const char * pszStr)
{
  CGYString tmpObj = *this;
  tmpObj.append_string(pszStr);

  return tmpObj;
}


CGYString CGYString::operator+(const CGYString & obj)
{
  CGYString tmpObj = *this;
  tmpObj.append_string(obj.m_pString);

  return tmpObj;
}


CGYString & CGYString::operator+=(const char * pszStr)
{
  append_string(pszStr);

  return *this;
}


CGYString & CGYString::operator+=(CGYString & obj)
{
  append_string(obj.m_pString);

  return *this;
}


char& CGYString::operator[](int nIndex)
{
  return m_pString[nIndex];
}

const char * CGYString::operator*()
{
  return m_pString;
}


bool CGYString::operator==(const char * pszStr)
{
  if (strcmp(m_pString, pszStr) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool CGYString::operator==(const CGYString & obj)
{
  if (strcmp(m_pString, obj.m_pString) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool CGYString::operator!=(const char * pszStr)
{
  if (strcmp(m_pString, pszStr) == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}


bool CGYString::operator!=(const CGYString & obj)
{
  if (strcmp(m_pString, obj.m_pString) == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}
