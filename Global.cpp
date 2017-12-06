#include "stdafx.h"
#include "Global.h"
#include <atlfile.h>

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

std::string Global::toUTF8(const CStlString& local) {
	CT2W wide(local.c_str());
	return SysWideToUTF8(wide.m_psz);
}

CStlString Global::formUTF8(const std::string& utf8) {
	CW2T ret(SysUTF8ToWide(utf8).c_str());
	return ret.m_psz;
}

BOOL Global::ReadFileData(const CStlString& filename, std::string& filedata) {
	filedata.clear();
	CAtlFile afile;
	if (!afile.Create(filename.c_str(), GENERIC_READ, 
			FILE_SHARE_READ, OPEN_EXISTING)) {
		return FALSE;
	}
	ULONGLONG nsize = 0;
	HRESULT hr = afile.GetSize(nsize);
	if (FAILED(hr)) {
		return FALSE;
	}
	if (nsize > 0) {
		filedata.resize(nsize, '\0');
		DWORD dwBytesRead = 0;
		char* pos = (char*)filedata.data();
		do {
			if (!afile.Read(pos, nsize, dwBytesRead)) {
				return FALSE;
			}
			pos += dwBytesRead;
			nsize -= dwBytesRead;
		} while (nsize > 0);
	}
	afile.Close();
	return TRUE;
}

BOOL Global::SaveFileData(const CStlString& filename, const std::string& filedata, BOOL append) {
	CAtlFile afile;
	if (!afile.Create(filename.c_str(), GENERIC_WRITE,
			FILE_SHARE_WRITE, OPEN_ALWAYS)) {
		return FALSE;
	}
	if (append) {
		if (FAILED(afile.Seek(0, FILE_END))) {
			return FALSE;
		}
	}
	ULONGLONG nsize = filedata.size();
	DWORD dwBytesWrite = 0;
	char* pos = (char*)filedata.data();
	do {
		if (!afile.Write(pos, nsize, &dwBytesWrite)) {
			return FALSE;
		}
		pos += dwBytesWrite;
		nsize -= dwBytesWrite;
	} while (nsize > 0);
	return TRUE;
}





















/////////////////////////////////////////////////////////////////////////////////

std::string Global::SysWideToUTF8(const wchar_t* wide)
{
	return SysWideToMultiByte(wide, CP_UTF8);
}

std::string Global::SysWideToUTF8(const std::wstring& wide)
{
	return SysWideToUTF8(wide.c_str());
}

std::wstring Global::SysUTF8ToWide(const char* utf8)
{
	return SysMultiByteToWide(utf8, CP_UTF8);
}

std::wstring Global::SysUTF8ToWide(const std::string& utf8)
{
	return SysUTF8ToWide(utf8.c_str());
}

std::string Global::SysWideToNativeMB(const wchar_t* wide)
{
	return SysWideToMultiByte(wide, CP_ACP);
}

std::string  Global::SysWideToNativeMB(const std::wstring& wide)
{
	return SysWideToNativeMB(wide.c_str());
}


std::wstring Global::SysNativeMBToWide(const char* native_mb)
{
	return SysMultiByteToWide(native_mb, CP_ACP);
}

std::wstring Global::SysNativeMBToWide(const std::string& native_mb)
{
	return SysNativeMBToWide(native_mb.c_str());
}

std::wstring Global::SysMultiByteToWide(const char* mb, UINT code_page)
{
	if (mb == NULL) return std::wstring();
	int mb_length = strlen(mb);
	int charcount = MultiByteToWideChar(code_page, 0, mb, mb_length, NULL, 0);
	if (charcount == 0) return std::wstring();
	std::wstring wide;
	wide.resize(charcount, L'\0');
	MultiByteToWideChar(code_page, 0, mb, mb_length, &wide[0], charcount);
	wide.erase(wcslen(wide.c_str()));
	return wide;
}

std::string Global::SysWideToMultiByte(const wchar_t* wide, UINT code_page)
{
	if (wide == NULL) return std::string();
	int wide_length = wcslen(wide);
	if (wide_length == 0) return std::string();
	int charcount = WideCharToMultiByte(code_page, 0, wide, wide_length, NULL, 0, NULL, NULL);
	if (charcount == 0) return std::string();
	std::string mb;
	mb.resize(charcount, '\0');
	WideCharToMultiByte(code_page, 0, wide, wide_length, &mb[0], charcount, NULL, NULL);
	mb.erase(strlen(mb.c_str()));
	return mb;
}