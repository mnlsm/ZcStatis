#ifndef _STL_FILES
#define _STL_FILES
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <sstream>
#include <map>

typedef std::basic_ifstream<TCHAR, std::char_traits<TCHAR> > Tifstream;
typedef std::basic_ofstream<TCHAR, std::char_traits<TCHAR> > Tofstream;
typedef Tofstream CStlOutFile;
typedef Tifstream CStlInFile;

typedef std::vector<int>			CIntArray;
typedef std::vector< CIntArray >	CIntxyArray;
typedef std::vector< CIntxyArray >	CIntxyzArray;

typedef std::vector<double>			CDoubleArray;
typedef std::vector< CDoubleArray >	CDoublexyArray;

typedef std::basic_string<TCHAR> CStlString;
typedef std::vector<CStlString> CStlStrArray;
typedef std::pair<int,int> CIntPair;

#endif