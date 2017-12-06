#include "stdafx.h"
#include "DialogTZ.h"
#include "Global.h"

CDialogTZ::CDialogTZ(IDbSystem *pDbSystem, IDbDatabase *pDbDatabase, 
	const CStlString& qh, int gambleID) {
	m_pDbSystem = pDbSystem;
	m_pDbDatabase = pDbDatabase;
	m_strQH = qh;
	m_GambleID = gambleID;
	m_bDataChanged = FALSE;
    m_brush = GetSysColorBrush(COLOR_MENU);
}

BOOL CDialogTZ::IsDbDataChanged() {
	return m_bDataChanged;
}

LRESULT CDialogTZ::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    HWND wnd = (HWND)lParam;
    HDC dc = (HDC)wParam;
    if(wnd == GetDlgItem(IDC_STDI) || wnd == GetDlgItem(IDC_STSHOWQI)
            || wnd == GetDlgItem(IDC_STQI)) {
        ::SetTextColor(dc, RGB(0, 0, 255));
        ::SetBkMode(dc, TRANSPARENT);
        return (LRESULT)(HBRUSH)m_brush;
    }
    bHandled = FALSE;
    return 1L;
}

LRESULT CDialogTZ::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CenterWindow();
	initConctrols();
	ReLoadDataToShow();
    return TRUE;
}

LRESULT CDialogTZ::OnResultSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CComboBox co = GetDlgItem(wID);
	if (co.GetCurSel() == 0) {
		return 1;
	}
	int pos = wID - IDC_CORESULT1;
	int pos1 = pos + 1;
	int pos2 = pos + 2;
	if ((pos % 3) == 1) {
		pos1 = pos - 1;
		pos2 = pos + 1;
	} else if ((pos % 3) == 2) {
		pos1 = pos - 1;
		pos2 = pos - 2;
	}
	pos1 += IDC_CORESULT1;
	pos2 += IDC_CORESULT1;
	co = GetDlgItem(pos1);
	co.SetCurSel(0);
	co = GetDlgItem(pos2);
	co.SetCurSel(0);
	return 1L;
}

LRESULT CDialogTZ::OnClickedBuAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CStlString strResults;
	CStringATL strError;
	if (!ReadUserChoice(strResults, strError)) {
		MessageBox(strError, _T("错误"), MB_OK | MB_ICONWARNING);
		return 0;
	}
	BOOL bRet = DoUpdateDatabase(strResults);
	if (bRet) {
		CStringATL text = (m_GambleID == -1) ? _T("添加成功") : _T("更新成功");
		MessageBox(text, _T("提示"), MB_OK | MB_ICONWARNING);
		if (m_GambleID == -1) {
			EndDialog(IDOK);
		}
	} else {
		CStringATL text = (m_GambleID == -1) ? _T("添加失败") : _T("更新失败");
		MessageBox(text, _T("错误"), MB_OK | MB_ICONWARNING);
	}
	return 0;
}

LRESULT CDialogTZ::OnClickedBuClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ClearUserChoice();
	return 0;
}

LRESULT CDialogTZ::OnClickedBuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	EndDialog(IDCANCEL);
	return 0;
}

void CDialogTZ::initConctrols() {
	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR|LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);

    //init static data
    m_wndQI = GetDlgItem(IDC_STSHOWQI);
	if (m_GambleID != -1) {
		SetWindowText("更新选择");
		CWindow wnd = GetDlgItem(IDC_BUADD);
		wnd.SetWindowText(_T("更新"));
	}
    CComboBox coWnd = NULL;
    for(int i = IDC_CORESULT1; i <= IDC_CORESULT42; i++) {
        coWnd = GetDlgItem(i);
        if(coWnd.IsWindow()) {
			int mod = ((i - IDC_CORESULT1) % 3);
			if (mod == 0) {
				coWnd.AddString(_T(" "));
				coWnd.AddString(_T("3"));
				coWnd.AddString(_T("1"));
				coWnd.AddString(_T("0"));
			} else if (mod == 1) {
				coWnd.AddString(_T(" "));
				coWnd.AddString(_T("31"));
				coWnd.AddString(_T("30"));
				coWnd.AddString(_T("10"));
			} else if (mod == 2) {
				coWnd.AddString(_T(" "));
				coWnd.AddString(_T("310"));
			}
        }
    }
}

