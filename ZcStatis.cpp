// ZcStatis.cpp : main source file for ZcStatis.exe
//

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "Engine.h"

#include "Global.h"
#include "BasicExcel.hpp"

#include <deque>

CAppModule _Module;

//http://odds.500.com/europe_sfc.shtml
int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWMAXIMIZED)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	std::shared_ptr<CMainDlg> dlgMain = std::make_shared<CMainDlg>();

	if(dlgMain->Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain->ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

void proguard_gen(const std::vector<char>& src, std::deque<char>& tmp,
		std::vector<std::string>& result) {
	if (tmp.size() >= 3) {
		std::string temp;
		temp.assign(tmp.begin(), tmp.end()).append("qwk");
		result.push_back(temp);
		if (tmp.size() >= 10) {
			return;
		}
	}
	for (const auto& item : src) {
		tmp.push_back(item);
		proguard_gen(src, tmp, result);
		tmp.pop_back();
	}
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	/*
	std::vector<char> src = { '1', 'l', 'I' };
	std::deque<char> tmp;
	std::vector<std::string> result;
	proguard_gen(src, tmp, result);
	std::sort(result.begin(), result.end(), [](const std::string& A, const std::string& B) -> bool {
		return A.size() < B.size(); 
	});
	std::string lines;
	for (const auto& item : result) {
		lines.append(item).append("\n");
	}
	Global::SaveFileData("d:\\proguard.txt", lines, FALSE);
	*/
	//HRESULT hRes = ::CoInitialize(NULL);
	HRESULT hRes = OleInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	nCmdShow = SW_SHOWMAXIMIZED;
	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	//::CoUninitialize();
	OleUninitialize();
	return nRet;
}
