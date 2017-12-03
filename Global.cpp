#include "stdafx.h"
#include "Global.h"

CStlString Global::GetAppPath() {
    TCHAR cbPath[MAX_PATH + 1] = { _T('\0') };
    GetModuleFileName(GetModuleHandle(NULL), cbPath, MAX_PATH);
    CStlString strPath = cbPath;
    strPath = strPath.substr(0, strPath.find_last_of(_T('\\'), strPath.length() - 1));
    strPath = strPath + _T("\\");
    return strPath;
}


void Global::TrimString(CStlString& str, TCHAR a) {
    CStlString::size_type pos = str.find_first_not_of(a);
    if(pos == CStlString::npos) {
        return;
    }
    CStlString::size_type pos2 = str.find_last_not_of(a);
    if(pos2 != CStlString::npos) {
        str = str.substr(pos, pos2 - pos + 1);
    }
    str = str.substr(pos);
}

void Global::TrimBlank(CStlString& str) {
    TrimString(str, _T(' '));
    TrimString(str, _T('\t'));
}

BOOL Global::DepartString(const CStlString& strTxt, CStlString strDim, CStlStrArray &arrPart) {
    arrPart.clear();
    if(strDim.empty()) {
        return false;
    }
    CStlString::size_type pos = CStlString::npos;
    CStlString::size_type offset = 0;
    while((pos = strTxt.find(strDim, offset)) != CStlString::npos) {
        CStlString token = strTxt.substr(offset, pos - offset);
        TrimBlank(token);
        if(!token.empty()) {
            arrPart.push_back(token);
        }
        offset = pos + strDim.length();
    }
    if(!arrPart.empty()) {
        if(offset < strTxt.length()) {
            CStlString token = strTxt.substr(offset);
            TrimBlank(token);
            if(!token.empty()) {
                arrPart.push_back(token);
            }
        }
    }
    return TRUE;
}
