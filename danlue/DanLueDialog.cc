#include "stdafx.h"
#include "DanLueDialog.h"
#include "Global.h"
//"http://appserver.87.cn/jc/match"

static const CStlString DZ_FILTER_NAME = _T("结果文件(*.txt)");
static const CStlString DZ_FILTER = _T("*.txt");

DanLueDialog DanLueDialog::sInst;
void DanLueDialog::PopUp() {
	if (!sInst.IsWindow()) {
		sInst.Create(::GetDesktopWindow());
	}
	if (sInst.IsWindow()) {
		sInst.ShowWindow(SW_SHOW);
		::SetForegroundWindow(sInst.m_hWnd);
	}
}

void DanLueDialog::Destroy() {
	if (sInst.IsWindow()) {
		sInst.DestroyWindow();
	}
}


DanLueDialog::DanLueDialog() : 
	m_lstMatch(this, 1),
	m_lstResult(this, 2),
	m_stYZM(this, 3),
	m_stBetArea(this, 4),
	m_buLogin(this, 100),
	m_buLogoff(this, 100),
	m_buRefresh(this, 100),
	m_buClear(this, 100),
	m_stBetAreaTitle(this, 100),
	m_buCalc(this, 100),
	m_stSep1(this, 100),
	m_stSep2(this, 100),
	m_buUpload(this, 100) 
{
	SYSTEMTIME tm = { 0 };
	GetLocalTime(&tm);
	m_strQH.Format("%04d%02d%02d", (int)tm.wYear, (int)tm.wMonth, (int)tm.wDay);
	CreateWorkDir();
}

LRESULT DanLueDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DoDataExchange(FALSE);

	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	int gapX = rcDesktop.Width() / 50;
	int gapY = rcDesktop.Height() / 50;
	rcDesktop.DeflateRect(gapX, gapY, gapX, gapY);
	SetWindowPos(NULL, &rcDesktop, SWP_NOZORDER);

	//CenterWindow();
	InitControls();

	ReloadStatisData();
	return TRUE;
}

LRESULT DanLueDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
	pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left - 10;
	pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top - 10;
	return 1L;
}

LRESULT DanLueDialog::OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstMatch.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}

LRESULT DanLueDialog::OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstMatch.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}

LRESULT DanLueDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ShowWindow(SW_HIDE);
	return 1L;
}
/*
LRESULT DanLueDialog::OnAddRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CStringATL strLuaFile;
	CStlString strScript, reason;
	strLuaFile.Format(_T("%s\\%s.lua"), m_strWorkDir, m_strQH);
	if (!PathFileExists(strLuaFile)) {
		MessageBox("Lua脚本文件读取失败 0！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	if (!Global::ReadFileData((LPCTSTR)strLuaFile, strScript) || strScript.empty()) {
		MessageBox("Lua脚本文件读取失败 1！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	DanLueEngine engine(strScript);
	if (engine.CalculateAllResult(reason)) {
		strLuaFile.Format(_T("【%s】计算完成， 共有【%u】注结果， 是否保存？"), m_strQH, engine.GetResult().size());
		if (MessageBox(strLuaFile, "完成", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			DoSaveResult(engine);
		}
	} else {
		strLuaFile.Format(_T("【%s】计算失败！"), m_strQH);
		MessageBox(strLuaFile, "错误", MB_ICONERROR | MB_OK);
	}
	return 1L;
}
*/


LRESULT DanLueDialog::OnLoginIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT DanLueDialog::OnLoginOff(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT DanLueDialog::OnCalc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT DanLueDialog::OnUpload(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT DanLueDialog::OnClearAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT DanLueDialog::OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ReloadStatisData();
	return 1L;
}

void DanLueDialog::InitControls() {
	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);

	DWORD dwStyleEx = LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL;
	ListView_SetExtendedListViewStyle(m_lstMatch.m_hWnd, dwStyleEx);
	ListView_SetExtendedListViewStyle(m_lstResult.m_hWnd, dwStyleEx);

	CRect rcItem, rcc;
	GetClientRect(rcc);

	m_lstResult.GetClientRect(rcItem);
	rcItem.right = rcc.right - 10;
	rcItem.bottom = rcc.bottom - 10;
	m_lstResult.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER | SWP_NOMOVE);

	m_stBetArea.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	int temp = rcItem.Width();
	m_stBetArea.SetWindowPos(NULL, rcc.right - 10 - temp, rcItem.top, temp, rcItem.Height(), SWP_NOZORDER | SWP_NOSIZE);
	temp = rcc.right - 10 - temp;
	m_stBetAreaTitle.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	m_stBetAreaTitle.SetWindowPos(NULL, temp, rcItem.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	m_lstMatch.GetClientRect(rcItem);
	rcItem.right = temp - 20;
	m_lstMatch.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER | SWP_NOMOVE);

	m_stSep1.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.left = 0;
	rcItem.right = rcc.right;
	m_stSep1.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER);

	m_stSep2.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.left = 0;
	rcItem.right = rcc.right;
	m_stSep2.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER);



	//insert header;
	int colIndex = 0;
	m_lstMatch.InsertColumn(colIndex, "期号", LVCFMT_CENTER, 100);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "赛事", LVCFMT_CENTER, 100);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "对阵", LVCFMT_CENTER, 250);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstMatch.InsertColumn(colIndex, "过期时间", LVCFMT_CENTER, 130);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "胜平负", LVCFMT_CENTER, 200);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstMatch.InsertColumn(colIndex, "让胜平负", LVCFMT_CENTER, 200);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	m_lstMatch.SetSortColumn(0);

	m_lstResult.GetClientRect(rcItem);
	colIndex = 0;
	m_lstResult.InsertColumn(colIndex, "编号", LVCFMT_CENTER, 70);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstResult.InsertColumn(colIndex, "奖金(元)", LVCFMT_CENTER, 90);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstResult.InsertColumn(colIndex, "结果", LVCFMT_CENTER, rcItem.Width() - 160);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	//set sort type
}