BOOL CDialogTZ::ReadUserChoice(CStlString &strResults, CStringATL& strErrInfo) {
	strResults.clear();
	strErrInfo.Empty();
	for (int i = IDC_CORESULT1; i <= IDC_CORESULT42; i = i + 3) {
		CStringATL result;
		CComboBox co1 = GetDlgItem(i);
		CComboBox co2 = GetDlgItem(i + 1);
		CComboBox co3 = GetDlgItem(i + 2);
		if (co1.GetCurSel() > 0) {
			co1.GetLBText(co1.GetCurSel(), result);
		} else if (co2.GetCurSel() > 0) {
			co2.GetLBText(co2.GetCurSel(), result);
		} else if (co3.GetCurSel() > 0) {
			co3.GetLBText(co3.GetCurSel(), result);
		}
		if (result.IsEmpty()) {
			CStringATL strTemp;
			CWindow wnd = GetDlgItem(IDC_STMATCHONE1 + (i - IDC_CORESULT1) / 3);
			wnd.GetWindowText(strTemp);
			strErrInfo.Format(_T("请选择结果： %s"), strTemp);
			return FALSE;
		}
		if (strResults.empty()) {
			strResults = result;
		} else {
			strResults = strResults + _T(",") + (LPCTSTR)result;
		}
	}
	return TRUE;
}

void CDialogTZ::ClearUserChoice() {
	CComboBox coWnd = NULL;
	for (int i = IDC_CORESULT1; i <= IDC_CORESULT42; i++) {
		coWnd = GetDlgItem(i);
		if (coWnd.IsWindow()) coWnd.SetCurSel(0);
	}
}

BOOL CDialogTZ::ReLoadDataToShow() {
	m_wndQI.SetWindowText(m_strQH.c_str());
	ClearUserChoice();
	if (m_GambleID != -1) {
		CStringATL strSQL, strResults;
		strSQL.Format(_T("select CODES, PLDATA, MATCHS from GAMBEL where ID=%d"), m_GambleID);
		std::auto_ptr<IDbRecordset> pRS(m_pDbSystem->CreateRecordset(m_pDbDatabase));
		if(pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY)) {
			if (!pRS->IsEOF()) {
				pRS->GetField(0, strResults);
				pRS->GetField(1, m_strPL);
				pRS->GetField(2, m_strMatchs);
			}
		}
		pRS->Close();

		CStlStrArray arrResults;
		Global::DepartString((LPCTSTR)strResults, _T(","), arrResults);
		for (int i = IDC_CORESULT1; i <= IDC_CORESULT42; i = i + 3) {
			int arrPos = (i - IDC_CORESULT1) / 3;
			int ctrlPos = i + arrResults[arrPos].length() - 1;
			CComboBox coWnd = GetDlgItem(ctrlPos);
			if (coWnd.IsWindow()) {
				int index = coWnd.FindStringExact(0, arrResults[arrPos].c_str());
				if (index >= 0) {
					coWnd.SetCurSel(index);
				}
			}
		}
	} else {
		CStringATL strSQL;
		strSQL.Format(_T("select PLDATA, MATCHS from PLDATA where ID='%s'"), m_strQH.c_str());
		std::auto_ptr<IDbRecordset> pRS(m_pDbSystem->CreateRecordset(m_pDbDatabase));
		if(pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY)) {
			if (!pRS->IsEOF()) {
				pRS->GetField(0, m_strPL);
				pRS->GetField(1, m_strMatchs);
			}
		}
		pRS->Close();
	}
	CStlStrArray arrMatchs;
	Global::DepartString(m_strMatchs, _T("\n"), arrMatchs);
	if (arrMatchs.size() == (IDC_STMATCHONE14 - IDC_STMATCHONE1) + 1) {
		for (int i = 0; i < arrMatchs.size(); i++) {
			int ctrlPos = i + IDC_STMATCHONE1;
			CStatic matchCtrl = GetDlgItem(ctrlPos);
			matchCtrl.SetWindowText(arrMatchs[i].c_str());
		}
	}
	return TRUE;
}

BOOL CDialogTZ::DoUpdateDatabase(const CStlString &strResults) {
	BOOL ret = FALSE;
	CStringATL strSQL;
	std::auto_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
	if (m_GambleID == -1) {
		strSQL = _T("INSERT INTO GAMBEL (QH, INUSE, CODESTYPE, CODES, PLDATA, MATCHS) VALUES(?,?,?,?,?,?)");
		pCmd->Create(strSQL);
		pCmd->SetParam(0, m_strQH);
		long val = 0;
		pCmd->SetParam(1, &val);
		pCmd->SetParam(2, &val);
		pCmd->SetParam(3, strResults.c_str());
		pCmd->SetParam(4, m_strPL);
		pCmd->SetParam(5, m_strMatchs);
		ret = pCmd->Execute(NULL);
	} else {
		strSQL.Format(_T("UPDATE GAMBEL SET CODES=? WHERE ID=%d"), m_GambleID);
		pCmd->Create(strSQL);
		pCmd->SetParam(0, strResults.c_str());
		ret = pCmd->Execute(NULL);
	}
	pCmd->Close();
	m_bDataChanged = TRUE;
	return ret;
}
