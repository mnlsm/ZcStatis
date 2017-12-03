#pragma once
#include "StlFiles.h"

class Global {
public:
	static CStlString GetAppPath();
	static BOOL DepartString(const CStlString& strTxt, CStlString strDim, CStlStrArray &arrPart);
	static void TrimString(CStlString& str, TCHAR a);
	static void TrimBlank(CStlString& str);

};