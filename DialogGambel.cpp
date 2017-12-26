#include "stdafx.h"
#include "DialogGambel.h"
#include "DialogTZ.h"
#include "Global.h"
#include "EngineLua.h"

static const CStlString DZ_FILTER_NAME = _T("单注文件(*.dz)");
static const CStlString DZ_FILTER = _T("*.dz");

static const CStlString LUA_FILTER_NAME = _T("脚本文件(*.lua)");
static const CStlString LUA_FILTER = _T("*.lua");

CDialogGambel::CDialogGambel(std::shared_ptr<SQLite::Database> db, const CStlString& qh) 
	: m_lstGambel(this, 1), 
	  m_edOutput(this, 2),
	  m_edSearch(this, 3) {
	m_pDatabase = db;
	m_strQH = qh;

	m_pPrintInfo.reset(new (std::nothrow) CMyPrintInfo());
	m_pPrinter.reset(new (std::nothrow) CPrinter());
	m_wndPreview.reset(new (std::nothrow) CPrintWnd());

	CreateWorkDir();
}

LRESULT CDialogGambel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CenterWindow();
	InitControls();
	ReloadFangAnData();
	return 1L;
}

LRESULT CDialogGambel::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if (m_wndPreview->IsWindow()) {
		m_wndPreview->DestroyWindow();
	}
	bHandled = FALSE;
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CAutoWaitCursor waiter;
	CStlString strCodes(TOTO_COUNT, _T('\0'));
	CStringATL strOutput;
	ClearOutputText();
	int code_length = m_edSearch.GetWindowText((LPTSTR)strCodes.data(), strCodes.size() + 1);
	if (code_length != TOTO_COUNT) {
		strOutput.Format(_T("搜索串长度错误, (%d)"), code_length);
		AppendOutputText(strOutput);
		return 1L;
	}
	for (const auto& code : strCodes) {
		if (code != _T('0') && code != _T('1') && code != _T('3')) {
			strOutput = _T("搜索格式错误!");
			AppendOutputText(strOutput);
			return 1L;
		}
	}
	if (m_arrDbData.empty()) {
		strOutput = _T("没有任何方案!");
		AppendOutputText(strOutput);
		return 1L;
	}
	CIntArray arrRecord;
	CEngine::GetRecord(strCodes, arrRecord);
	for (const auto& data : m_arrDbData) {
		if (data.m_nInUse != 1) {
			strOutput.Format(_T("方案(%d)未启用"), data.m_nID);
			AppendOutputText(strOutput);
			continue;
		}
		
		if (data.m_strResult.IsEmpty()) {
			strOutput.Format(_T("方案(%d)未计算结果"), data.m_nID);
			AppendOutputText(strOutput);
			continue;
		}

		std::unique_ptr<CEngine> pEngine;
		if (data.m_strScript.IsEmpty()) {
			pEngine.reset(new CEngine());
		}
		else {
			pEngine.reset(new CEngineLua((LPCTSTR)data.m_strScript));
		}

		strOutput.Format(_T("方案[%d]搜索开始:\r\n"), data.m_nID);
		AppendOutputText(strOutput);
		pEngine->SetDZRecords((LPCTSTR)data.m_strResult);
		pEngine->SetPL((LPCTSTR)data.m_strPL);
		CStlString reason;
		pEngine->IsAValidRecord(arrRecord, reason);
		if (!reason.empty()) {
			AppendOutputText(reason.c_str());
		}
		strOutput.Format(_T("方案[%d]搜索结束\r\n"), data.m_nID);
		AppendOutputText(strOutput);
	}
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuAddDanShi(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	TCHAR szFilterName[30] = { _T('\0') };
	_tcscpy(szFilterName, DZ_FILTER_NAME.c_str());
	_tcscat(szFilterName + DZ_FILTER_NAME.length() + 1, DZ_FILTER.c_str());
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilterName, m_hWnd);
	dlg.m_ofn.lpstrInitialDir = m_strWorkDir;
	if (dlg.DoModal() != IDOK) {
		return 1L;
	}
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	std::string filedata;
	if (!Global::ReadFileData(strLoadPath, filedata) || filedata.size() == 0) {
		MessageBox("单注文件读取失败！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	CStlString strCodes = Global::fromUTF8(filedata);
	CStlString strPL, strMatchs;
	CStringATL strSQL;
	strSQL.Format(_T("SELECT PLDATA, MATCHS FROM PLDATA WHERE ID='%s'"), m_strQH.c_str());
	SQLite::Statement sm(*m_pDatabase, strSQL);
	if (sm.executeStep()) {
		strPL = sm.getColumn(0).getString();
		strMatchs = sm.getColumn(1).getString();
	}

	strSQL = _T("INSERT INTO GAMBEL (QH, INUSE, CODESTYPE, CODES, PLDATA, MATCHS) VALUES(?,?,?,?,?,?)");
	SQLite::Statement sm1(*m_pDatabase, strSQL);
	sm1.bindNoCopy(1, m_strQH);
	sm1.bind(2, 0);
	sm1.bind(3, 1);
	sm1.bindNoCopy(4, strCodes);
	sm1.bindNoCopy(5, strPL);
	sm1.bindNoCopy(6, strMatchs);
	if (sm1.exec() <= 0) {
		MessageBox("INSERT DB 失败！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	ReloadFangAnData();
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuAddFuShi(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CDialogTZ dlg(m_pDatabase, m_strQH);
	dlg.DoModal();
	if (dlg.IsDbDataChanged()) {
		ReloadFangAnData();
	}
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuPreview(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	m_wndPreview->Hide();
	CStlStrArray arrS;
	CIntxyArray arrAllRecord;
	for (const auto& data : m_arrDbData) {
		if (data.m_nInUse != 1) {
			continue;
		}
		CIntxyArray arrRecords;
		if (CEngine::GetRecords((LPCTSTR)data.m_strResult, arrRecords)) {
			for (const auto& r : arrRecords) {
				arrAllRecord.push_back(r);
			}
		}
	}
	if (arrAllRecord.empty()) {
		MessageBox(_T("没有任何投注"), "提示", MB_ICONINFORMATION | MB_OK);
		return 1L;
	}
	std::stable_sort(arrAllRecord.begin(), arrAllRecord.end());
	arrAllRecord.erase(std::unique(arrAllRecord.begin(), arrAllRecord.end()), arrAllRecord.end());
	CEngine::GetRecordsPrintRecords(arrAllRecord, arrS);
	CPrintDialog dlg(false);
	dlg.DoModal();
	m_pPrintInfo->setData(arrS);
	m_pPrinter->ClosePrinter();

	CStringATL strName = dlg.GetDeviceName();
	if (m_pPrinter->OpenPrinter(strName))
	{
		CDC dcPrinter;
		dcPrinter.Attach(m_pPrinter->CreatePrinterDC(NULL));
		m_pPrintInfo->BeginPrintJob(dcPrinter);
		dcPrinter.DeleteDC();
		m_wndPreview->SetPrintPreviewInfo(*m_pPrinter, NULL, m_pPrintInfo.get(),
			m_pPrintInfo->GetMinPage(),
			m_pPrintInfo->GetMaxPage());
		m_wndPreview->SetPage(1);
		m_wndPreview->ShowWindow(SW_SHOWMAXIMIZED);
		m_wndPreview->UpdateWindow();
	}
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuPrint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CStlStrArray arrS;
	CIntxyArray arrAllRecord;
	for (const auto& data : m_arrDbData) {
		if (data.m_nInUse != 1) {
			continue;
		}
		CIntxyArray arrRecords;
		if (CEngine::GetRecords((LPCTSTR)data.m_strResult, arrRecords)) {
			for (const auto& r : arrRecords) {
				arrAllRecord.push_back(r);
			}
		}
	}
	if (arrAllRecord.empty()) {
		MessageBox(_T("没有任何投注"), "提示", MB_ICONINFORMATION | MB_OK);
		return 1L;
	}
	std::stable_sort(arrAllRecord.begin(), arrAllRecord.end());
	arrAllRecord.erase(std::unique(arrAllRecord.begin(), arrAllRecord.end()), arrAllRecord.end());

	CPrintDialog dlg(false);
	dlg.DoModal();
	m_pPrintInfo->setData(arrS);
	m_pPrinter->ClosePrinter();

	CStringATL strName = dlg.GetDeviceName();
	if (m_pPrinter->OpenPrinter(strName)) {
		CPrintJob job;
		job.StartPrintJob(false, *m_pPrinter, NULL, m_pPrintInfo.get(),
			" ", m_pPrintInfo->GetMinPage(),
			m_pPrintInfo->GetMaxPage());

	}
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	EndDialog(0);
	return 1L;
}

LRESULT CDialogGambel::OnListItemChanged(int wParam, LPNMHDR lParam, BOOL& bHandled) {
	LPNMLISTVIEW p = (LPNMLISTVIEW)lParam;
	if (p != NULL && p->uOldState != 0) {
		UINT uCheckState = (p->uNewState & LVIS_STATEIMAGEMASK) >> 12;
		DoRowInUse(p->iItem, (uCheckState == 2));
	}
	bHandled = FALSE;
	return 1L;
}

LRESULT CDialogGambel::OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstGambel.DefWindowProc(uMsg, wParam, lParam);
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LVHITTESTINFO lvh = { 0 };
	lvh.pt = pt;
	UINT index = m_lstGambel.HitTest(&lvh);
	if (index != -1 && lvh.flags != LVHT_ONITEMSTATEICON) {
		CMenu menu;
		if (menu.CreatePopupMenu()) {
			menu.AppendMenu(MF_STRING, IDM_CACL_RESULT, _T("计算结果"));
			menu.AppendMenu(MF_STRING, IDM_SAVE_RESULT, _T("保存结果"));
			menu.AppendMenu(MF_STRING, IDM_DELETE_RESULT, _T("删除结果"));
			menu.AppendMenu(MF_SEPARATOR);

			menu.AppendMenu(MF_STRING, IDM_EDIT_CODES, _T("更新选择"));
			menu.AppendMenu(MF_SEPARATOR);

			menu.AppendMenu(MF_STRING, IDM_EDIT_SCRIPT, _T("更新脚本"));
			menu.AppendMenu(MF_STRING, IDM_COPY_SCRIPT, _T("复制脚本"));
			menu.AppendMenu(MF_STRING, IDM_DELETE_SCRIPT, _T("删除脚本"));
			menu.AppendMenu(MF_SEPARATOR);

			menu.AppendMenu(MF_STRING, IDM_COPY_ROW, _T("复制方案"));
			menu.AppendMenu(MF_STRING, IDM_DELETE_ROW, _T("删除方案"));
			m_lstGambel.ClientToScreen(&pt);
			UINT cmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, 
				pt.x, pt.y, m_hWnd);
			DoListMenuCommand(cmd, index);
			menu.DestroyMenu();
		}
	}
	return lRet;
}

void CDialogGambel::DoListMenuCommand(UINT cmd, UINT nItem) {
	CStringATL strID;
	m_lstGambel.GetItemText(nItem, 0, strID);
	int nID = _ttoi(strID);
	const DataRow *pData = NULL;
	for (const auto& row : m_arrDbData) {
		if (nID == row.m_nID) {
			pData = &row;
			break;
		}
	}
	if (pData == NULL) {
		return;
	}
	if (cmd == IDM_CACL_RESULT) {
		DoCalcResult(*pData);
	} else if (cmd == IDM_SAVE_RESULT) {
		DoSaveResult(*pData);
	} else if (cmd == IDM_DELETE_RESULT) {
		DoDeleteResult(*pData);
	} else if (cmd == IDM_EDIT_CODES) {
		DoEditCodes(*pData);
	} else if (cmd == IDM_EDIT_SCRIPT) {
		DoEditScript(*pData);
	} else if (cmd == IDM_COPY_SCRIPT) {
		DoCopyScript(*pData);
	} else if (cmd == IDM_DELETE_SCRIPT) {
		DoDeleteScript(*pData);
	} else if (cmd == IDM_DELETE_ROW) {
		DoDeleteRow(*pData);
	} else if (cmd == IDM_COPY_ROW) {
		DoCopyRow(*pData);
	}
}

void CDialogGambel::InitControls() {
	DoDataExchange(FALSE);

	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR|LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR|LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);
	
	m_edSearch.SetLimitText(TOTO_COUNT);

	DWORD dwStyleEx = ListView_GetExtendedListViewStyle(m_lstGambel.m_hWnd);
	dwStyleEx |= (LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL);
	ListView_SetExtendedListViewStyle(m_lstGambel.m_hWnd, dwStyleEx);
	int colIndex = 0;
	m_lstGambel.InsertColumn(colIndex++, "编号", LVCFMT_LEFT, 65);    //70
	m_lstGambel.InsertColumn(colIndex++, "投注类型", LVCFMT_CENTER, 65);    //70
	m_lstGambel.InsertColumn(colIndex++, "投注数据", LVCFMT_LEFT, 380);    //70
	m_lstGambel.InsertColumn(colIndex++, "脚本数据", LVCFMT_LEFT, 220);    //70
	m_lstGambel.InsertColumn(colIndex++, "结果数据", LVCFMT_LEFT, 100);    //70
	m_lstGambel.InsertColumn(colIndex++, "结果个数", LVCFMT_CENTER, 100);    //70

	CStringATL strTemp;         
	strTemp.Format(_T("方案列表(%s):"), m_strQH.c_str());
	CWindow wnd = GetDlgItem(IDC_STGAMBELS);
	wnd.SetWindowText(strTemp);

	CStringATL strSQL;
	strSQL.Format(_T("SELECT RESULT FROM PLDATA WHERE ID='%s' ORDER BY ID ASC"), m_strQH.c_str());
	SQLite::Statement sm(*m_pDatabase, strSQL);
	if (sm.executeStep()) {
		CStlString result = sm.getColumn(0).getString();
		if (!result.empty()) {
			m_edSearch.SetWindowText(result.c_str());
		}
	}
}



void CDialogGambel::ReloadFangAnData() {
	m_arrDbData.clear();
	m_lstGambel.DeleteAllItems();
	ClearOutputText();
	CStringATL strSQL;
	strSQL.Format(_T("SELECT ID, INUSE, CODESTYPE, CODES, PLDATA, MATCHS, SCRIPT,")
		_T(" RESULT, RESULT9 FROM GAMBEL WHERE QH='%s' ORDER BY ID ASC"), m_strQH.c_str());
	SQLite::Statement sm(*m_pDatabase, strSQL);
	while (sm.executeStep()) {
		DataRow row;
		row.m_nID = sm.getColumn(0).getInt();
		row.m_nInUse = sm.getColumn(1).getInt();
		row.m_nCodesType = sm.getColumn(2).getInt();
		row.m_strCodes = sm.getColumn(3).getString().c_str();
		row.m_strPL = sm.getColumn(4).getString().c_str();
		row.m_strMatchs = sm.getColumn(5).getString().c_str();
		row.m_strScript = Global::fromUTF8(sm.getColumn(6).getString()).c_str();
		row.m_strResult = sm.getColumn(7).getString().c_str();
		row.m_strResult9 = sm.getColumn(8).getString().c_str();
		m_arrDbData.push_back(row);
	}

    CStringATL strNum;
	UINT rowIndex = 0;
	for (const auto& row : m_arrDbData) {
		strNum.Format(_T("%u"), row.m_nID);
		rowIndex = m_lstGambel.InsertItem(rowIndex, strNum);
		UINT colIndex = 0;
		if (row.m_nCodesType == 0) {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, _T("复式"));
			m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strCodes);
		} else if (row.m_nCodesType == 1) {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, _T("单式"));
			m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strCodes.Left(15) + _T(" ......"));
		} else if (row.m_nCodesType == 2) {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, _T("复盘"));
			m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strCodes.Left(15) + _T(" ......"));
		}

		CStringATL strScript = row.m_strScript.Left(row.m_strScript.Find(_T("\n")));
		strScript.TrimRight();
		strScript.Replace(_T("--"), _T(""));
		m_lstGambel.SetItemText(rowIndex, ++colIndex, strScript);

		++colIndex;
		if (!row.m_strResult.IsEmpty()) {
			m_lstGambel.SetItemText(rowIndex, colIndex, row.m_strResult.Left(15) + _T(" ......"));
		}
		strNum.Format(_T("%u"), CEngine::GetRecordsCount((LPCTSTR)row.m_strResult));
		m_lstGambel.SetItemText(rowIndex, ++colIndex, strNum);
		m_lstGambel.SetCheckState(rowIndex, (row.m_nInUse == 1));
	}
	::SetForegroundWindow(m_hWnd);
}