void DanLueDialog::ReloadStatisData() {
	return;
	CStlString strTextFile = Global::GetAppPath() + _T("jqc.txt");
	if (PathFileExists(strTextFile.c_str())) {
		m_lstMatch.DeleteAllItems();
		std::string filedate;
		Global::ReadFileData(strTextFile, filedate);
		std::vector<CStlString> arrLines;
		Global::DepartString(filedate, _T("\r\n"), arrLines);
		int iIndex = 0, maxQH = 0;
		for (const auto& line : arrLines) {
			int colIndex = 0;
			std::vector<CStlString> arrParts;
			Global::DepartString(line, _T(","), arrParts);
			if (arrParts.size() != 4) {
				continue;
			}
			if (arrParts[1].size() != 8) {
				continue;
			}
			int qh = _ttol(arrParts[0].c_str());
			if (qh > maxQH) {
				maxQH = qh;
			}
			iIndex = m_lstMatch.InsertItem(iIndex, arrParts[0].c_str());
			m_lstMatch.SetItemText(iIndex, ++colIndex, arrParts[3].c_str());
			m_lstMatch.SetItemText(iIndex, ++colIndex, arrParts[2].c_str());
			m_lstMatch.SetItemText(iIndex, ++colIndex, arrParts[1].c_str());
			CStlString& codes = arrParts[1];
			CIntArray arrCodes;
			int sumAll = 0;
			for (int j = 0; j < 8; j++) {
				int code = (codes[j] - _T('0'));
				sumAll += code;
				arrCodes.push_back(code);
			}
			CStringATL strNum;
			sprintf(strNum.GetBuffer(255), "%u", sumAll);
			strNum.ReleaseBuffer();
			m_lstMatch.SetItemText(iIndex, ++colIndex, strNum);
			sprintf(strNum.GetBuffer(255), "%02u-%02u-%02u-%02u", arrCodes[1] + arrCodes[0],
				arrCodes[3] + arrCodes[2], arrCodes[5] + arrCodes[4], arrCodes[7] + arrCodes[6]);
			strNum.ReleaseBuffer();
			m_lstMatch.SetItemText(iIndex, ++colIndex, strNum);
		}
		m_strQH.Format(_T("%u"), maxQH + 1);
		CreateWorkDir();
	}
}

void DanLueDialog::CreateWorkDir() {
	CStringATL strPath(Global::GetAppPath().c_str());
	strPath += _T("JC");
	CStringATL strInitFileName;
	CreateDirectory(strPath, NULL);
	strPath = strPath + _T("\\") + m_strQH;
	CreateDirectory(strPath, NULL);
	m_strWorkDir = strPath;
}

void DanLueDialog::DoSaveResult(DanLueEngine& engine) {
	CStlString strResult;
	engine.GetResultString(strResult);
	if (strResult.empty()) {
		return;
	}
	TCHAR szFilterName[30] = { _T('\0') };
	_tcscpy(szFilterName, DZ_FILTER_NAME.c_str());
	_tcscat(szFilterName + DZ_FILTER_NAME.length() + 1, DZ_FILTER.c_str());
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilterName, m_hWnd);
	dlg.m_ofn.lpstrInitialDir = m_strWorkDir;
	CStringATL strInitFileName;
	strInitFileName.Format(_T("%s.txt"), m_strQH);
	TCHAR szFileName[MAX_PATH + 1] = { _T('\0') };
	_tcscpy(szFileName, strInitFileName);
	dlg.m_ofn.lpstrFile = szFileName;
	if (dlg.DoModal() != IDOK) {
		return;
	}
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	std::string utf8 = Global::toUTF8((LPCTSTR)strResult.c_str());
	Global::SaveFileData(strLoadPath, utf8, FALSE);
}
