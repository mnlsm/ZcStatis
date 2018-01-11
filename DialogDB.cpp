// DialogDB.cpp : CDialogDB 的实现

#include "stdafx.h"
#include "DialogDB.h"
#include "Global.h"

#include "BasicExcel.hpp"

void CDialogDB::LoadData() {
    m_lstQH.ResetContent();

    CStringATL strSQL = _T("SELECT ID FROM PLDATA ORDER BY ID DESC");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	while (sm.executeStep()) {
		CStringATL strID = sm.getColumn(0).getString().c_str();
		strID.TrimLeft();
		strID.TrimRight();
		if(!strID.IsEmpty()) {
			m_lstQH.AddString(strID);
		}
	}

}


// CDialogDB
LRESULT CDialogDB::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CAxDialogImpl<CDialogDB>::OnInitDialog(uMsg, wParam, lParam, bHandled);
    DoDataExchange(FALSE);
	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);

    //GetDlgItem(IDC_EDIT_QH)->
    m_edQH.SetLimitText(5);
    m_edCode.SetLimitText(14);
	m_edSales.SetLimitText(10);

    LoadData();

    CenterWindow(::GetDesktopWindow());
	m_edQH.SetFocus();
    return 0;  // 使系统设置焦点
}

LRESULT CDialogDB::OnClickedAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    DoDataExchange(TRUE);

    int nLenQH = m_strQH.GetLength();
    if(nLenQH != 5) {
        MessageBox("期号输入错误，请检查！", "错误", MB_ICONERROR|MB_OK);
        return 1L;
    }
    for(int i = 0 ; i < nLenQH ; i++) {
        if(m_strQH[i] < '0' || m_strQH[i] > '9') {
            MessageBox("期号输入错误，请检查！", "错误", MB_ICONERROR|MB_OK);
            return 1L;
        }
    }

	if (m_strPL.IsEmpty()) {
		MessageBox("赔率输入错误，请检查！", "错误", MB_ICONERROR|MB_OK);
		return 1L;
	}

    int nLenCode = m_strCode.GetLength();
    if(nLenCode != 14) {
		m_strCode.Empty();
        //MessageBox(_T("结果输入错误，请检查！"), _T("错误"), MB_ICONERROR|MB_OK);
        //return 1L;
    }
	for(int i = 0 ; i < nLenCode ; i++) {
        if(m_strCode[i] != '0' && m_strCode[i] != '1' && m_strCode[i] != '3') {
            //MessageBox("结果输入错误，请检查！", "错误", MB_ICONERROR|MB_OK);
            //return 1L;
			m_strCode.Empty();
			break;
        }
    }

    CStringATL strSQL =  _T("DELETE FROM PLDATA WHERE ID = ?");
	SQLite::Statement sm(*m_pDatabase, strSQL);
    sm.bindNoCopy(1, m_strQH);
	sm.exec();
 
    float fBonus = _ttof(m_strBonus);
	long lSales = _ttol(m_strSales);
    strSQL = _T("INSERT INTO PLDATA (ID, BONUS,RESULT,PLDATA,SALES,MATCHS) values(?,?,?,?,?,?)");
	SQLite::Statement sm1(*m_pDatabase, strSQL);
	sm1.bindNoCopy(1, m_strQH);
	sm1.bind(2, fBonus);
	if (!m_strCode.IsEmpty()) {
		sm1.bindNoCopy(3, m_strCode);
	} else {
		sm1.bindNoCopy(3, "00000000000000");
	}
	sm1.bindNoCopy(4, m_strPL);
	sm1.bind(5, lSales);
	sm1.bindNoCopy(6, m_strMatchs);
	sm1.exec();

	if (m_lstQH.FindString(0, m_strQH) < 0)
		m_lstQH.AddString(m_strQH);

	ClearDataShow();
	return 0;
}

LRESULT CDialogDB::OnClickedExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	EndDialog(wID);
	return 0;
}

LRESULT CDialogDB::OnClickedClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ClearDataShow();
	return 0;
}

LRESULT CDialogDB::OnClickedDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	int nSel = m_lstQH.GetCurSel();
	if (nSel < 0) return 1L;

	CStringATL strID;
	m_lstQH.GetText(nSel, strID.GetBuffer(1024));
	strID.ReleaseBuffer();

	CStringATL strSQL = _T("DELETE FROM PLDATA WHERE ID = ?");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	sm.bindNoCopy(1, strID);
	sm.exec();
	m_lstQH.DeleteString(nSel);
	return 0;
}

