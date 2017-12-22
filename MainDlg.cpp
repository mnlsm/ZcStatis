// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "DialogDB.h"
#include "Global.h"
#include "Engine.h"
#include "DialogGambel.h"
#include <SQLiteCpp/SQLiteCpp.h>

CMainDlg::CMainDlg() : m_lstStatis(this, 1) {
	m_pDbSystem = NULL;
	m_pDbDatabase = NULL;
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg) {
    return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle() {
    return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
    // center the dialog on the screen
    CenterWindow();

	InitControls();

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

LRESULT CMainDlg::OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstStatis.DefWindowProc(uMsg, wParam, lParam);
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LVHITTESTINFO lvh = { 0 };
	lvh.pt = pt;
	UINT index = m_lstStatis.HitTest(&lvh);
	if (index != -1) {
		CMenu menu;
		if (menu.CreatePopupMenu()) {
			menu.AppendMenu(MF_STRING, IDM_EDIT_FANGANS, _T("编辑方案"));
			menu.AppendMenu(MF_STRING, IDM_DELETE_PLDATA, _T("删除记录"));
			m_lstStatis.ClientToScreen(&pt);
			UINT cmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
			DoListMenuCommand(cmd, index);
			menu.DestroyMenu();
		}
	}
	return lRet;
}

LRESULT CMainDlg::OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstStatis.DefWindowProc(uMsg, wParam, lParam);
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LVHITTESTINFO lvh = { 0 };
	lvh.pt = pt;
	UINT index = m_lstStatis.HitTest(&lvh);
	if (index != -1) {
		DoListMenuCommand(IDM_EDIT_FANGANS, index);
	}
	return 1L;
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

void CMainDlg::InitControls() {
	DoDataExchange(FALSE);

	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR|LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR|LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);

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

	m_lstStatis.InsertColumn(colIndex, "不均衡赔率", LVCFMT_CENTER, 80);    //270
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstStatis.InsertColumn(colIndex, "赔率分布统计", LVCFMT_CENTER, 150);    //270
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	//set sort type
	m_lstStatis.SetSortColumn(0);
}

void CMainDlg::InitializeStatisData() {
	//connect db
	m_pDbSystem = new COledbSystem();
	m_pDbDatabase = m_pDbSystem->CreateDatabase();
	CStlString strAppPath = Global::GetAppPath();
	CStringATL strConnect = "Provider= Microsoft.Jet.OLEDB.4.0;";
	strConnect += "User ID=Admin;";
	strConnect = strConnect + CStringATL("Data Source=") + CStringATL(strAppPath.c_str()) + CStringATL("ZcStatis.mdb;");
	BOOL bRet = m_pDbDatabase->Open(NULL, (LPCTSTR)strConnect, _T(""), _T(""), DB_OPEN_READ_ONLY);
}

static void toSqliteDB(const CStringATL& qh, double bonus,
	const CStringATL& codes, const CStringATL& pl, long sales, const CStringATL& strMatchs) {
	using namespace SQLite;
	CStlString strAppPath = Global::GetAppPath();
	CStlString strDbFilePath = strAppPath + _T("ZcStatis.db3");
	CStlString strSQL = _T("INSERT INTO PLDATA VALUES (@ID, @BONUS,@RESULT,@PLDATA,@SALES,@MATCHS)");
	try {
		Database db(strDbFilePath.c_str(), OPEN_READWRITE);
		Statement sm(db, strSQL.c_str());
		sm.reset();
		sm.clearBindings();
		sm.bind("@ID", (LPCTSTR)qh);
		sm.bind("@BONUS", bonus);
		sm.bind("@RESULT", (LPCTSTR)codes);
		sm.bind("@PLDATA", (LPCTSTR)pl);
		sm.bind("@SALES", sales);
		sm.bind("@MATCHS", Global::toUTF8((LPCTSTR)strMatchs));
		sm.exec();
	}
	catch (Exception& err) {
		strSQL = err.getErrorStr();
	}
}