void CDialogGambel::AppendOutputText(const TCHAR* text) {
	CStringATL oldText;
	m_edOutput.GetWindowText(oldText);
	if (oldText.GetLength() > 60000) {
		oldText.Empty();
	}
	if (!oldText.IsEmpty()) {
		oldText = oldText + _T("\r\n") + text;
	} else {
		oldText = text;
	}
	m_edOutput.SetWindowText(oldText);
}

void CDialogGambel::ClearOutputText() {
	//m_edOutput.Clear();
	m_edOutput.SetWindowText(_T(""));
	m_edOutput.RedrawWindow(NULL, NULL);
}

void CDialogGambel::DoEditCodes(const DataRow& data) {
	if (data.m_nCodesType == 1) {
		TCHAR szFilterName[30] = { _T('\0') };
		_tcscpy(szFilterName, DZ_FILTER_NAME.c_str());
		_tcscat(szFilterName + DZ_FILTER_NAME.length() + 1, DZ_FILTER.c_str());
		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			szFilterName, m_hWnd);
		dlg.m_ofn.lpstrInitialDir = m_strWorkDir;
		if (dlg.DoModal() != IDOK) {
			return;
		}
		CStlString strLoadPath = dlg.m_ofn.lpstrFile;
		std::string filedata;
		if (!Global::ReadFileData(strLoadPath, filedata) || filedata.size() == 0) {
			MessageBox("单注文件读取失败！", "错误", MB_ICONERROR | MB_OK);
			return;
		}
		CStlString strCodes = Global::fromUTF8(filedata);
		CStringATL strSQL = _T("UPDATE GAMBEL SET CODESTYPE=?, CODES=?, RESULT=? WHERE ID=?");
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bind(1, 1);
		sm.bindNoCopy(2, strCodes);
		sm.bindNoCopy(3, _T(""));
		sm.bind(4, data.m_nID);
		if (sm.exec() > 0) {
			ReloadFangAnData();
		}
	} else if (data.m_nCodesType == 0) {
		CDialogTZ dlg(m_pDatabase, m_strQH, data.m_nID);
		dlg.DoModal();
		if (dlg.IsDbDataChanged()) {
			ReloadFangAnData();
		}
	}
}

