#include "stdafx.h"
#include "DialogTZ.h"
#include "Global.h"
#include "Engine.h"


CDialogTZ::CDialogTZ(const std::shared_ptr<SQLite::Database>& db,
	const CStlString& qh, int gambleID) {
	m_pDatabase = db;
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
	ReLoadDataToShow(true);
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

LRESULT CDialogTZ::OnRecommendMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	if (wID == IDM_RECOMMEND_ONE) {
		for (int i = 0; i < TOTO_COUNT; i++) {
			UINT ctlID = IDC_CORESULT1 + i * 3;
			CComboBox co = GetDlgItem(ctlID);
			co.SetCurSel(1);
			co = GetDlgItem(ctlID + 1);
			co.SetCurSel(0);
			co = GetDlgItem(ctlID + 2);
			co.SetCurSel(0);
		}
	}
	else if (wID == IDM_RECOMMEND_TWO) {
		for (int i = 0; i < TOTO_COUNT; i++) {
			UINT ctlID = IDC_CORESULT2 + i * 3;
			CComboBox co = GetDlgItem(ctlID);
			co.SetCurSel(1);
			co = GetDlgItem(ctlID + 1);
			co.SetCurSel(0);
			co = GetDlgItem(ctlID - 1);
			co.SetCurSel(0);
		}
	}
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
	m_MenuButton = GetDlgItem(IDC_BUSET);
	m_MenuButton.AddMenuItem(IDM_RECOMMEND_ONE, _T("单选"));
	m_MenuButton.AddMenuItem(IDM_RECOMMEND_TWO, _T("双选"));
}

struct GvItem {
	CStlString code;
	double gv;
};

bool operator<(const GvItem &x, const GvItem &y) {
	return x.gv > y.gv;
}

void CDialogTZ::initComboxes() {
	CDoublexyArray arrPls, arrGvs;
	CEngine::GetPLDatas(m_strPL, arrPls, arrGvs);

	GvItem items[TOTO_COUNT][3];;
	for (int i = 0; i < TOTO_COUNT; i++) {
		for (int j = 0; j < 3; j++) {
			if (j == 0) {
				items[i][j].code = _T("3");
			} else if (j == 1) {
				items[i][j].code = _T("1");
			} else {
				items[i][j].code = _T("0");
			}
			items[i][j].gv = arrGvs[i][j];
		}
		GvItem* start = &items[i][0];
		std::stable_sort(start, start + 3);
	}
	CComboBox coWnd = NULL;
	for (int i = 0; i < TOTO_COUNT; i++) {
		coWnd = GetDlgItem(i * 3 + IDC_CORESULT1);
		coWnd.AddString(_T(" "));
		coWnd.AddString(items[i][0].code.c_str());
		coWnd.AddString(items[i][1].code.c_str());
		coWnd.AddString(items[i][2].code.c_str());

		coWnd = GetDlgItem(i * 3 + 1 + IDC_CORESULT1);
		coWnd.AddString(_T(" "));
		coWnd.AddString((items[i][0].code + items[i][1].code).c_str());
		coWnd.AddString((items[i][0].code + items[i][2].code).c_str());
		coWnd.AddString((items[i][1].code + items[i][2].code).c_str());

		coWnd = GetDlgItem(i * 3 + 2 + IDC_CORESULT1);
		coWnd.AddString(_T(" "));
		coWnd.AddString((items[i][0].code + items[i][1].code + items[i][2].code).c_str());
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

BOOL CDialogTZ::ReLoadDataToShow(BOOL first) {
	m_wndQI.SetWindowText(m_strQH.c_str());
	ClearUserChoice();
	if (m_GambleID != -1) {
		CStringATL strSQL, strResults;
		strSQL.Format(_T("SELECT CODES, PLDATA, MATCHS FROM GAMBEL WHERE ID=%d"), m_GambleID);
		SQLite::Statement sm(*m_pDatabase, strSQL);
		if (sm.executeStep()) {
			strResults = sm.getColumn(0).getString().c_str();
			m_strPL = sm.getColumn(1).getString().c_str();
			m_strMatchs = Global::formUTF8(sm.getColumn(2).getString().c_str());
		}

		if (first) {
			initComboxes();
		}

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
				} else {
					if (arrResults[arrPos] == _T("31")) {
						index = coWnd.FindStringExact(0, "13");
						coWnd.SetCurSel(index);
					} else if (arrResults[arrPos] == _T("30")) {
						index = coWnd.FindStringExact(0, "03");
						coWnd.SetCurSel(index);
					} else if (arrResults[arrPos] == _T("10")) {
						index = coWnd.FindStringExact(0, "01");
						coWnd.SetCurSel(index);
					}	else if (arrResults[arrPos] == _T("310")) {
						coWnd.SetCurSel(1);
					}
				}
			}
		}
	} else {
		CStringATL strSQL;
		strSQL.Format(_T("SELECT PLDATA, MATCHS FROM PLDATA WHERE ID='%s'"), m_strQH.c_str());
		SQLite::Statement sm(*m_pDatabase, strSQL);
		if (sm.executeStep()) {
			m_strPL = sm.getColumn(0).getString().c_str();
			m_strMatchs = Global::formUTF8(sm.getColumn(0).getString()).c_str();
		}
		if (first) {
			initComboxes();
		}
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
	if (m_GambleID == -1) {
		strSQL = _T("INSERT INTO GAMBEL (QH, INUSE, CODESTYPE, CODES, PLDATA, MATCHS) VALUES(?,?,?,?,?,?)");
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bindNoCopy(1, m_strQH);
		sm.bind(2, 0);
		sm.bind(3, 0);
		sm.bindNoCopy(4, strResults);
		sm.bindNoCopy(5, m_strPL);
		sm.bindNoCopy(6, m_strMatchs);
		ret = (sm.exec() > 0);
	} else {                                 
		strSQL.Format(_T("UPDATE GAMBEL SET CODESTYPE=?, CODES=?, RESULT=? WHERE ID=%d"), m_GambleID);
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bind(1, 0);
		sm.bindNoCopy(2, strResults);
		sm.bindNoCopy(3, _T(""));
		ret = (sm.exec() > 0);
	}
	m_bDataChanged = TRUE;
	return ret;
}
