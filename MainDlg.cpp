// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"


#include "MainDlg.h"

#include "DialogDB.h"


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
    rcDesktop.DeflateRect(1,1,1,1);


    SetWindowPos(NULL,&rcDesktop,SWP_NOZORDER);



    InitializeStatisData();
    ReloadStatisData();


    return TRUE;
}

void CMainDlg::InitializeStatisData() {

    DWORD dwStyleEx=LVS_EX_GRIDLINES |LVS_EX_INFOTIP|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP
                    |LVS_EX_REGIONAL;
//	m_ctlList=GetDlgItem(IDC_LIWIN);
    ListView_SetExtendedListViewStyle(m_lstStatis.m_hWnd,dwStyleEx);


    //insert header;
    m_lstStatis.InsertColumn(0,"期号",LVCFMT_CENTER,65);    //70
    m_lstStatis.SetColumnSortType(0,LVCOLSORT_TEXT);

    m_lstStatis.InsertColumn(1,"奖金(万)",LVCFMT_LEFT,90);    //170
    m_lstStatis.SetColumnSortType(1,LVCOLSORT_DOUBLE);

    m_lstStatis.InsertColumn(2,"号 码",LVCFMT_CENTER,100);    //270
    m_lstStatis.SetColumnSortType(2,LVCOLSORT_NONE);

    m_lstStatis.InsertColumn(3,"31连-30连-10连",LVCFMT_CENTER,100);    //270
    m_lstStatis.SetColumnSortType(3,LVCOLSORT_NONE);

    m_lstStatis.InsertColumn(4,"一赔",LVCFMT_LEFT,65);    //70
    m_lstStatis.SetColumnSortType(4,LVCOLSORT_LONG);

    m_lstStatis.InsertColumn(5,"二赔",LVCFMT_LEFT,65);    //70
    m_lstStatis.SetColumnSortType(5,LVCOLSORT_LONG);

    m_lstStatis.InsertColumn(6,"三赔",LVCFMT_LEFT,65);    //70
    m_lstStatis.SetColumnSortType(6,LVCOLSORT_LONG);


    m_lstStatis.InsertColumn(7,"陪率和",LVCFMT_LEFT,75);    //270
    m_lstStatis.SetColumnSortType(7,LVCOLSORT_DOUBLE);

    m_lstStatis.InsertColumn(8,"概率积",LVCFMT_LEFT,75);    //270
    m_lstStatis.SetColumnSortType(8,LVCOLSORT_DOUBLE);

    m_lstStatis.InsertColumn(9,"赔率范围",LVCFMT_CENTER,265);    //270
    m_lstStatis.SetColumnSortType(9,LVCOLSORT_NONE);


    //set sort type
    m_lstStatis.SetSortColumn(0);


    //connect db
    m_pDbSystem = new COledbSystem();
    m_pDbDatabase = m_pDbSystem->CreateDatabase();
    CStlString strAppPath = GetAppPath();
    CStringWTL strConnect = "Provider= Microsoft.Jet.OLEDB.4.0;";
    strConnect += "User ID=Admin;";
    strConnect = strConnect + CStringWTL("Data Source=") + CStringWTL(strAppPath.c_str()) + CStringWTL("ZcStatis.mdb;");
    BOOL bRet = m_pDbDatabase->Open(NULL, (LPCTSTR) strConnect, _T(""), _T(""), DB_OPEN_READ_ONLY);



}

