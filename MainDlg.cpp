// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "DialogDB.h"
#include "Global.h"
#include "Engine.h"

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg) {
    return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle() {
    return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
    // center the dialog on the screen
    CenterWindow();

    DoDataExchange(FALSE);

    // set icons
    HICON hIcon = (HICON) ::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
                                      IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON) ::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
                                           IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);

    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    DlgResize_Init();

    CRect rcDesktop ;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
    rcDesktop.DeflateRect(1, 1, 1, 1);


    SetWindowPos(NULL, &rcDesktop, SWP_NOZORDER);



    InitializeStatisData();
    ReloadStatisData();


    return TRUE;
}

void CMainDlg::InitializeStatisData() {

    DWORD dwStyleEx = LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
                      | LVS_EX_REGIONAL;
    ListView_SetExtendedListViewStyle(m_lstStatis.m_hWnd, dwStyleEx);

    //insert header;
    int colIndex = 0;
    m_lstStatis.InsertColumn(colIndex, "期号", LVCFMT_CENTER, 65);    //70
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);

    m_lstStatis.InsertColumn(colIndex, "奖金(万)", LVCFMT_LEFT, 80);    //170
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_DOUBLE);

    m_lstStatis.InsertColumn(colIndex, "均奖(万)", LVCFMT_LEFT, 80);    //170
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_DOUBLE);

    m_lstStatis.InsertColumn(colIndex, "号 码", LVCFMT_CENTER, 100);    //270
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

    m_lstStatis.InsertColumn(colIndex, "一赔", LVCFMT_CENTER, 45);    //70
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

    m_lstStatis.InsertColumn(colIndex, "二赔", LVCFMT_CENTER, 45);    //70
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

    m_lstStatis.InsertColumn(colIndex, "三赔", LVCFMT_CENTER, 45);    //70
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

    m_lstStatis.InsertColumn(colIndex, "陪率和", LVCFMT_LEFT, 55);    //270
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_DOUBLE);

    m_lstStatis.InsertColumn(colIndex, "概率积", LVCFMT_LEFT, 55);    //270
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_DOUBLE);

    m_lstStatis.InsertColumn(colIndex, "赔率范围", LVCFMT_CENTER, 100);    //270
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	m_lstStatis.InsertColumn(colIndex, "断点数", LVCFMT_LEFT, 50);    //270
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	m_lstStatis.InsertColumn(colIndex, "总3-总1-总0", LVCFMT_CENTER, 100);    //270
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	m_lstStatis.InsertColumn(colIndex, "连3-连1-连0", LVCFMT_CENTER, 100);    //270
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

    m_lstStatis.InsertColumn(colIndex, "31连-30连-10连", LVCFMT_CENTER, 100);    //270
    m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

    //set sort type
    m_lstStatis.SetSortColumn(0);

    //connect db
    m_pDbSystem = new COledbSystem();
    m_pDbDatabase = m_pDbSystem->CreateDatabase();
    CStlString strAppPath = Global::GetAppPath();
    CStringWTL strConnect = "Provider= Microsoft.Jet.OLEDB.4.0;";
    strConnect += "User ID=Admin;";
    strConnect = strConnect + CStringWTL("Data Source=") + CStringWTL(strAppPath.c_str()) + CStringWTL("ZcStatis.mdb;");
    BOOL bRet = m_pDbDatabase->Open(NULL, (LPCTSTR) strConnect, _T(""), _T(""), DB_OPEN_READ_ONLY);

}

