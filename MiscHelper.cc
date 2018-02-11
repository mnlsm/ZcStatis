#include "stdafx.h"
#include "MiscHelper.h"
#include "CRCUtility.h"

std::string CMiscHelper::SysWideToUTF8(const wchar_t* wide)
{
	return SysWideToMultiByte(wide, CP_UTF8);
}

std::string CMiscHelper::SysWideToUTF8(const std::wstring& wide)
{
	return SysWideToUTF8(wide.c_str());
}

std::wstring CMiscHelper::SysUTF8ToWide(const char* utf8)
{
	return SysMultiByteToWide(utf8, CP_UTF8);
}

std::wstring CMiscHelper::SysUTF8ToWide(const std::string& utf8)
{
	return SysUTF8ToWide(utf8.c_str());
}

std::string CMiscHelper::SysWideToNativeMB(const wchar_t* wide)
{
	return SysWideToMultiByte(wide, CP_ACP);
}

std::string  CMiscHelper::SysWideToNativeMB(const std::wstring& wide)
{
	return SysWideToNativeMB(wide.c_str());
}


std::wstring CMiscHelper::SysNativeMBToWide(const char* native_mb)
{
	return SysMultiByteToWide(native_mb, CP_ACP);
}

std::wstring CMiscHelper::SysNativeMBToWide(const std::string& native_mb)
{
	return SysNativeMBToWide(native_mb.c_str());
}


std::wstring CMiscHelper::SysMultiByteToWide(const char* mb, UINT code_page)
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

std::string CMiscHelper::SysWideToMultiByte(const wchar_t* wide, UINT code_page)
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


bool CMiscHelper::PathProviderWin(int key, HMODULE this_module, wchar_t *buf, UINT bufsize)
{
	wchar_t system_buffer[MAX_PATH + 1] = { L'\0' };
	switch (key)
	{
	case CMiscHelper::DIR_FILE_EXE:
		GetModuleFileNameW(NULL, system_buffer, MAX_PATH);
		PathRemoveFileSpecW(system_buffer);
		break;
	case CMiscHelper::DIR_FILE_MODULE:
		GetModuleFileNameW(this_module, system_buffer, MAX_PATH);
		PathRemoveFileSpecW(system_buffer);
		break;
	case CMiscHelper::DIR_WINDOWS:
		GetWindowsDirectoryW(system_buffer, MAX_PATH);
		break;
	case CMiscHelper::DIR_SYSTEM:
		GetSystemDirectoryW(system_buffer, MAX_PATH);
		break;
	case CMiscHelper::DIR_TEMP:
		GetTempPathW(MAX_PATH, system_buffer);
		break;
	case CMiscHelper::DIR_PROGRAM_FILES:
		if (FAILED(SHGetFolderPathW(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, system_buffer)))
			return false;
		break;
	case CMiscHelper::DIR_IE_INTERNET_CACHE:
		if (FAILED(SHGetFolderPathW(NULL, CSIDL_INTERNET_CACHE, NULL, SHGFP_TYPE_CURRENT, system_buffer)))
			return false;
		break;
	case CMiscHelper::DIR_COMMON_START_MENU:
		if (FAILED(SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, system_buffer)))
			return false;
		break;
	case CMiscHelper::DIR_START_MENU:
		if (FAILED(SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, system_buffer)))
			return false;
		break;
	case CMiscHelper::DIR_APP_DATA:
		if (FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, system_buffer)))
			return false;
		break;
	case CMiscHelper::DIR_PROFILE:
		if (FAILED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, system_buffer)))
			return false;
		break;
		//case CMiscHelper::DIR_LOCAL_APP_DATA_LOW:
		//    if ( win::GetVersion() < win::VERSION_VISTA )
		//    {
		//        return false;
		//    }
		//    // TODO(nsylvain): We should use SHGetKnownFolderPath instead. Bug 1281128
		//    if ( FAILED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT,
		//                                  system_buffer ) ) )
		//        return false;
		//    cur = FilePath( system_buffer ).DirName().AppendASCII( "LocalLow" );
		//    break;
	case CMiscHelper::DIR_LOCAL_APP_DATA:
		if (FAILED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, system_buffer)))
			return false;
		break;
	default:
		return false;
	}
	GetLongPathNameW(system_buffer, system_buffer, MAX_PATH);
	if (wcslen(system_buffer) + 1 > bufsize)
		return false;
	if (buf != NULL && bufsize > 0)
	{
		wcscpy(buf, system_buffer);
	}
	else return false;
	return true;
}

