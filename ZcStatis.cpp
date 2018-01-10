// ZcStatis.cpp : main source file for ZcStatis.exe
//

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "Engine.h"

#include "Global.h"
#include "BasicExcel.hpp"
CAppModule _Module;

//http://odds.500.com/europe_sfc.shtml
int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{

	CStlString fn = Global::GetAppPath() + _T("odds\\18002_1.xls");
	using namespace YExcel;
	BasicExcel BE;
	if (BE.Load(fn.c_str()) && BE.GetTotalWorkSheets() > 0) {
		Worksheet* ws = BE.GetRawWorksheet(0);
		auto& cells = ws->mergedCells_.mergedCellsVector_;
		for (auto cell : cells) {
			std::string buffer;
			buffer.resize(cell.DataSize(), '\0');
			cell.Read((char*)buffer.data());
			Sleep(0);
		}

		/*
		std::ostringstream os;
		BEW->Print(os);
		std::string info = os.str();
		wchar_t* name = BEW->GetUnicodeSheetName();
		int rowCount = BEW->GetTotalRows();
		Sleep(0);
		*/
	}
	//using namespace xls;
	//WorkBook wb(fn);
	//xlsString name = wb.GetSheetName(0);
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
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

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	//::CoUninitialize();
	OleUninitialize();
	return nRet;
}
