#include "stdafx.h"
#include "JQCDialog.h"
#include "Global.h"
static const CStlString DZ_FILTER_NAME = _T("结果文件(*.txt)");
static const CStlString DZ_FILTER = _T("*.txt");

JQCDialog JQCDialog::sInst;
void JQCDialog::PopUp(const std::shared_ptr<SQLite::Database>& db) {
	if (!sInst.IsWindow()) {
		//sInst.m_pDatabase = db;
		sInst.Create(::GetDesktopWindow());
	}
	if (sInst.IsWindow()) {
		sInst.ShowWindow(SW_SHOWMAXIMIZED);
		::SetForegroundWindow(sInst.m_hWnd);
	}
}

void JQCDialog::Destroy() {
	if (sInst.IsWindow()) {
		sInst.DestroyWindow();
	}
}


JQCDialog::JQCDialog()
		:m_lstStatis(this, 1)
		,m_stInfo(this, 1) {
	httpMgr_.reset(new (std::nothrow) CHttpClientMgr());
	if (httpMgr_.get() != nullptr) {
		httpMgr_->Init();
	}
}

LRESULT JQCDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddIdleHandler(this);

	CenterWindow();
	InitControls();
	DlgResize_Init();

	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	int gapX = rcDesktop.Width() / 10;
	int gapY = rcDesktop.Height() / 10;
	rcDesktop.DeflateRect(gapX, gapY, gapX, gapY);
	SetWindowPos(NULL, &rcDesktop, SWP_NOZORDER);

	ReloadStatisDataFromLocal(0);
	return TRUE;
}

LRESULT JQCDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
	//pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left - 10;
	//pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top - 10;
	return 1L;
}

LRESULT JQCDialog::OnInitMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CMenuHandle menu((HMENU)wParam);
	//menu.RemoveMenu(IDM_ADDRECORD, MF_BYCOMMAND);
	CStringATL text;
	text.Format(_T("计算本期【%s】"), m_strQH);
	menu.ModifyMenu(IDM_ADDRECORD, MF_BYCOMMAND | MF_STRING, IDM_ADDRECORD, text);
	menu.RemoveMenu(IDM_JQC, MF_BYCOMMAND);
	menu.RemoveMenu(IDM_DANLUE, MF_BYCOMMAND);
	menu.RemoveMenu(IDM_OKOOO, MF_BYCOMMAND);
	menu.RemoveMenu(IDM_ZUCAI, MF_BYCOMMAND);
	menu.RemoveMenu(IDM_BEIDAN, MF_BYCOMMAND);
	menu.DeleteMenu(2, MF_BYPOSITION);
	menu.DeleteMenu(2, MF_BYPOSITION);

	return 1L;
}

LRESULT JQCDialog::OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstStatis.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}

LRESULT JQCDialog::OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstStatis.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}

LRESULT JQCDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	//EndDialog(0);
	//_Module.GetMessageLoop()->RemoveIdleHandler(this);
	ShowWindow(SW_HIDE);
	return 1L;
}

LRESULT JQCDialog::OnAddRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
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
	JQCEngine engine(strScript);
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

LRESULT JQCDialog::OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ReloadStatisDataFromLocal(0);
	return 1L;
}

void JQCDialog::InitControls() {
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

void JQCDialog::ReloadStatisDataFromLocal(int from) {
	CStlString strTextFile = Global::GetAppPath() + _T("jqc.txt");
	m_lstStatis.DeleteAllItems();
	m_localids.clear();
	if (PathFileExists(strTextFile.c_str())) {
		std::string filedate;
		Global::ReadFileData(strTextFile, filedate);
		CMiscHelper::string_replace(filedate, "\r\n", "\n");
		std::vector<CStlString> arrLines;
		Global::DepartString(filedate, _T("\n"), arrLines);
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
			m_localids.insert(std::make_pair(arrParts[0], line));
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
		if (from == 0) {
			doNetUpdateResults();
		}
	}
}

void JQCDialog::CreateWorkDir() {
	CStringATL strPath(Global::GetAppPath().c_str());
	strPath += _T("JQC");
	CStringATL strInitFileName;
	CreateDirectory(strPath, NULL);
	strPath = strPath + _T("\\") + m_strQH;
	CreateDirectory(strPath, NULL);
	m_strWorkDir = strPath;
}

void JQCDialog::DoSaveResult(JQCEngine& engine) {
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