void CMainDlg::ReloadStatisData() {
    m_arrPLSCOPE.clear();
    CStringWTL strSQL =  _T("select PLSCOPE from PLSCOPE");
    IDbRecordset *pRS1 = m_pDbSystem->CreateRecordset(m_pDbDatabase);
    pRS1->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY);
    while(!pRS1->IsEOF()) {
        double fPL = 0;
        pRS1->GetField(0, fPL);
        m_arrPLSCOPE.push_back(fPL);
        pRS1->MoveNext();

    }
    pRS1->Close();
    delete pRS1;

    m_lstStatis.DeleteAllItems();

    strSQL =  _T("select ID,BONUS,RESULT,PLDATA,SALES from PLDATA order by ID desc");
    IDbRecordset *pRS = m_pDbSystem->CreateRecordset(m_pDbDatabase);
    pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY);

    int iIndex = 0;
    while(!pRS->IsEOF()) {
        int colIndex = 0;
        float fBonus = 0;
        long lSales = 0;
        CStringWTL strQH;
        CStringWTL strCode;
        CStringWTL strPL;
        CStringWTL strBonus;
        CStringWTL strBonusAvg;

        pRS->GetField(0, strQH);
        pRS->GetField(1, fBonus);
        pRS->GetField(2, strCode);
        pRS->GetField(3, strPL);
        pRS->GetField(4, lSales);

        sprintf(strBonus.GetBuffer(255), "%.2f", fBonus);
        strBonus.ReleaseBuffer();

        double avgBonus = lSales;
        avgBonus = avgBonus / 20000000;
        if(lSales == 0) {
            avgBonus = 1.0;
        }
        fBonus = fBonus / (float)(avgBonus);
        if(fBonus >= 5000.0) {
            fBonus = 10000.0;
        } else if(fBonus >= 500) {
            fBonus = 500.0;
        }

        sprintf(strBonusAvg.GetBuffer(255), "%.2f", fBonus);
        strBonusAvg.ReleaseBuffer();

        iIndex = m_lstStatis.InsertItem(iIndex, strQH);
        m_lstStatis.SetItemText(iIndex, ++colIndex, strBonus);
        m_lstStatis.SetItemText(iIndex, ++colIndex, strBonusAvg);
        m_lstStatis.SetItemText(iIndex, ++colIndex, strCode);

		DataRow dataRow;
		GetPL(strCode, strPL, dataRow);

		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strPL1);
		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strPL2);
		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strPL3);

		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strPLSum);
		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strGvJ);
		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strPlSCOPE);

		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strCodeDuanDian);
		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strCodeZongShu);
		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strCodeLianOne);
		m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strCodeLianTwo);

        pRS->MoveNext();
    }

    pRS->Close();
    delete pRS;






    return ;
}



LRESULT CMainDlg::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CRect rcDesktop;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
    LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
    pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left;
    pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top;
    return 1L;
}


LRESULT CMainDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    if(m_pDbDatabase != NULL) {
        if(m_pDbDatabase->IsOpen()) {
            m_pDbDatabase->Close();
        }
        delete m_pDbDatabase;
    }
    if(m_pDbSystem != NULL) {
        delete m_pDbSystem;
    }
    DestroyWindow();
    ::PostQuitMessage(wID);
    return 1L;
}


LRESULT CMainDlg::OnAddRecord(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    CDialogDB dlg(m_pDbSystem, m_pDbDatabase);
    dlg.DoModal();
    ReloadStatisData();
    return 1L;
}

LRESULT CMainDlg::OnRefresh(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    ReloadStatisData();
    return 1L;
}



BOOL CMainDlg::GetPL(const CStringWTL &strCode, const CStringWTL &strPL1, DataRow &dataRow) {
    //CStlStrArray arrOther;
	CDoublexyArray arrPLData, arrGVData;
    CStlString strPL = strPL1;

    CIntArray tempArr;
    for(int i = 0 ; i < strCode.GetLength(); i++) {
        if(strCode[i] != '0' && strCode[i] != '1' && strCode[i] != '3') {
            return FALSE;
        }
        tempArr.push_back(strCode[i] - '0');
    }
	CEngine::GetPLDatas(strPL, arrPLData, arrGVData);
	CommonFilterFactors commonFF;
	CEngine::CalcCommonFilterFactors(tempArr, arrPLData, arrGVData, m_arrPLSCOPE, commonFF);

	dataRow.m_strCodeDuanDian.Format("%d", commonFF.mBreakCount);
	dataRow.m_strCodeZongShu.Format("%02d-%02d-%02d", commonFF.mTotal3Count, commonFF.mTotal1Count, commonFF.mTotal0Count);
	dataRow.m_strCodeLianOne.Format("%02d-%02d-%02d", commonFF.mLian3Count, commonFF.mLian1Count, commonFF.mLian0Count);
	dataRow.m_strCodeLianTwo.Format("%02d-%02d-%02d", commonFF.mLian31Count, commonFF.mLian30Count, commonFF.mLian10Count);


	dataRow.m_strPL1.Format("%02d", commonFF.mPL1Count);
	dataRow.m_strPL2.Format("%02d", commonFF.mPL2Count);
	dataRow.m_strPL3.Format("%02d", commonFF.mPL3Count);

	sprintf(dataRow.m_strPLSum.GetBuffer(255), "%.2f", commonFF.mPLSum);
	dataRow.m_strPLSum.ReleaseBuffer();

	sprintf(dataRow.m_strGvJ.GetBuffer(255), "%.3f", commonFF.mGvj * 10 * 10 * 10 * 10 * 10 * 10);
	dataRow.m_strGvJ.ReleaseBuffer();

    
	for (int i = 0; i < commonFF.mPLScopes.size(); i++) {
        CStringWTL strTemp;
		strTemp.Format("%02d-", commonFF.mPLScopes[i]);
		dataRow.m_strPlSCOPE += strTemp;
    }
	dataRow.m_strPlSCOPE = dataRow.m_strPlSCOPE.Left(dataRow.m_strPlSCOPE.GetLength() - 1);

    return TRUE;
}