void CMainDlg::ReloadStatisData() {
    m_arrPLSCOPE.clear();
    CStringWTL strSQL =  _T(" select PLSCOPE from PLSCOPE  ");
    IDbRecordset *pRS1 = m_pDbSystem->CreateRecordset(m_pDbDatabase);
    pRS1->Open(strSQL,DB_OPEN_TYPE_FORWARD_ONLY);
    while(!pRS1->IsEOF()) {
        double fPL = 0;
        pRS1->GetField(0,fPL);
        m_arrPLSCOPE.push_back(fPL);
        pRS1->MoveNext();

    }
    pRS1->Close();
    delete pRS1;

    m_lstStatis.DeleteAllItems();

    strSQL =  _T(" select ID,BONUS,RESULT,PLDATA from PLDATA  ");
    IDbRecordset *pRS = m_pDbSystem->CreateRecordset(m_pDbDatabase);
    pRS->Open(strSQL,DB_OPEN_TYPE_FORWARD_ONLY);

    int iIndex = 0;
    while(!pRS->IsEOF()) {
        float fBonus = 0;
        CStringWTL strQH;
        CStringWTL strCode;
        CStringWTL strPL;
        CStringWTL strBonus;

        pRS->GetField(0, strQH);
        pRS->GetField(1, fBonus);
        pRS->GetField(2, strCode);
        pRS->GetField(3, strPL);

        sprintf(strBonus.GetBuffer(255),"%.2f",fBonus);
        strBonus.ReleaseBuffer();


        iIndex = m_lstStatis.InsertItem(iIndex,strQH);
        m_lstStatis.SetItemText(iIndex,1,strBonus);
        m_lstStatis.SetItemText(iIndex,2,strCode);

        long nMaxSP = 0,nMaxSF = 0,nMaxPF = 0;
        GetLianXu(strCode,nMaxSP,nMaxSF,nMaxPF);
        CStringWTL strLianXu;
        strLianXu.Format("%02d-%02d-%02d",nMaxSP,nMaxSF,nMaxPF);
        m_lstStatis.SetItemText(iIndex,3,strLianXu);

        DataPL dataPL;
        GetPL(strCode,strPL,dataPL);

        m_lstStatis.SetItemText(iIndex,4,dataPL.m_strPL1);
        m_lstStatis.SetItemText(iIndex,5,dataPL.m_strPL2);
        m_lstStatis.SetItemText(iIndex,6,dataPL.m_strPL3);

        m_lstStatis.SetItemText(iIndex,7,dataPL.m_strPLSum);
        m_lstStatis.SetItemText(iIndex,8,dataPL.m_strGvJ);
        m_lstStatis.SetItemText(iIndex,9,dataPL.m_strPlSCOPE);




        pRS->MoveNext();
    }

    pRS->Close();
    delete pRS;






    return ;
}



LRESULT CMainDlg::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CRect rcDesktop;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
//	rcDesktop.DeflateRect(1,1,1,1);


    LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);

    pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left;
    pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top;





    return 1L;
}


LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {

    if(m_pDbDatabase != NULL) {
        if(m_pDbDatabase->IsOpen())
            m_pDbDatabase->Close();

        delete m_pDbDatabase;
    }

    if(m_pDbSystem != NULL)
        delete m_pDbSystem;


    DestroyWindow();
    ::PostQuitMessage(wID);
    return 1L;
}


LRESULT CMainDlg::OnAddRecord(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    CDialogDB dlg(m_pDbSystem,m_pDbDatabase);
    dlg.DoModal();

    return 1L;
}

LRESULT CMainDlg::OnRefresh(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    ReloadStatisData();
    return 1L;
}



BOOL CMainDlg::GetLianXu(const CStringWTL &strCode,long &nMaxSP,long &nMaxSF,long &nMaxPF) {
    CIntArray tempArr;
    for(int i = 0 ; i < strCode.GetLength() ; i++) {
        if(strCode[i] != '0'
                &&strCode[i] != '1'
                &&strCode[i] != '3')
            return FALSE;
        tempArr.push_back(strCode[i] - '0');
    }




    if(tempArr.size() != 14)
        return FALSE;

    CIntPair pairCurSP,pairCurSF,pairCurPF;
    pairCurSP.first = 0;
    pairCurSP.second = 0;
    pairCurSF.first = 0;
    pairCurSF.second = 0;
    pairCurPF.first = 0;
    pairCurPF.second = 0;

    for(int i = 0; i < tempArr.size(); i++) {
        if(tempArr[i] == 3) {
            ++pairCurSP.first;
            ++pairCurSF.first;

            if(pairCurPF.first != 0 && pairCurPF.second != 0) {
                if(nMaxPF < pairCurPF.first + pairCurPF.second)
                    nMaxPF = pairCurPF.first + pairCurPF.second;
            }
            pairCurPF.first = 0;
            pairCurPF.second = 0;

        }
        if(tempArr[i] == 1) {
            ++pairCurSP.second;
            ++pairCurPF.first;

            if(pairCurSF.first != 0 && pairCurSF.second != 0) {
                if(nMaxSF < pairCurSF.first + pairCurSF.second)
                    nMaxSF = pairCurSF.first + pairCurSF.second;
            }
            pairCurSF.first = 0;
            pairCurSF.second = 0;


        }
        if(tempArr[i] == 0) {
            ++pairCurSF.second;
            ++pairCurPF.second;

            if(pairCurSP.first != 0 && pairCurSP.second != 0) {
                if(nMaxSP < pairCurSP.first + pairCurSP.second)
                    nMaxSP = pairCurSP.first + pairCurSP.second;
            }

            pairCurSP.first = 0;
            pairCurSP.second = 0;
        }
    }

    if(pairCurSP.first != 0 && pairCurSP.second != 0) {
        if(nMaxSP < pairCurSP.first + pairCurSP.second)
            nMaxSP = pairCurSP.first + pairCurSP.second;
    }

    if(pairCurSF.first != 0 && pairCurSF.second != 0) {
        if(nMaxSF < pairCurSF.first + pairCurSF.second)
            nMaxSF = pairCurSF.first + pairCurSF.second;
    }


    if(pairCurPF.first != 0 && pairCurPF.second != 0) {
        if(nMaxPF < pairCurPF.first + pairCurPF.second)
            nMaxPF = pairCurPF.first + pairCurPF.second;
    }

    return FALSE;
}