UUID CMiscHelper::generateUUID() {
	UUID uuid = { 0 };
	if (UuidCreate(&uuid) != RPC_S_OK) {
		ZeroMemory(&uuid, sizeof(UUID));
		uuid.Data1 = GetTickCount();
	}
	return uuid;
}

UUID CMiscHelper::generateUUID(std::string *result) {
	UUID uuid = { 0 };
	if (UuidCreate(&uuid) != RPC_S_OK) {
		ZeroMemory(&uuid, sizeof(UUID));
		uuid.Data1 = GetTickCount();
	}
	if (result == NULL)
		return uuid;
	unsigned char* str = NULL;
	UuidToStringA(&uuid, &str);
	if (str != NULL) {
		result->assign(reinterpret_cast<char*>(str));
	}
	RpcStringFreeA(&str);
	return uuid;
}

bool CMiscHelper::ParseDateTime(const char* datetime_str, CMiscHelper::LPDateTime datetime) {
	//20120326T15:11:54
	if (datetime_str == NULL)
		return false;
	if (datetime == NULL)
		return false;
	char szTmp[5] = { '\0' };
	if (*datetime_str != '\0')
	{
		strncpy_s(szTmp, 5, datetime_str, 4);
		datetime->year = atol(szTmp);
		datetime_str += 4;
		strncpy_s(szTmp, 5, datetime_str, 2);
		datetime->month = atol(szTmp);
		datetime_str += 2;
		strncpy_s(szTmp, 5, datetime_str, 2);
		datetime->day = atol(szTmp);
		datetime_str += 3;
		strncpy_s(szTmp, 5, datetime_str, 2);
		datetime->hour = atol(szTmp);
		datetime_str += 3;
		strncpy_s(szTmp, 5, datetime_str, 2);
		datetime->minute = atol(szTmp);
		datetime_str += 3;
		strncpy_s(szTmp, 5, datetime_str, 2);
		datetime->second = atol(szTmp);
		datetime->millisecond = 0;
		//check datatime value?,todo
		return true;

	}
	return false;
}

std::string CMiscHelper::DateTimeToString(CMiscHelper::LPDateTime datetime) {
	char szTmp[32] = { '\0' };
	sprintf_s(szTmp, 32, "%u%u%uT%u:%u:%u", datetime->year, datetime->month, datetime->day, datetime->hour, datetime->minute, datetime->second);
	std::string ret = szTmp;
	return ret;
}


UINT CMiscHelper::ShowInformDialog(HWND hWnd, UINT MB_Type, const wchar_t* fmt, ...)
{
	CStringW strText;
	va_list argList;
	va_start(argList, fmt);
	strText.FormatV(fmt, argList);
	va_end(argList);
	UINT ret = MessageBoxW(hWnd, strText, L" 提示", MB_Type);
	return ret;
}

UINT CMiscHelper::ShowInformDialog(HWND hWnd, const wchar_t*title, UINT MB_Type, const wchar_t* fmt, ...)
{
	CStringW strText;
	va_list argList;
	va_start(argList, fmt);
	strText.FormatV(fmt, argList);
	va_end(argList);
	UINT ret = MessageBoxW(hWnd, strText, title, MB_Type);
	return ret;
}

BOOL CMiscHelper::DoForceForegroundWindow(HWND hWnd)
{
	//SetWindowPos( GetDesktopWindow(), HWND_TOP, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
	//SetForegroundWindow( GetDesktopWindow() );
	//SetWindowPos( hWnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
	ShowWindow(hWnd, SW_SHOW);
	//SetActiveWindow( hWnd );
	//SetForegroundWindow( hWnd );
	BOOL bRet = FALSE;
	HWND hForWnd = ::GetForegroundWindow();
	if (hWnd == hForWnd)
		return TRUE;

	DWORD dwProcessId;
	DWORD dwThread1 = GetWindowThreadProcessId(hWnd, &dwProcessId);
	DWORD dwThread2 = GetWindowThreadProcessId(hForWnd, &dwProcessId);
	if (dwThread1 != dwThread2)
	{
		AttachThreadInput(dwThread2, dwThread1, TRUE);
		bRet = SetForegroundWindow(hWnd);
		AttachThreadInput(dwThread2, dwThread1, FALSE);
	}
	else
	{
		bRet = SetForegroundWindow(hWnd);
	}

	if (IsIconic(hWnd))
		ShowWindow(hWnd, SW_RESTORE);
	else
		ShowWindow(hWnd, SW_SHOW);

	return bRet;
}

