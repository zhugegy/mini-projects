#pragma once


/*实现一个简单的String类，用于简单的项目，没有字符串长度安全检查，采用深拷贝（没有引用
计数和写时拷贝）*/
class CGYString
{
public:
  CGYString();
  explicit CGYString(const char * pszStr);
  CGYString(CGYString & obj);
  virtual ~CGYString();

public:
  const char * get_string_content();
  int get_string_length();
  const char * operator*();
  CGYString & operator=(const char * pszStr);
  CGYString & operator=(const CGYString & obj);
  CGYString & append(const char * pszStr);
  CGYString & append(const CGYString & obj);
  CGYString operator+(const char * pszStr);
  CGYString operator+(const CGYString & obj);
  CGYString & operator+=(const char * pszStr);
  CGYString & operator+=(CGYString & obj);
  char& operator[](int nIndex);
  bool operator==(const char * pszStr);
  bool operator==(const CGYString & obj);
  bool operator!=(const char * pszStr);
  bool operator!=(const CGYString & obj);

private:
  bool set_string(const char * pszStr);
  bool release_string();
  bool append_string(const char * pszStr);

private:
  //int m_nStringLength;    //字符串长度
  char *m_pString;
};