#pragma once
#include "StlFiles.h"
#include "tinyxml2.h"

//typedef void(*LFN_ComposeMultiSelectedProgress)(void* owner, int row_index, int row_max, int col_index, int col_max);




class Global {
public:
	static CStlString GetAppPath();
	static BOOL DepartString(const CStlString& strTxt, const CStlString& strDim, 
		CStlStrArray &arrPart);
	static BOOL DepartString(const CStlString& strTxt, const CStlString& strDim,
		bool trim, CStlStrArray& arrPart);

	static void ReplaceStringInStrArrayOnce(CStlStrArray& lines,
		const CStlString& old, const CStlString& n);

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
	static BOOL SaveFileData(const CStlString& filename, LPBYTE data, DWORD len, BOOL append);


	static CStlString toFixedLengthString(const CStlString& src, size_t fixed_length, bool right_align);
	static void getBiFenDateInfo(CStringATL& beginDay, CStringATL& endDay, CStringATL& beginWeekDay);

	static BOOL IsFileExist(LPCTSTR lpszFileName);

	static CStringATL GetFileName(LPCTSTR fullName);
	static CStringATL GetFileNameExt(LPCTSTR fileName);
	static CStringATL GetFilePath(LPCTSTR fullName);

	static CStringATL GetTimeString();
	static CStringATL GetNextDayString();
	static CStringATL GetUniqueCharStringGreater(const CStringATL& src);
	static CStringATL GetUniqueCharStringLess(const CStringATL& src);

	static CStringATL GetUniqueCharString(const CStringATL& src, bool greater);
	static size_t GetCharCount(const char* src, const char c);


	static bool ComposeMultiSelected(std::vector<std::map<std::string, std::string>>& items, 
		bool greater, const std::function<void(int,int,int,int)>& callback);

	static bool ComposeMultiSelected(std::vector<std::map<std::string, std::pair<int, std::string>>>& items, 
		bool greater);

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

CStringA GetElementAttrValue(tinyxml2::XMLElement* root, const CStringA& name);
CStringA GetElementClassAttrValue(tinyxml2::XMLElement* root);
CStringA GetElementText(tinyxml2::XMLElement* root);
tinyxml2::XMLElement* FindElementByClassAttr(tinyxml2::XMLElement* root, 
	const CStringA& class_value);


void OpenDirAndSelectFiles(const char* sFile, const std::vector<const char*>& lFilelist);

ULONG GetSecondsSince1970();

bool parse_json_string(const std::string& data, Json::Value& result);
bool json_get_string(const Json::Value& in, std::string* out);
bool json_get_object(const Json::Value& in, const std::string& k, Json::Value* out);
bool json_get_string(const Json::Value& in, const std::string& k, std::string* out);
bool json_get_int64(const Json::Value& in, const std::string& k, int64_t* out);
bool jsonarray_get_object(const Json::Value& in, size_t n, Json::Value* out);