#include "stdafx.h"
#include "DialogTZ.h"

CDialogTZ::CDialogTZ(IDbSystem *pDbSystem, IDbDatabase *pDbDatabase, 
	const CStlString& qh, int gambleID) {
	m_pDbSystem = pDbSystem;
	m_pDbDatabase = pDbDatabase;
	m_strQH = qh;
	m_GambleID = gambleID;
    m_brush = GetSysColorBrush(COLOR_MENU);
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
    bHandled = false;
    return 1L;
}

LRESULT CDialogTZ::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CenterWindow();
	SetIcon(AtlLoadIcon(IDR_MAINFRAME), TRUE);
	initConctrol();
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
	co = GetDlgItem(pos1);
	co.SetCurSel(0);
	co = GetDlgItem(pos2);
	co.SetCurSel(0);
	return 1L;
}


LRESULT CDialogTZ::OnClickedBuAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CStringATL strResults, strError;
	if (!ReadUserChoice(strResults, strError)) {
		MessageBox(strError, "错误", MB_OK | MB_ICONWARNING);
		return 0;
	}
	return 0;
}


LRESULT CDialogTZ::OnClickedBuClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ClearUserChoice();
	return 0;
}

void CDialogTZ::initConctrol() {
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

BOOL CDialogTZ::ReadUserChoice(CStringATL &strResults, CStringATL& strErrInfo) {
	strResults.Empty();
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
		if (strResults.IsEmpty()) {
			strResults = result;
		} else {
			strResults = strResults + _T(",") + result;
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
	return TRUE;
}