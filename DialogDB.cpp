// DialogDB.cpp : CDialogDB µƒ µœ÷

#include "stdafx.h"
#include "DialogDB.h"
#include "Global.h"

#include "libxls/XlsReader.h"
#include "mso.tlh"
#include "vbe6ext.tlh"
#include "excel.tlh"

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
    return 0;  //  πœµÕ≥…Ë÷√Ωπµ„
}

LRESULT CDialogDB::OnClickedAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    DoDataExchange(TRUE);

    int nLenQH = m_strQH.GetLength();
    if(nLenQH != 5) {
        MessageBox("∆⁄∫≈ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
        return 1L;
    }
    for(int i = 0 ; i < nLenQH ; i++) {
        if(m_strQH[i] < '0' || m_strQH[i] > '9') {
            MessageBox("∆⁄∫≈ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
            return 1L;
        }
    }

	if (m_strPL.IsEmpty()) {
		MessageBox("≈‚¬  ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
		return 1L;
	}

    int nLenCode = m_strCode.GetLength();
    if(nLenCode != 14) {
		m_strCode.Empty();
        //MessageBox(_T("Ω·π˚ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°"), _T("¥ÌŒÛ"), MB_ICONERROR|MB_OK);
        //return 1L;
    }
	for(int i = 0 ; i < nLenCode ; i++) {
        if(m_strCode[i] != '0' && m_strCode[i] != '1' && m_strCode[i] != '3') {
            //MessageBox("Ω·π˚ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
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

    if(m_lstQH.FindString(0, m_strQH) < 0)
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
    if(nSel < 0) return 1L;

    CStringATL strID;
    m_lstQH.GetText(nSel, strID.GetBuffer(1024));
    strID.ReleaseBuffer();

    CStringATL strSQL =  _T("DELETE FROM PLDATA WHERE ID = ?");
	SQLite::Statement sm(*m_pDatabase, strSQL);
	sm.bindNoCopy(1, strID);
    sm.exec();
    m_lstQH.DeleteString(nSel);
    return 0;
}

LRESULT CDialogDB::OnListQHSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    int nSel = m_lstQH.GetCurSel();
    if(nSel < 0) return 1L;

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
    int nLenQH = m_strQH.GetLength();
    if(nLenQH != 5) {
        MessageBox("∆⁄∫≈ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
        return 1L;
    }
    CStlString fn = Global::GetAppPath() + _T("odds\\") + (LPCTSTR)m_strQH + _T(".xls");
	//using namespace xls;
	//WorkBook wb(fn);

	CComPtr<Excel::_Application> pExcelApp;
	try {
        CT2OLE fn_ole(fn.c_str());

        Excel::_ApplicationPtr pExcelApp1;
        HRESULT hr = pExcelApp1.CreateInstance(L"Excel.Application");
        if(FAILED(hr)) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 0£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
            return 1L;
        }
        pExcelApp = pExcelApp1.GetInterfacePtr();

        //HRESULT hr = pExcelApp.CoCreateInstance(L"Excel.Application", NULL);
        if(FAILED(hr)) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 0£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
            return 1L;
        }
        CComPtr<Excel::Workbooks> pWorkbooks;
        hr = pExcelApp->get_Workbooks(&pWorkbooks);
        if(FAILED(hr)) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 1£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
            return 1L;
        }
        CComPtr<Excel::_Workbook> pWorkbook;
		hr = pWorkbooks->raw_Open(CComBSTR(fn_ole.m_psz), vtMissing, vtMissing,
                                  vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,
                                  vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,
                                  0, &pWorkbook);
        if(FAILED(hr) || pWorkbook.p == NULL) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 2£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
            return 1L;
        }
        CComPtr<Excel::Sheets> pSheets;
        hr = pWorkbook->get_Sheets(&pSheets);
        if(FAILED(hr) || pSheets.p == NULL) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 5£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
            return 1L;
        }
        long sheet_count = 0;
        hr = pSheets->get_Count(&sheet_count);
        if(FAILED(hr) || sheet_count <= 0) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 6£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
            return 1L;
        }
        CStlString rateStr, matchStr;
		const std::string dmVS = "    VS    ";
        for(long j = 0; j < sheet_count; j++) {
            CComPtr<IDispatch> pIDispath;
            CComVariant vIndex(j + 1);
            hr = pSheets->get_Item(vIndex, &pIDispath);
            if(FAILED(hr) || pIDispath.p == NULL) {
                MessageBox("Excel Com ≥ı ºªØ ß∞‹ 7£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
                return 1L;
            }
            CComQIPtr<Excel::_Worksheet> pWorksheet(pIDispath);
            CComBSTR sheet_name;
            hr = pWorksheet->get_Name(&sheet_name);
            if(FAILED(hr) || sheet_name == NULL) {
                MessageBox("Excel Com ≥ı ºªØ ß∞‹ 8£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
                return 1L;
            }
			if (wcscmp(sheet_name.m_str, L"Sheet1") != 0) {
				continue;
			}
            CComPtr<Excel::Range> pRootCells;
            hr = pWorksheet->get_UsedRange(0, &pRootCells);
            if(FAILED(hr) || pRootCells.p == NULL) {
                MessageBox("Excel Com ≥ı ºªØ ß∞‹ 9£°", "¥ÌŒÛ", MB_ICONERROR|MB_OK);
                return 1L;
            }
            long root_cell_count = pRootCells->GetCount();
            Excel::RangePtr rows = pRootCells->GetRows();
            long row_count = rows->GetCount();
            for(int i = 3; i <= row_count; i = i + 2) {
				CComVariant rowIndex(i);
				CStlString strMatchPrefix(16, ' ');
				int matchNameState = 0;
				for(int j = 1; j < 11; j++) {
                    CComVariant colIndex(j);
                    Excel::RangePtr cell_item = pRootCells->GetItem(rowIndex, colIndex);
					if (cell_item.GetInterfacePtr() == NULL) {
						continue;
					}
					_variant_t cell_value = cell_item->GetValue2();
					TCHAR szR[20] = { _T('\0') };
					if (j == 1 && cell_value.vt == VT_R8) {
						DWORD iMatchNo = (DWORD)cell_value.dblVal;
						_stprintf(szR, _T("%02u."), iMatchNo);
						TCHAR* dataPos = (TCHAR*)strMatchPrefix.data();
						memcpy(dataPos, szR, _tcslen(szR) * sizeof(TCHAR));
						matchNameState++;
					} else if (j == 5 && cell_value.vt == VT_BSTR) {
						CW2T teamName(cell_value.bstrVal);
						int teamNameLen = _tcslen(teamName.m_psz);
						if (teamNameLen < strMatchPrefix.size() - 3) {
							matchNameState++;
							TCHAR* dataPos = (TCHAR*)strMatchPrefix.data() + strMatchPrefix.size() - teamNameLen;
							memcpy(dataPos, teamName.m_psz, teamNameLen * sizeof(TCHAR));
						}
					} else if (j == 7 && cell_value.vt == VT_BSTR) {
						if (matchNameState == 2) {
							strMatchPrefix = strMatchPrefix + dmVS + CW2T(cell_value.bstrVal).m_psz;
						} else {
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
					} else if (j == 8 || j == 9 || j == 10) {
						if (cell_value.vt == VT_R8) {
							_stprintf(szR, _T("%.2f"), cell_value.dblVal);
							if (rateStr.empty()) {
								rateStr = szR;
							}
							else {
								rateStr = rateStr + _T('#') + szR;
							}
						}
					}
                }
            }
        }
		CStlStrArray arrTemp;
		Global::DepartString(rateStr, _T("#"), arrTemp);
		if (arrTemp.size() == 42) {
			m_strPL = rateStr.c_str();
		}
		Global::DepartString(matchStr, _T("\n"), arrTemp);
		if (arrTemp.size() == 14) {
			m_strMatchs = matchStr.c_str();
			std::wstring matchs_w = CT2W(matchStr.c_str()).m_psz;
		}
		DoDataExchange(FALSE);
    } catch(_com_error& error) {
        MessageBox(error.ErrorMessage(), "¥ÌŒÛ", MB_ICONERROR|MB_OK);
        return 1L;
    }
	try {
		if (pExcelApp.p != NULL) {
			pExcelApp->Quit();
		}
	} catch (...) {
	}
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