void CDialogGambel::DoEditScript(const DataRow& data) {
	std::string filedata;
	TCHAR szFilterName[30] = { _T('\0') };
	_tcscpy(szFilterName, LUA_FILTER_NAME.c_str());
	_tcscat(szFilterName + LUA_FILTER_NAME.length() + 1, LUA_FILTER.c_str());
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilterName, m_hWnd);
	dlg.m_ofn.lpstrInitialDir = m_strWorkDir;
	CStringATL strInitFileName;
	strInitFileName.Format(_T("%s_%d.lua"), m_strQH.c_str(), data.m_nID);
	TCHAR szFileName[MAX_PATH + 1] = { _T('\0') };
	_tcscpy(szFileName, strInitFileName);
	dlg.m_ofn.lpstrFile = szFileName;
	CStlString strFileFullName = m_strWorkDir + _T("\\") + szFileName;
	DWORD dwFileAttributes = ::GetFileAttributes(strFileFullName.c_str());
	if (dwFileAttributes == INVALID_FILE_ATTRIBUTES || 
			(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		Global::SaveFileData(strFileFullName, "", FALSE);
	}
	if (dlg.DoModal() != IDOK) {
		return;
	}
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	if (!Global::ReadFileData(strLoadPath, filedata) || filedata.size() == 0) {
		MessageBox("Script文件读取失败！", "错误", MB_ICONERROR | MB_OK);
		return;
	}
	
	/*
	if (!OpenClipboard()) {
		MessageBox("打开剪贴板失败！", "错误", MB_ICONERROR | MB_OK);
		return;
	}
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData != NULL) {
		filedata = (char*)GlobalLock(hData);
		GlobalUnlock(hData);
	}
	CloseClipboard();
	if (filedata.size() == 0) {
		MessageBox("更新Script脚本失败！", "错误", MB_ICONERROR | MB_OK);
		return;
	}
	*/
	CStlString strScript = Global::fromUTF8(filedata);
	CStringATL strSQL = _T("UPDATE GAMBEL SET SCRIPT=? WHERE ID=?");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	sm.bindNoCopy(1, strScript);
	sm.bind(2, data.m_nID);
	if (sm.exec() > 0) {
		ReloadFangAnData();
	}
}