BOOL CMainDlg::GetPL(const CStringWTL &strCode, const CStringWTL &strPL1,DataPL &dataPL) {
    CStlStrArray arrOther;
    CDoublexyArray m_arrOthersData,m_arrOthersDataPL;
	CStlString strPL = strPL1;

    CIntArray tempArr;
    for(int i = 0 ; i < strCode.GetLength(); i++) {
		if (strCode[i] != '0' && strCode[i] != '1' && strCode[i] != '3') {
			return FALSE;
		}
        tempArr.push_back(strCode[i] - '0');
    }


	if (!DepartString(strPL, "#", arrOther)) {
		return FALSE;
	}

	if (arrOther.size() != 42) {
		return FALSE;
	}


    for(int i = 0; i < arrOther.size(); i = i + 3) {
        CDoubleArray arrTemp,arrTemp1;

        double fTemp = 0.9;
        double dGap = 0.00;

        double d3 = fTemp/atof(arrOther[i].c_str());
        double d1 = fTemp/atof(arrOther[i + 1].c_str());
        double d0 = fTemp/atof(arrOther[i + 2].c_str());

        dGap = 1 - (d3 + d1 + d0);
        d3+= (dGap/3);
        d1+= (dGap/3);
        d0+= (dGap/3);

        arrTemp1.push_back(atof(arrOther[i].c_str()));
        arrTemp1.push_back(atof(arrOther[i + 1].c_str()));
        arrTemp1.push_back(atof(arrOther[i + 2].c_str()));

        arrTemp.push_back(d3);
        arrTemp.push_back(d1);
        arrTemp.push_back(d0);
        m_arrOthersData.push_back(arrTemp);
        m_arrOthersDataPL.push_back(arrTemp1);

    }
    if(m_arrOthersData.size() != 14 && m_arrOthersDataPL.size() != 14) {
        m_arrOthersData.clear();
        return FALSE;
    }

    long PL1 = 0, PL2 = 0, PL3 = 0;
    double dGVJ = 1,PLSUM = 0;
    int arrSCOPE[1024];
    for(int i = 0 ; i < 1024 ; i++)
        arrSCOPE[i] = 0;

    for(int i = 0 ; i < tempArr.size() ; i++) {
        int nIndex = 0;
        if(tempArr[i] == 1)
            nIndex = 1;
        else if(tempArr[i] == 0)
            nIndex = 2;


        CDoubleArray arrTemp = m_arrOthersDataPL[i];
        CDoubleArray arrTemp1 = m_arrOthersData[i];
        double dCurPL = arrTemp[nIndex];
        double dCurGV = arrTemp1[nIndex];

        PLSUM += dCurPL;

        std::stable_sort(arrTemp.begin(),arrTemp.end(),std::less<double>());
        if(arrTemp[0] == dCurPL) PL1++;
        else if(arrTemp[1] == dCurPL) PL2++;
        else PL3++;
        dGVJ = dGVJ * dCurGV;

        for(int j = 1 ; j < m_arrPLSCOPE.size(); j++) {
            if(dCurPL > m_arrPLSCOPE[j - 1] && dCurPL <= m_arrPLSCOPE[j])
                arrSCOPE[j-1]++;
        }

    }

    dataPL.m_strPL1.Format("%02d",PL1);
    dataPL.m_strPL2.Format("%02d",PL2);
    dataPL.m_strPL3.Format("%02d",PL3);

    sprintf(dataPL.m_strPLSum.GetBuffer(255),"%.2f",PLSUM);
    dataPL.m_strPLSum.ReleaseBuffer();

    sprintf(dataPL.m_strGvJ.GetBuffer(255),"%.3f",dGVJ * 10 * 10 * 10 * 10 * 10 * 10);
    dataPL.m_strGvJ.ReleaseBuffer();

    if(m_arrPLSCOPE.size() > 1) {
        for(int i = 0 ; i < m_arrPLSCOPE.size() - 1; i++) {
            CStringWTL strTemp;
            strTemp.Format("%02d-",arrSCOPE[i]);
            dataPL.m_strPlSCOPE += strTemp;
        }
        dataPL.m_strPlSCOPE = dataPL.m_strPlSCOPE.Left(dataPL.m_strPlSCOPE.GetLength() - 1);
    }

    return TRUE;
}