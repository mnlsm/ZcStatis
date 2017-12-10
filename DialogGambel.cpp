#include "stdafx.h"
#include "DialogGambel.h"
#include "DialogTZ.h"
#include "Global.h"
#include "EngineLua.h"

static const CStlString DZ_FILTER_NAME = _T("单注文件(*.dz)");
static const CStlString DZ_FILTER = _T("*.dz");

static const CStlString LUA_FILTER_NAME = _T("脚本文件(*.lua)");
static const CStlString LUA_FILTER = _T("*.lua");

CDialogGambel::CDialogGambel(IDbSystem *pDbSystem, IDbDatabase *pDbDatabase, 
	const CStlString& qh) : 
		m_lstGambel(this, 1), 
		m_edOutput(this, 2),
		m_edSearch(this, 3) {
	m_pDbSystem = pDbSystem;
	m_pDbDatabase = pDbDatabase;
	m_strQH = qh;

	m_pPrintInfo.reset(new (std::nothrow) CMyPrintInfo());
	m_pPrinter.reset(new (std::nothrow) CPrinter());
	m_wndPreview.reset(new (std::nothrow) CPrintWnd());
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
	CStlString strPath = Global::GetAppPath() + _T("gambel");
	CreateDirectory(strPath.c_str(), NULL);
	strPath = strPath + _T("\\") + m_strQH;
	CreateDirectory(strPath.c_str(), NULL);
	dlg.m_ofn.lpstrInitialDir = strPath.c_str();
	if (dlg.DoModal() != IDOK) {
		return 1L;
	}
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	std::string filedata;
	if (!Global::ReadFileData(strLoadPath, filedata) || filedata.size() == 0) {
		MessageBox("单注文件读取失败！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	CStlString strCodes = Global::formUTF8(filedata);
	CStringATL strSQL, strPL, strMatchs;
	strSQL.Format(_T("SELECT PLDATA, MATCHS FROM PLDATA WHERE ID='%s'"), m_strQH.c_str());
	std::unique_ptr<IDbRecordset> pRS(m_pDbSystem->CreateRecordset(m_pDbDatabase));
	if (pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY)) {
		if (!pRS->IsEOF()) {
			pRS->GetField(0, strPL);
			pRS->GetField(1, strMatchs);
		}
	}
	pRS->Close();
	strSQL = _T("INSERT INTO GAMBEL (QH, INUSE, CODESTYPE, CODES, PLDATA, MATCHS) VALUES(?,?,?,?,?,?)");
	std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	pCmd->Create(strSQL);
	pCmd->SetParam(0, m_strQH);
	long val = 0;
	pCmd->SetParam(1, &val);
	val = 1;
	pCmd->SetParam(2, &val);
	pCmd->SetParam(3, strCodes);
	pCmd->SetParam(4, strPL);
	pCmd->SetParam(5, strMatchs);
	if (!pCmd->Execute(NULL)) {
		MessageBox("INSERT DB 失败！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	pCmd->Close();
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuAddFuShi(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CDialogTZ dlg(m_pDbSystem, m_pDbDatabase, m_strQH);
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
	
}

void CDialogGambel::ReloadFangAnData() {
	m_arrDbData.clear();
	m_lstGambel.DeleteAllItems();
	ClearOutputText();
	std::unique_ptr<IDbRecordset> pRS(m_pDbSystem->CreateRecordset(m_pDbDatabase));
	CStringATL strSQL;
	strSQL.Format(_T("select ID, INUSE, CODESTYPE, CODES, PLDATA, SCRIPT,")
		_T(" RESULT FROM GAMBEL WHERE QH='%s' order by ID asc"), m_strQH.c_str());
	if (pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY)) {
		while (!pRS->IsEOF()) {
			DataRow row;
			pRS->GetField(0, row.m_nID);
			pRS->GetField(1, row.m_nInUse);
			pRS->GetField(2, row.m_nCodesType);
			pRS->GetField(3, row.m_strCodes);
			pRS->GetField(4, row.m_strPL);
			pRS->GetField(5, row.m_strScript);
			pRS->GetField(6, row.m_strResult);
			m_arrDbData.push_back(row);
			pRS->MoveNext();
		}
	}
	pRS->Close();

    CStringATL strNum;
	UINT rowIndex = 0;
	for (const auto& row : m_arrDbData) {
		strNum.Format(_T("%u"), row.m_nID);
		rowIndex = m_lstGambel.InsertItem(rowIndex, strNum);
		UINT colIndex = 0;
		if (row.m_nCodesType == 0) {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, _T("复式"));
			m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strCodes);
		} else {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, _T("单式"));
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
		CStlString strPath = Global::GetAppPath() + _T("gambel");
		CreateDirectory(strPath.c_str(), NULL);
		strPath = strPath + _T("\\") + m_strQH;
		CreateDirectory(strPath.c_str(), NULL);
		dlg.m_ofn.lpstrInitialDir = strPath.c_str();
		if (dlg.DoModal() != IDOK) {
			return;
		}
		CStlString strLoadPath = dlg.m_ofn.lpstrFile;
		std::string filedata;
		if (!Global::ReadFileData(strLoadPath, filedata) || filedata.size() == 0) {
			MessageBox("单注文件读取失败！", "错误", MB_ICONERROR | MB_OK);
			return;
		}
		CStlString strCodes = Global::formUTF8(filedata);
		std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
		CStringATL strSQL = _T("UPDATE GAMBEL SET CODESTYPE=?, CODES=?, RESULT=? WHERE ID=?");
		pCmd->Create(strSQL);
		long lVal = 1;
		pCmd->SetParam(0, &lVal);
		pCmd->SetParam(1, strCodes);
		pCmd->SetParam(2, _T(""));
		pCmd->SetParam(3, &data.m_nID);
		if (pCmd->Execute(NULL)) {
			ReloadFangAnData();
		}
	} else if (data.m_nCodesType == 0) {
		CDialogTZ dlg(m_pDbSystem, m_pDbDatabase, m_strQH, data.m_nID);
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
	CStlString strPath = Global::GetAppPath() + _T("gambel");
	CreateDirectory(strPath.c_str(), NULL);
	strPath = strPath + _T("\\") + m_strQH;
	CreateDirectory(strPath.c_str(), NULL);
	dlg.m_ofn.lpstrInitialDir = strPath.c_str();
	CStringATL strInitFileName;
	strInitFileName.Format(_T("%s_%d.lua"), m_strQH.c_str(), data.m_nID);
	TCHAR szFileName[MAX_PATH + 1] = { _T('\0') };
	_tcscpy(szFileName, strInitFileName);
	dlg.m_ofn.lpstrFile = szFileName;
	CStlString strFileFullName = strPath + _T("\\") + szFileName;
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
	CStlString strScript = Global::formUTF8(filedata);
	std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET SCRIPT=? WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, strScript);
	pCmd->SetParam(1, &data.m_nID);
	if (pCmd->Execute(NULL)) {
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
	std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET SCRIPT=? WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, _T(""));
	pCmd->SetParam(1, &data.m_nID);
	if (pCmd->Execute(NULL)) {
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
	CStringATL strPath(Global::GetAppPath().c_str());
	strPath += _T("gambel");
	CStringATL strInitFileName;
	CreateDirectory(strPath, NULL);
	strPath = strPath + _T("\\") + m_strQH.c_str();
	CreateDirectory(strPath, NULL);
	dlg.m_ofn.lpstrInitialDir = strPath;
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
}

void CDialogGambel::DoDeleteResult(const DataRow& data) {
	std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET RESULT=? WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, _T(""));
	pCmd->SetParam(1, &data.m_nID);
	if (pCmd->Execute(NULL)) {
		ReloadFangAnData();
	}
}

void CDialogGambel::DoDeleteRow(const DataRow& data) {
	std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("DELETE FROM GAMBEL WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, &data.m_nID);
	if (pCmd->Execute(NULL)) {
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
	std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET INUSE=? WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, &pData->m_nInUse);
	pCmd->SetParam(1, &pData->m_nID);
	if (pCmd->Execute(NULL)) {
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
	} else if (data.m_nCodesType == 0) {
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
		//MessageBox(strMsg, _T("错误"), MB_ICONERROR | MB_OK);
		return;
	}
	std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET RESULT=? WHERE ID=?");
	CStlString strResult;
	CEngine::GetRecordsString(pEngine->GetResult(), strResult);
	pCmd->Create(strSQL);
	pCmd->SetParam(0, strResult);
	pCmd->SetParam(1, &data.m_nID);
	if (pCmd->Execute(NULL)) {
		ReloadFangAnData();
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
	return;
}