void CDialogGambel::DoCopyScript(const DataRow& data) {
	if (OpenClipboard()) {
		EmptyClipboard();
		if (data.m_strScript.IsEmpty()) {
			return;
		}
		std::string scriptUtf8 = Global::toUTF8((LPCTSTR)data.m_strScript);
		HGLOBAL hGlobal = GlobalAlloc(GHND, scriptUtf8.size() + 1);
		if (hGlobal != NULL) {
			char* buffer = (char*)GlobalLock(hGlobal);
			memcpy(buffer, scriptUtf8.data(), scriptUtf8.size());
			GlobalUnlock(hGlobal);
			SetClipboardData(CF_TEXT, hGlobal);
		}
		CloseClipboard();
	}
}

void CDialogGambel::DoDeleteScript(const DataRow& data) {
	CStringATL strSQL = _T("UPDATE GAMBEL SET SCRIPT=? WHERE ID=?");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	sm.bind(1, _T(""));
	sm.bind(2, data.m_nID);
	if (sm.exec() > 0) {
		ReloadFangAnData();
	}
}

void CDialogGambel::DoSaveResult(const DataRow& data) {
	if (data.m_strResult.IsEmpty()) {
		return;
	}
	TCHAR szFilterName[30] = { _T('\0') };
	_tcscpy(szFilterName, DZ_FILTER_NAME.c_str());
	_tcscat(szFilterName + DZ_FILTER_NAME.length() + 1, DZ_FILTER.c_str());
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilterName, m_hWnd);
	dlg.m_ofn.lpstrInitialDir = m_strWorkDir;
	CStringATL strInitFileName;
	strInitFileName.Format(_T("%s_%d.dz"), m_strQH.c_str(), data.m_nID);
	TCHAR szFileName[MAX_PATH + 1] = { _T('\0') };
	_tcscpy(szFileName, strInitFileName);
	dlg.m_ofn.lpstrFile = szFileName;
	if (dlg.DoModal() != IDOK) {
		return;
	}
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	std::string utf8 = Global::toUTF8((LPCTSTR)data.m_strResult);
	Global::SaveFileData(strLoadPath, utf8, FALSE);

	utf8 = Global::toUTF8((LPCTSTR)data.m_strResult9);
	if (!utf8.empty()) {
		for (auto& code : utf8) {
			if (code == _T('8')) {
				code = '#';
			}
		}
		Global::SaveFileData(strLoadPath + _T("9"), utf8, FALSE);
	}
}