void CMainDlg::ReloadStatisData() {
	m_lstStatis.DeleteAllItems();

	m_arrPLSCOPE.clear();
	CStringATL strSQL = _T("select PLSCOPE from PLSCOPE");
	std::unique_ptr<IDbRecordset> pRS1(m_pDbSystem->CreateRecordset(m_pDbDatabase));
	if (pRS1->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY)) {
		while (!pRS1->IsEOF()) {
			double fPL = 0;
			pRS1->GetField(0, fPL);
			m_arrPLSCOPE.push_back(fPL);
			pRS1->MoveNext();
		}
	}
	pRS1->Close();

	strSQL = _T("SELECT ID,BONUS,RESULT,PLDATA,SALES,MATCHS from PLDATA ORDER BY ID ASC");
	std::unique_ptr<IDbRecordset> pRS(m_pDbSystem->CreateRecordset(m_pDbDatabase));
	if(pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY)) {
		int iIndex = 0;
		while (!pRS->IsEOF()) {
			int colIndex = 0;
			float fBonus = 0;
			long lSales = 0;
			CStringATL strQH;
			CStringATL strCode;
			CStringATL strPL;
			CStringATL strBonus;
			CStringATL strBonusAvg;
			CStringATL strMatchs;

			pRS->GetField(0, strQH);
			pRS->GetField(1, fBonus);
			pRS->GetField(2, strCode);
			pRS->GetField(3, strPL);
			pRS->GetField(4, lSales);
			pRS->GetField(5, strMatchs);

			
			toSqliteDB(strQH, fBonus, strCode, strPL, lSales, strMatchs);
			sprintf(strBonus.GetBuffer(255), "%.2f", fBonus);
			strBonus.ReleaseBuffer();

			double avgBonus = lSales;
			avgBonus = avgBonus / 20000000;
			if (lSales == 0) {
				avgBonus = 1.0;
			}
			fBonus = fBonus / (float)(avgBonus);
			if (fBonus >= 5000.0) {
				fBonus = 10000.0;
			}
			else if (fBonus >= 500) {
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
			m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strPLAvgCount);
			m_lstStatis.SetItemText(iIndex, ++colIndex, dataRow.m_strPLAvg);

			pRS->MoveNext();
		}
	}
	pRS->Close();
	m_lstStatis.DoSortItems(0, false);
	return;
}





BOOL CMainDlg::GetPL(const CStringATL &strCode, const CStringATL &strPL1, DataRow &dataRow) {
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

	dataRow.m_strCodeDuanDian.Format(_T("%d"), commonFF.mBreakCount);
	dataRow.m_strCodeZongShu.Format("%02d-%02d-%02d", commonFF.mTotal3Count, commonFF.mTotal1Count, commonFF.mTotal0Count);
	dataRow.m_strCodeLianOne.Format("%02d-%02d-%02d", commonFF.mLian3Count, commonFF.mLian1Count, commonFF.mLian0Count);
	dataRow.m_strCodeLianTwo.Format("%02d-%02d-%02d", commonFF.mLian31Count, commonFF.mLian30Count, commonFF.mLian10Count);


	dataRow.m_strPL1.Format("%02d", commonFF.mPL1Count);
	dataRow.m_strPL2.Format("%02d", commonFF.mPL2Count);
	dataRow.m_strPL3.Format("%02d", commonFF.mPL3Count);

	sprintf(dataRow.m_strPLSum.GetBuffer(255), "%.2f", commonFF.mPLSum);
	dataRow.m_strPLSum.ReleaseBuffer();

	sprintf(dataRow.m_strGvJ.GetBuffer(255), "%.4f", commonFF.mGvj * 10 * 10 * 10 * 10 * 10 * 10);
	dataRow.m_strGvJ.ReleaseBuffer();

 	for (int i = 0; i < commonFF.mPLScopes.size(); i++) {
        CStringATL strTemp;
		strTemp.Format("%02d-", commonFF.mPLScopes[i]);
		dataRow.m_strPlSCOPE += strTemp;
    }
	dataRow.m_strPlSCOPE = dataRow.m_strPlSCOPE.Left(dataRow.m_strPlSCOPE.GetLength() - 1);
	dataRow.m_strPLAvg.Format("%02d-%02d-%02d-%02d-%02d-%02d",
		commonFF.mPLAvgs[0], commonFF.mPLAvgs[1], commonFF.mPLAvgs[2],
		commonFF.mPLAvgs[3], commonFF.mPLAvgs[4], commonFF.mPLAvgs[5]);
	dataRow.m_strPLAvgCount.Format(_T("%d"),
		commonFF.mPLAvgs[0] + commonFF.mPLAvgs[4] + commonFF.mPLAvgs[5]);
    return TRUE;
}

void CMainDlg::DoListMenuCommand(UINT cmd, UINT nItem) {
	CStringATL strQH;
	m_lstStatis.GetItemText(nItem, 0, strQH);
	if (cmd == IDM_EDIT_FANGANS) {
		CDialogGambel dlg(m_pDbSystem, m_pDbDatabase, (LPCTSTR)strQH);
		dlg.DoModal();
		return;
	} else if (cmd == IDM_DELETE_PLDATA) {
		std::unique_ptr<IDbCommand> pCmd(m_pDbSystem->CreateCommand(m_pDbDatabase));
		CStringATL strSQL = _T("DELETE FROM PLDATA WHERE ID=?");
		pCmd->Create(strSQL);
		pCmd->SetParam(0, strQH);
		if (pCmd->Execute(NULL)) {
			ReloadStatisData();
		}
		return;
	}
}