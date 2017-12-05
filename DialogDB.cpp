// DialogDB.cpp : CDialogDB µƒ µœ÷

#include "stdafx.h"
#include "DialogDB.h"
#include "Global.h"

#include "mso.tlh"
#include "vbe6ext.tlh"
#include "excel.tlh"

void CDialogDB::LoadData() {
    m_lstQH.ResetContent();

    CStringATL strSQL = _T("SELECT ID FROM PLDATA ORDER BY ID DESC");
    IDbRecordset *pRS = m_pDbSystem->CreateRecordset(m_pDbDatabase);
    pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY);

    while(!pRS->IsEOF()) {
        CStringATL strID;
        pRS->GetField(0, strID);
        strID.TrimLeft();
        strID.TrimRight();
        if(!strID.IsEmpty()) {
            m_lstQH.AddString(strID);
        }
        pRS->MoveNext();
    }

    pRS->Close();
    delete pRS;
}


// CDialogDB
LRESULT CDialogDB::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CAxDialogImpl<CDialogDB>::OnInitDialog(uMsg, wParam, lParam, bHandled);
    DoDataExchange(FALSE);

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
        MessageBox("∆⁄∫≈ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_OK);
        return 1L;
    }
    for(int i = 0 ; i < nLenQH ; i++) {
        if(m_strQH[i] < '0' || m_strQH[i] > '9') {
            MessageBox("∆⁄∫≈ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_OK);
            return 1L;
        }
    }

	if (m_strPL.IsEmpty()) {
		MessageBox("≈‚¬  ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_OK);
		return 1L;
	}

    int nLenCode = m_strCode.GetLength();
    if(nLenCode != 14) {
		m_strCode.Empty();
        //MessageBox(_T("Ω·π˚ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°"), _T("¥ÌŒÛ"), MB_OK);
        //return 1L;
    }
	for(int i = 0 ; i < nLenCode ; i++) {
        if(m_strCode[i] != '0' && m_strCode[i] != '1' && m_strCode[i] != '3') {
            //MessageBox("Ω·π˚ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_OK);
            //return 1L;
			m_strCode.Empty();
			break;
        }
    }


    //CStringATL strSQL;
    //strSQL.Format( _T(" delete from PLDATA where ID = '%s' "),m_strQH);
    //m_pDbDatabase->ExecuteSQL(m_strQH);
    CStringATL strSQL =  _T("delete from PLDATA where ID = ?");
    IDbCommand *pCmd1 = m_pDbSystem->CreateCommand(m_pDbDatabase);
    pCmd1->Create(strSQL);
    pCmd1->SetParam(0, m_strQH);
    pCmd1->Execute(NULL);
    pCmd1->Close();
    delete pCmd1;

    float fBonus = _ttof(m_strBonus);
	long lSales = _ttol(m_strSales);
    strSQL = _T("INSERT INTO PLDATA (ID, BONUS,RESULT,PLDATA,SALES,MATCHS) values(?,?,?,?,?,?)");
    IDbCommand *pCmd = m_pDbSystem->CreateCommand(m_pDbDatabase);
    pCmd->Create(strSQL);
    pCmd->SetParam(0, m_strQH);
    pCmd->SetParam(1, &fBonus);
	if (!m_strCode.IsEmpty()) {
		pCmd->SetParam(2, m_strCode);
	} else {
		pCmd->SetParam(2, "00000000000000");
	}
    pCmd->SetParam(3, m_strPL);
	pCmd->SetParam(4, &lSales);
	pCmd->SetParam(5, m_strMatchs);

    pCmd->Execute(NULL);
    pCmd->Close();
    delete pCmd;

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

    CStringATL strSQL =  _T("delete from PLDATA where ID = ?");
    IDbCommand *pCmd = m_pDbSystem->CreateCommand(m_pDbDatabase);
    pCmd->Create(strSQL);
    pCmd->SetParam(0, strID);
    pCmd->Execute(NULL);

    pCmd->Close();
    delete pCmd;

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
    strSQL.Format(_T("select ID,BONUS,RESULT,PLDATA,SALES,MATCHS from PLDATA where ID='%s'"), strID);
    IDbRecordset *pRS = m_pDbSystem->CreateRecordset(m_pDbDatabase);
    pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY);

    if(!pRS->IsEOF()) {
        float fBonus = 0;
		long lSales = 0;
        pRS->GetField(0, m_strQH);
        pRS->GetField(1, fBonus);
        sprintf(m_strBonus.GetBuffer(255), "%.2f", fBonus);
        m_strBonus.ReleaseBuffer();
        pRS->GetField(2, m_strCode);
        pRS->GetField(3, m_strPL);
		pRS->GetField(4, lSales);
		sprintf(m_strSales.GetBuffer(255), "%u", lSales);
		m_strSales.ReleaseBuffer();
		pRS->GetField(5, m_strMatchs);
    }
    pRS->Close();
    delete pRS;

    DoDataExchange(FALSE);

    return 0;

}


