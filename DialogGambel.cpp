#include "stdafx.h"
#include "DialogGambel.h"
#include "DialogTZ.h"
#include "Global.h"
#include "Engine.h"

static const CStlString DZ_FILTER_NAME = _T("单注文件(*.dz)");
static const CStlString DZ_FILTER = _T("*.dz");

static const CStlString LUA_FILTER_NAME = _T("脚本文件(*.lua)");
static const CStlString LUA_FILTER = _T("*.lua");

CDialogGambel::CDialogGambel(IDbSystem *pDbSystem, IDbDatabase *pDbDatabase, 
		const CStlString& qh) : m_lstGambel(this) {
	m_pDbSystem = pDbSystem;
	m_pDbDatabase = pDbDatabase;
	m_strQH = qh;
}

LRESULT CDialogGambel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CenterWindow();
	InitControls();
	ReloadFangAnData();
	return 1L;
}

LRESULT CDialogGambel::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = FALSE;
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuAddDanShi(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {

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

LRESULT CDialogGambel::OnClickedBuEmpty(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT CDialogGambel::OnClickedBuCalc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
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
	
	DWORD dwStyleEx = ListView_GetExtendedListViewStyle(m_lstGambel.m_hWnd);
	dwStyleEx |= (LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL);
	ListView_SetExtendedListViewStyle(m_lstGambel.m_hWnd, dwStyleEx);
	int colIndex = 0;
	m_lstGambel.InsertColumn(colIndex++, "编号", LVCFMT_LEFT, 65);    //70
	m_lstGambel.InsertColumn(colIndex++, "投注类型", LVCFMT_CENTER, 65);    //70
	m_lstGambel.InsertColumn(colIndex++, "投注数据", LVCFMT_LEFT, 300);    //70
	m_lstGambel.InsertColumn(colIndex++, "脚本数据", LVCFMT_LEFT, 200);    //70
	m_lstGambel.InsertColumn(colIndex++, "结果数据", LVCFMT_LEFT, 200);    //70
	m_lstGambel.InsertColumn(colIndex++, "结果个数", LVCFMT_CENTER, 65);    //70

	CStringATL strTemp;         
	strTemp.Format(_T("方案列表(%s):"), m_strQH.c_str());
	CWindow wnd = GetDlgItem(IDC_STGAMBELS);
	wnd.SetWindowText(strTemp);
	
}

void CDialogGambel::ReloadFangAnData() {
	m_arrDbData.clear();
	m_lstGambel.DeleteAllItems();
	std::auto_ptr<IDbRecordset> pRS(m_pDbSystem->CreateRecordset(m_pDbDatabase));
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
		m_lstGambel.SetCheckState(rowIndex, (row.m_nInUse == 1));
		UINT colIndex = 0;
		if (row.m_nCodesType == 0) {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, _T("复式"));
			m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strCodes);
		} else {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, _T("单式"));
			m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strCodes.Left(15) + _T(" ......"));
		}
		m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strScript.Left(20));
		if (!row.m_strResult.IsEmpty()) {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strResult.Left(15) + _T(" ......"));
		} else {
			++colIndex;
		}
		//CIntxyArray arrRecords
		strNum.Format(_T("%u"), CEngine::GetRecordCount((LPCTSTR)row.m_strResult));
		m_lstGambel.SetItemText(rowIndex, ++colIndex, strNum);

	}
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
		std::auto_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
		CStringATL strSQL = _T("UPDATE GAMBEL SET CODES=? WHERE ID=?");
		pCmd->Create(strSQL);
		pCmd->SetParam(0, strCodes);
		pCmd->SetParam(1, &data.m_nID);
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
	if (dlg.DoModal() != IDOK) {
		return;
	}
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	std::string filedata;
	if (!Global::ReadFileData(strLoadPath, filedata) || filedata.size() == 0) {
		MessageBox("Script文件读取失败！", "错误", MB_ICONERROR | MB_OK);
		return;
	}
	CStlString strCodes = Global::formUTF8(filedata);
	std::auto_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET SCRIPT=? WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, strCodes);
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
	std::auto_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET SCRIPT=? WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, _T(""));
	pCmd->SetParam(1, &data.m_nID);
	if (pCmd->Execute(NULL)) {
		ReloadFangAnData();
	}
}

void CDialogGambel::DoSaveResult(const DataRow& data) {

}

void CDialogGambel::DoDeleteResult(const DataRow& data) {
	std::auto_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET RESULT=? WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, _T(""));
	pCmd->SetParam(1, &data.m_nID);
	if (pCmd->Execute(NULL)) {
		ReloadFangAnData();
	}
}

void CDialogGambel::DoCalcResult(const DataRow& data) {

}

void CDialogGambel::DoDeleteRow(const DataRow& data) {
	std::auto_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("DELETE FROM GAMBEL WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, &data.m_nID);
	if (pCmd->Execute(NULL)) {
		ReloadFangAnData();
	}
}

void CDialogGambel::DoRowInUse(UINT uItem, BOOL inuse) {
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
	if (pData == NULL) {
		return;
	}
	int nInUse = inuse ? 1 : 0;
	if (nInUse == pData->m_nInUse) {
		return;
	}
	pData->m_nInUse = nInUse;
	std::auto_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	CStringATL strSQL = _T("UPDATE GAMBEL SET INUSE=? WHERE ID=?");
	pCmd->Create(strSQL);
	pCmd->SetParam(0, &pData->m_nInUse);
	pCmd->SetParam(1, &pData->m_nID);
	if (pCmd->Execute(NULL)) {
		//ReloadFangAnData();
	}

}