void CDialogGambel::DoDeleteResult(const DataRow& data) {
	CStringATL strSQL = _T("UPDATE GAMBEL SET RESULT=?, RESULT9=? WHERE ID=?");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	sm.bind(1, _T(""));
	sm.bind(2, _T(""));
	sm.bind(3, data.m_nID);
	if (sm.exec() > 0) {
		ReloadFangAnData();
	}
}

void CDialogGambel::DoDeleteRow(const DataRow& data) {
	CStringATL strASK;
	strASK.Format(_T("是否要删除方案(%d) ?"), data.m_nID);
	if (MessageBox(strASK, "请确认", MB_ICONASTERISK | MB_YESNO) != IDYES) {
		return;
	}
	CStringATL strSQL = _T("DELETE FROM GAMBEL WHERE ID=?");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	sm.bind(1, data.m_nID);
	if (sm.exec() > 0) {
		ReloadFangAnData();
	}
}

void CDialogGambel::DoCopyRow(const DataRow& data) {
	CStringATL strSQL;
	strSQL = _T("INSERT INTO GAMBEL (QH, INUSE, CODESTYPE, CODES, PLDATA, MATCHS, SCRIPT) VALUES(?,?,?,?,?,?,?)");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	sm.bindNoCopy(1, m_strQH);
	sm.bind(2, 0);
	sm.bind(3, data.m_nCodesType);
	sm.bindNoCopy(4, data.m_strCodes);
	sm.bindNoCopy(5, data.m_strPL);
	sm.bindNoCopy(6, (LPCTSTR)data.m_strMatchs);
	sm.bindNoCopy(7, data.m_strScript);
	if (sm.exec() > 0) {
		ReloadFangAnData();
	}
}