LRESULT CDialogDB::OnListQHSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	int nSel = m_lstQH.GetCurSel();
	if (nSel < 0) return 1L;

	CStringATL strID;
	m_lstQH.GetText(nSel, strID.GetBuffer(1024));
	strID.ReleaseBuffer();

	CStringATL strSQL;
	strSQL.Format(_T("SELECT ID,BONUS,RESULT,PLDATA,SALES,MATCHS FROM PLDATA WHERE ID='%s'"), strID);
	SQLite::Statement sm(*m_pDatabase, strSQL);
	if (sm.executeStep()) {
		float fBonus = 0;
		long lSales = 0;
		m_strQH = sm.getColumn(0).getString().c_str();
		fBonus = sm.getColumn(1).getDouble();
		sprintf(m_strBonus.GetBuffer(255), "%.2f", fBonus);
		m_strBonus.ReleaseBuffer();
		m_strCode = sm.getColumn(2).getString().c_str();
		m_strPL = sm.getColumn(3).getString().c_str();
		lSales = sm.getColumn(4).getInt();
		m_strMatchs = sm.getColumn(5).getString().c_str();
		sprintf(m_strSales.GetBuffer(255), "%u", lSales);
		m_strSales.ReleaseBuffer();
	}

	DoDataExchange(FALSE);

	return 0;

}


LRESULT CDialogDB::OnClickedExcel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	DoDataExchange(TRUE);
	const std::string dmVS = "    VS    ";
	CStlString rateStr, matchStr;
	int nLenQH = m_strQH.GetLength();
	if (nLenQH != 5) {
		MessageBox("期号输入错误，请检查！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	CStlString fn = Global::GetAppPath() + _T("odds\\") + (LPCTSTR)m_strQH + _T(".xls");
	using namespace YExcel;
	BasicExcel BE;
	if (!BE.Load(fn.c_str())) {
		MessageBox("打开xls文件失败！ 0", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}

	Worksheet* ws = BE.GetRawWorksheet(0);
	if (ws == NULL) {
		MessageBox("打开xls文件失败！ 1", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}

	const LONGINT maxRowIndex = ws->getMaxRowIndex();
	for (LONGINT row = 2; row < maxRowIndex; row = row + 2) {
		CStlString strMatchPrefix(16, ' ');
		int matchNameState = 0;
		TCHAR szR[20] = { _T('\0') };
		LONGINT matchNo = -1;
		std::string text;

		ws->getLongInt(row, 0, matchNo);
		if (matchNo != -1) {
			_stprintf(szR, _T("%02u."), matchNo);
			TCHAR* dataPos = (TCHAR*)strMatchPrefix.data();
			memcpy(dataPos, szR, _tcslen(szR) * sizeof(TCHAR));
			matchNameState++;
		} else {
			continue;
		}


		ws->getLabel(row, 4, text);
		if (!text.empty()) {
			int teamNameLen = text.size();
			if (teamNameLen < strMatchPrefix.size() - 3) {
				matchNameState++;
				TCHAR* dataPos = (TCHAR*)strMatchPrefix.data() + strMatchPrefix.size() - teamNameLen;
				memcpy(dataPos, text.c_str(), teamNameLen * sizeof(TCHAR));
			}
		}

		ws->getLabel(row, 6, text);
		if (!text.empty()) {
			if (matchNameState == 2) {
				strMatchPrefix = strMatchPrefix + dmVS + text;
			}
			else {
				strMatchPrefix.clear();
			}
			if (!strMatchPrefix.empty()) {
				if (matchStr.empty()) {
					matchStr = strMatchPrefix.c_str();
				}
				else {
					matchStr = matchStr + _T("\n") + strMatchPrefix.c_str();
				}
			}
		}

		for (int j = 7; j <= 9; j++) {
			double rate = 0.0;
			ws->getDouble(row, j, rate);
			if (rate > 0) {
				_stprintf(szR, _T("%.2f"), rate);
				if (rateStr.empty()) {
					rateStr = szR;
				} else {
					rateStr = rateStr + _T('#') + szR;
				}
			}
		}
	}
	CStlStrArray arrTemp;
	Global::DepartString(rateStr, _T("#"), arrTemp);
	if (arrTemp.size() == TOTO_COUNT * 3) {
		m_strPL = rateStr.c_str();
	}
	Global::DepartString(matchStr, _T("\n"), arrTemp);
	if (arrTemp.size() == TOTO_COUNT) {
		m_strMatchs = matchStr.c_str();
		std::wstring matchs_w = CT2W(matchStr.c_str()).m_psz;
	}
	DoDataExchange(FALSE);
    return 0L;
}

void CDialogDB::ClearDataShow() {
	m_strQH.Empty();
	m_strCode.Empty();
	m_strBonus.Empty();
	m_strPL.Empty();
	m_strSales.Empty();
	DoDataExchange(FALSE);
}