void CMiscHelper::SplitCStringA(const CStringA& strSource, const CStringA& dim, std::vector<CStringA>& vecString, bool trim) {
	vecString.clear();
	int iPos = 0;
	CStringA strTmp;
	strTmp = strSource.Tokenize(dim, iPos);
	if (trim) {
		strTmp = strTmp.Trim();
	}
	while (!strTmp.IsEmpty()) {
		vecString.push_back(strTmp);
		strTmp = strSource.Tokenize(dim, iPos);
		if (trim) {
			strTmp = strTmp.Trim();
		}
	}
}

void CMiscHelper::SplitCStringW(const CStringW& strSource, const CStringW& dim, std::vector<CStringW>& vecString, bool trim) {
	vecString.clear();
	int iPos = 0;
	CStringW strTmp;
	strTmp = strSource.Tokenize(dim, iPos);
	if (trim) {
		strTmp = strTmp.Trim();
	}
	while (!strTmp.IsEmpty()) {
		vecString.push_back(strTmp);
		strTmp = strSource.Tokenize(dim, iPos);
		if (trim) {
			strTmp = strTmp.Trim();
		}
	}

}



CStringA CMiscHelper::getExePath() {
	wchar_t dir[MAX_PATH + 1] = { L'\0' };
	PathProviderWin(DIR_FILE_EXE, NULL, dir, MAX_PATH);
	return SysWideToNativeMB(dir).c_str();

}

void CMiscHelper::DepartString(const CStringA& src, const CStringA& dim, CStringA& key, CStringA& val) {
	key.Empty();
	val.Empty();
	int nFind = src.Find(dim);
	if (nFind >= 0) {
		key = src.Left(nFind);
		key.Trim();
		val = src.Right(src.GetLength() - nFind - 1);
		val.Trim();
	}
}


BOOL CMiscHelper::DepartString(const CStlString& strTxt, const CStlString& strDim, 
		CStlStrArray &arrPart, bool skipEmpty) {
	arrPart.clear();
	if (strDim.empty()) {
		return false;
	}
	CStlString::size_type pos = CStlString::npos;
	CStlString::size_type offset = 0;
	while ((pos = strTxt.find(strDim, offset)) != CStlString::npos) {
		CStlString token = strTxt.substr(offset, pos - offset);
		//TrimBlank(token);
		if (!token.empty() || !skipEmpty) {
			arrPart.push_back(token);
		}
		offset = pos + strDim.length();
	}
	if (!arrPart.empty()) {
		if (offset < strTxt.length()) {
			CStlString token = strTxt.substr(offset);
			//TrimBlank(token);
			if (!token.empty() || !skipEmpty) {
				arrPart.push_back(token);
			}
		}
	}
	return TRUE;
}



bool CMiscHelper::SetClipBoardText(const char* text, HWND hWnd)
{
	//打开剪贴板  
	if (!::OpenClipboard(hWnd))
		return false;
	//empties the clipboard and frees handles to data in the clipboard  
	if (!EmptyClipboard())
	{
		CloseClipboard();
		return false;
	}
	//get text length  
	int len = strlen(text);
	//After SetClipboardData is called, the system owns the object identified by the hMem parameter.   
	//The application can read the data, but must not free the handle or leave it locked. If the   
	//hMem parameter identifies a memory object, the object must have been allocated using the   
	//GlobalAlloc function with the GMEM_MOVEABLE and GMEM_DDESHARE flags.   
	HANDLE hClip = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (len + 1)*sizeof(TCHAR));
	if (hClip == NULL)
	{
		CloseClipboard();
		return false;
	}
	//locks a global memory object and returns a pointer to the first byte of the object's memory block  
	char* pBuf = (char*)GlobalLock(hClip);
	if (pBuf == NULL)
	{
		GlobalFree(hClip);
		CloseClipboard();
		return false;
	}

	memcpy(pBuf, text, len*sizeof(char));
	pBuf[len] = NULL;

	GlobalUnlock(hClip);
	if (NULL == SetClipboardData(CF_TEXT, hClip))
	{
		GlobalFree(hClip);
		CloseClipboard();
		return false;
	}

	CloseClipboard();
	return true;
}

void CMiscHelper::string_replace(std::string& str, const std::string& src, 
		const std::string& dest) {
	std::string ret;
	std::string::size_type pos_begin = 0;
	std::string::size_type pos = str.find(src);
	while (pos != std::string::npos) {
		ret.append(str.data() + pos_begin, pos - pos_begin);
		ret += dest;
		pos_begin = pos + src.size();
		pos = str.find(src, pos_begin);
	}
	if (pos_begin < str.length()) {
		ret.append(str.begin() + pos_begin, str.end());
	}
	ret.swap(str);
}


