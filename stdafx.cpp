// stdafx.cpp : source file that includes just the standard includes
//	ZcStatis.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

CStlString GetAppPath() {
    TCHAR cbPath[MAX_PATH + 1] = {_T('\0')};
    GetModuleFileName(GetModuleHandle(NULL), cbPath, MAX_PATH);
    CStlString strPath = cbPath;
    strPath = strPath.substr(0, strPath.find_last_of(_T('\\'), strPath.length() - 1));
    strPath = strPath + _T("\\");
    return strPath;
}


CStlString TrimString(const CStlString& str, TCHAR a) {
    CStlString::size_type pos = str.find_first_not_of(a);
    if(pos == CStlString::npos) {
        return str;
    }
    CStlString::size_type pos2 = str.find_last_not_of(a);
    if(pos2 != CStlString::npos) {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

BOOL DepartString(CStlString strTxt, CStlString strDim, CStlStrArray &arrPart) {
    arrPart.clear();
    if(strDim.empty()) {
        return false;
    }
    CStlString::size_type pos = CStlString::npos;
    CStlString::size_type offset = 0;
    while((pos = strTxt.find(strDim, offset)) != CStlString::npos) {
        CStlString token = strTxt.substr(offset, pos - offset);
        TrimString(token, _T(' '));
        TrimString(token, _T('\t'));
        if(!token.empty()) {
            arrPart.push_back(token);
        }
        offset = pos + strDim.length();
    }
    return TRUE;
}