void CDialogGambel::DoRowInUse(UINT uItem, BOOL inuse) {
	int nInUse = inuse ? 1 : 0;
	CStringATL strID;
	m_lstGambel.GetItemText(uItem, 0, strID);
	int nID = _ttoi(strID);
	DataRow *pData = NULL;
	for (auto& row : m_arrDbData) {
		if (nID == row.m_nID) {
			pData = &row;
			break;
		}
	}
	nInUse = m_lstGambel.GetCheckState(uItem);
	if (pData == NULL || nInUse == pData->m_nInUse) {
		return;
	}
	pData->m_nInUse = nInUse;
	CStringATL strSQL = _T("UPDATE GAMBEL SET INUSE=? WHERE ID=?");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	sm.bind(1, pData->m_nInUse);
	sm.bind(2, pData->m_nID);
	if (sm.exec()) {
		//ReloadFangAnData();
	}
}

void CDialogGambel::DoCalcResult(const DataRow& data) {
	ClearOutputText();
	CAutoWaitCursor waiter;
	CStlString reason;
	CStringATL strMsg;
	if (data.m_nInUse != 1) {
		strMsg.Format(_T("(%d)请先启用方案"), data.m_nID);
		AppendOutputText(strMsg);
		return;
	}
	if (data.m_strCodes.IsEmpty()) {
		strMsg.Format(_T("(%d)请先选好号码"), data.m_nID);
		AppendOutputText(strMsg);
		return;
	}
	std::unique_ptr<CEngine> pEngine;
	if (data.m_strScript.IsEmpty()) {
		pEngine.reset(new CEngine());
	} else {
		pEngine.reset(new CEngineLua((LPCTSTR)data.m_strScript));
	}
	if (data.m_nCodesType == 0) {
		pEngine->SetChoices((LPCTSTR)data.m_strCodes);
	} else if (data.m_nCodesType == 1) {
		pEngine->SetDZRecords((LPCTSTR)data.m_strCodes);
	} else {
		strMsg.Format(_T("(%d)未知号码集合"), data.m_nID);
		AppendOutputText(strMsg);
		return;
	}
	pEngine->SetPL((LPCTSTR)data.m_strPL);
	if (!pEngine->CalculateAllResult(reason)) {
		strMsg.Format(_T("(%d)计算错误, 原因: %s"), data.m_nID, reason.c_str());
		AppendOutputText(strMsg);
		return;
	}
	CStlString strResult, strResult9;
	CEngine::GetRecordsString(pEngine->GetResult(), strResult);
	CEngine::GetRecordsString(pEngine->GetResult9(), strResult9);
	const int max_size = 1024 * 1024 - 1;
	if (strResult.size() < max_size) {
		CStringATL strSQL = _T("UPDATE GAMBEL SET RESULT=? , RESULT9=? WHERE ID=?");
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bindNoCopy(1, strResult);
		sm.bindNoCopy(2, strResult9);
		sm.bind(9, data.m_nID);
		if (sm.exec() > 0) {
			ReloadFangAnData();
		}
	}

	strMsg.Format(_T("[%d]计算完成, 共有%u(%u--->%u)注结果, 分布数据如下:\r\n"), 
		data.m_nID, pEngine->GetResult().size(), 
		pEngine->GetInitRecordsCount(), pEngine->GetAllRecord().size());
	AppendOutputText(strMsg);

	CStlStrArray arrPLs;
	Global::DepartString((LPCTSTR)data.m_strPL, _T("#"), arrPLs);

	const int* fenbu = pEngine->GetResultFenBu();
	for (int i = 0; i < TOTO_COUNT * 3; i = i + 3) {
		const int *pos = fenbu + i;
		strMsg.Format(_T("%02d:       %04d(%s)      %04d(%s)      %04d(%s)"),
			(i + 1) / 3 + 1, *pos, arrPLs[i].c_str(), *(pos + 1), arrPLs[i + 1].c_str(), 
			*(pos + 2), arrPLs[i + 2].c_str());
		AppendOutputText(strMsg);
	}

	strMsg.Format(_T("\r\n\r\n[%d]计算完成, 共有(%u)注任9结果"),
		data.m_nID, pEngine->GetResult9().size());
	AppendOutputText(strMsg);
	return;
}

void CDialogGambel::CreateWorkDir() {
	CStringATL strPath(Global::GetAppPath().c_str());
	strPath += _T("gambel");
	CStringATL strInitFileName;
	CreateDirectory(strPath, NULL);
	strPath = strPath + _T("\\") + m_strQH.c_str();
	CreateDirectory(strPath, NULL);
	m_strWorkDir = strPath;
}