LRESULT CDialogDB::OnClickedExcel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    DoDataExchange(TRUE);
    int nLenQH = m_strQH.GetLength();
    if(nLenQH != 5) {
        MessageBox("∆⁄∫≈ ‰»Î¥ÌŒÛ£¨«ÎºÏ≤È£°", "¥ÌŒÛ", MB_OK);
        return 1L;
    }
    CStlString fn = Global::GetAppPath() + _T("odds\\") + (LPCTSTR)m_strQH + _T(".xls");
	CComPtr<Excel::_Application> pExcelApp;
	try {
        CT2OLE fn_ole(fn.c_str());

        Excel::_ApplicationPtr pExcelApp1;
        HRESULT hr = pExcelApp1.CreateInstance(L"Excel.Application");
        if(FAILED(hr)) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 0£°", "¥ÌŒÛ", MB_OK);
            return 1L;
        }
        pExcelApp = pExcelApp1.GetInterfacePtr();

        //HRESULT hr = pExcelApp.CoCreateInstance(L"Excel.Application", NULL);
        if(FAILED(hr)) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 0£°", "¥ÌŒÛ", MB_OK);
            return 1L;
        }
        CComPtr<Excel::Workbooks> pWorkbooks;
        hr = pExcelApp->get_Workbooks(&pWorkbooks);
        if(FAILED(hr)) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 1£°", "¥ÌŒÛ", MB_OK);
            return 1L;
        }
        CComPtr<Excel::_Workbook> pWorkbook;
		hr = pWorkbooks->raw_Open(CComBSTR(fn_ole.m_psz), vtMissing, vtMissing,
                                  vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,
                                  vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,
                                  0, &pWorkbook);
        if(FAILED(hr) || pWorkbook.p == NULL) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 2£°", "¥ÌŒÛ", MB_OK);
            return 1L;
        }
        CComPtr<Excel::Sheets> pSheets;
        hr = pWorkbook->get_Sheets(&pSheets);
        if(FAILED(hr) || pSheets.p == NULL) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 5£°", "¥ÌŒÛ", MB_OK);
            return 1L;
        }
        long sheet_count = 0;
        hr = pSheets->get_Count(&sheet_count);
        if(FAILED(hr) || sheet_count <= 0) {
            MessageBox("Excel Com ≥ı ºªØ ß∞‹ 6£°", "¥ÌŒÛ", MB_OK);
            return 1L;
        }
        CStlString rateStr, matchStr;
		const std::string dmVS = "    VS    ";
        for(long j = 0; j < sheet_count; j++) {
            CComPtr<IDispatch> pIDispath;
            CComVariant vIndex(j + 1);
            hr = pSheets->get_Item(vIndex, &pIDispath);
            if(FAILED(hr) || pIDispath.p == NULL) {
                MessageBox("Excel Com ≥ı ºªØ ß∞‹ 7£°", "¥ÌŒÛ", MB_OK);
                return 1L;
            }
            CComQIPtr<Excel::_Worksheet> pWorksheet(pIDispath);
            CComBSTR sheet_name;
            hr = pWorksheet->get_Name(&sheet_name);
            if(FAILED(hr) || sheet_name == NULL) {
                MessageBox("Excel Com ≥ı ºªØ ß∞‹ 8£°", "¥ÌŒÛ", MB_OK);
                return 1L;
            }
			if (wcscmp(sheet_name.m_str, L"Sheet1") != 0) {
				continue;
			}
            CComPtr<Excel::Range> pRootCells;
            hr = pWorksheet->get_UsedRange(0, &pRootCells);
            if(FAILED(hr) || pRootCells.p == NULL) {
                MessageBox("Excel Com ≥ı ºªØ ß∞‹ 9£°", "¥ÌŒÛ", MB_OK);
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
        MessageBox(error.ErrorMessage(), "¥ÌŒÛ", MB_OK);
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