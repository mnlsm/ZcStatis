#include "stdafx.h"
#include "DanLueDialog.h"
#include "Global.h"
static const CStlString DZ_FILTER_NAME = _T("结果文件(*.txt)");
static const CStlString DZ_FILTER = _T("*.txt");

DanLueDialog::DanLueDialog() : m_lstStatis(this, 1) {
	SYSTEMTIME tm = { 0 };
	GetLocalTime(&tm);
	m_strQH.Format("%04d%02d%02d", (int)tm.wYear, (int)tm.wMonth, (int)tm.wDay);
}

LRESULT DanLueDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CenterWindow();
	InitControls();
	DlgResize_Init();

	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	int gapX = rcDesktop.Width() / 10;
	int gapY = rcDesktop.Height() / 10;
	rcDesktop.DeflateRect(gapX, gapY, gapX, gapY);
	SetWindowPos(NULL, &rcDesktop, SWP_NOZORDER);

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

LRESULT DanLueDialog::OnInitMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CMenuHandle menu((HMENU)wParam);
	//menu.RemoveMenu(IDM_ADDRECORD, MF_BYCOMMAND);
	CStringATL text;
	text.Format(_T("计算本期【%s】"), m_strQH);
	menu.ModifyMenu(IDM_ADDRECORD, MF_BYCOMMAND | MF_STRING, IDM_ADDRECORD, text);
	menu.RemoveMenu(IDM_JQC, MF_BYCOMMAND);
	return 1L;
}

LRESULT DanLueDialog::OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstStatis.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}

LRESULT DanLueDialog::OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstStatis.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}

LRESULT DanLueDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	EndDialog(0);
	return 1L;
}

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

LRESULT DanLueDialog::OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ReloadStatisData();
	return 1L;
}

void DanLueDialog::InitControls() {
	DoDataExchange(FALSE);

	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);

	DWORD dwStyleEx = LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL;
	ListView_SetExtendedListViewStyle(m_lstStatis.m_hWnd, dwStyleEx);

	//insert header;
	int colIndex = 0;
	m_lstStatis.InsertColumn(colIndex, "期号", LVCFMT_CENTER, 100);    //70
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);

	m_lstStatis.InsertColumn(colIndex, "销量(万)", LVCFMT_CENTER, 100);    //70
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstStatis.InsertColumn(colIndex, "奖金(万)", LVCFMT_LEFT, 100);    //170
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_DOUBLE);

	m_lstStatis.InsertColumn(colIndex, "号 码", LVCFMT_CENTER, 100);    //270
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	m_lstStatis.InsertColumn(colIndex, "总进球", LVCFMT_CENTER, 85);    //70
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstStatis.InsertColumn(colIndex, "场进球", LVCFMT_CENTER, 100);    //70
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);
	//set sort type
	m_lstStatis.SetSortColumn(0);
}

void DanLueDialog::ReloadStatisData() {
	CStlString strTextFile = Global::GetAppPath() + _T("jqc.txt");
	if (PathFileExists(strTextFile.c_str())) {
		m_lstStatis.DeleteAllItems();
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
			iIndex = m_lstStatis.InsertItem(iIndex, arrParts[0].c_str());
			m_lstStatis.SetItemText(iIndex, ++colIndex, arrParts[3].c_str());
			m_lstStatis.SetItemText(iIndex, ++colIndex, arrParts[2].c_str());
			m_lstStatis.SetItemText(iIndex, ++colIndex, arrParts[1].c_str());
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
			m_lstStatis.SetItemText(iIndex, ++colIndex, strNum);
			sprintf(strNum.GetBuffer(255), "%02u-%02u-%02u-%02u", arrCodes[1] + arrCodes[0],
				arrCodes[3] + arrCodes[2], arrCodes[5] + arrCodes[4], arrCodes[7] + arrCodes[6]);
			strNum.ReleaseBuffer();
			m_lstStatis.SetItemText(iIndex, ++colIndex, strNum);
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
