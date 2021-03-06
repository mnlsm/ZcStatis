#pragma once
#include "StlFiles.h"

class Global {
public:
	static CStlString GetAppPath();
	static BOOL DepartString(const CStlString& strTxt, const CStlString& strDim, 
		CStlStrArray &arrPart);

#ifdef _UNICODE
	static BOOL DepartString(const std::string& strTxt, const std::string& strDim,
		std::vector<std::string> &arrPart);
#endif

	static void TrimString(CStlString& str, TCHAR a);
	static void TrimBlank(CStlString& str);

	static std::string toUTF8(const CStlString& local);
	static CStlString fromUTF8(const std::string& utf8);
	
	static BOOL ReadFileData(const CStlString& filename, std::string& filedata);
	static BOOL SaveFileData(const CStlString& filename, const std::string& filedata, BOOL append);

	static CStlString toFixedLengthString(const CStlString& src, size_t fixed_length, bool right_align);
	static void getBiFenDateInfo(CStringATL& beginDay, CStringATL& endDay, CStringATL& beginWeekDay);
private:
	static std::string  SysWideToUTF8(const wchar_t* wide);
	static std::string  SysWideToUTF8(const std::wstring& wide);
	static std::wstring SysUTF8ToWide(const char* utf8);
	static std::wstring SysUTF8ToWide(const std::string& utf8);
	static std::string  SysWideToNativeMB(const wchar_t* wide);
	static std::string  SysWideToNativeMB(const std::wstring& wide);
	static std::wstring SysNativeMBToWide(const char* native_mb);
	static std::wstring SysNativeMBToWide(const std::string& native_mb);

	static std::wstring SysMultiByteToWide(const char* mb, UINT code_page);
	static std::string  SysWideToMultiByte(const wchar_t* wide, UINT code_page);
};