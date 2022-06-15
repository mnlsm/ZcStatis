#pragma once
typedef std::string CStlString;
typedef std::vector<CStlString> CStlStrArray;
class CMiscHelper {

public:
	static UINT ShowInformDialog(HWND hWnd, UINT MB_Type, const wchar_t* fmt, ...);
	static UINT ShowInformDialog(HWND hWnd, const wchar_t*title, UINT MB_Type, const wchar_t* fmt, ...);

	static BOOL DoForceForegroundWindow(HWND hWnd);
	static void SplitCStringA(const CStringA& strSource, const CStringA& dim, std::vector<CStringA>& vecString, bool trim = true);
	static void SplitCStringW(const CStringW& strSource, const CStringW& dim, std::vector<CStringW>& vecString, bool trim = true);
	static BOOL DepartString(const CStlString& strTxt, const CStlString& strDim, CStlStrArray &arrPart, bool skipEmpty);

	static void DepartString(const CStringA& src, const CStringA& dim, CStringA& key, CStringA& val);
	static bool SetClipBoardText(const char* text, HWND hWnd);



	static CStringA getExePath();

public:
	enum
	{
		DIR_FILE_EXE = 0,
		DIR_FILE_MODULE,
		DIR_WINDOWS,
		DIR_SYSTEM,
		DIR_PROGRAM_FILES,
		DIR_IE_INTERNET_CACHE,
		DIR_COMMON_START_MENU,
		DIR_START_MENU,
		DIR_APP_DATA,
		DIR_PROFILE,
		DIR_LOCAL_APP_DATA,
		DIR_TEMP
	};

public:
	static bool PathProviderWin(int key, HMODULE this_module, wchar_t *buf, UINT bufsize);
	static UUID generateUUID();
	static UUID generateUUID(std::string *result);


public:
	static std::string  SysWideToUTF8(const wchar_t* wide);
	static std::string  SysWideToUTF8(const std::wstring& wide);

	static std::wstring SysUTF8ToWide(const char* utf8);
	static std::wstring SysUTF8ToWide(const std::string& utf8);

	static std::string  SysWideToNativeMB(const wchar_t* wide);
	static std::string  SysWideToNativeMB(const std::wstring& wide);

	static std::wstring SysNativeMBToWide(const char* native_mb);
	static std::wstring SysNativeMBToWide(const std::string& native_mb);

public:
	typedef struct tagDateTime
	{
		unsigned long year;
		unsigned long month;
		unsigned long day;
		unsigned long hour;
		unsigned long minute;
		unsigned long second;
		unsigned long millisecond;
	}DateTime, *LPDateTime;
	static bool ParseDateTime(const char* datetime_str, LPDateTime datetime);
	static std::string DateTimeToString(LPDateTime datetime);

public:
	static void string_replace(std::string& str, 
		const std::string& src, const std::string& dest);

	static BOOL ExecConsoleCmd(const CStringATL& cmd, CStringA& result, UINT second_timeout);
	static BOOL IsDirectoryExist(LPCTSTR lpszDirName);
	static BOOL IsFileExist(LPCTSTR lpszFileName);
	static void ListFiles(LPCTSTR Path, std::vector<CStringATL>& dirs, std::vector<CStringATL>& files);

private:
	static std::wstring SysMultiByteToWide(const char* mb, UINT code_page);
	static std::string  SysWideToMultiByte(const wchar_t* wide, UINT code_page);


};