#include "stdafx.h"
#include "DialogGambel.h"
#include "DialogTZ.h"

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

LRESULT CDialogGambel::OnClickedListMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
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
			menu.AppendMenu(MF_STRING, IDM_EDIT_FANGAN, _T("编辑方案"));
			menu.AppendMenu(MF_STRING, IDM_EDIT_SCRIPT, _T("编辑脚本"));
			menu.AppendMenu(MF_STRING, IDM_DELETE_RESULT, _T("删除投注"));
			menu.AppendMenu(MF_STRING, IDM_DELETE_FANGAN, _T("删除方案"));
			ClientToScreen(&pt);
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON /*| TPM_RETURNCMD*/, pt.x, pt.y, m_hWnd);
			menu.DestroyMenu();
		}
	}
	return lRet;
}

LRESULT CDialogGambel::OnListLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstGambel.OnLButtonDown(uMsg, wParam, lParam, bHandled);
	return lRet;
}

void CDialogGambel::InitControls() {
	DoDataExchange(FALSE);

	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);
	
	DWORD dwStyleEx = ListView_GetExtendedListViewStyle(m_lstGambel.m_hWnd);
	dwStyleEx |= (LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL);
	ListView_SetExtendedListViewStyle(m_lstGambel.m_hWnd, dwStyleEx);

	int colIndex = 0;
	m_lstGambel.InsertColumn(colIndex++, "编号", LVCFMT_LEFT, 65);    //70
	m_lstGambel.InsertColumn(colIndex++, "方案类型", LVCFMT_LEFT, 65);    //70
	m_lstGambel.InsertColumn(colIndex++, "方案数据", LVCFMT_LEFT, 200);    //70
	m_lstGambel.InsertColumn(colIndex++, "脚本数据", LVCFMT_LEFT, 100);    //70
	m_lstGambel.InsertColumn(colIndex++, "结果数据", LVCFMT_LEFT, 100);    //70

	CStringATL strTemp;         
	strTemp.Format(_T("方案列表(%s)"), m_strQH.c_str());
	CWindow wnd = GetDlgItem(IDC_STGAMBELS);
	wnd.SetWindowText(strTemp);
	
}

void CDialogGambel::ReloadFangAnData() {
	m_arrDbData.clear();
	m_lstGambel.DeleteAllItems();
	CStringATL strSQL;
	strSQL.Format(_T("select ID, INUSE, CODESTYPE, CODES, PLDATA, SCRIPT, \
		RESULT FROM GAMBEL WHERE QH='?' order by ID asc"), m_strQH.c_str());
	std::auto_ptr<IDbRecordset> pRS(m_pDbSystem->CreateRecordset(m_pDbDatabase));
	pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY);
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
	pRS->Close();
	CStringATL strNum;
	for (const auto& row : m_arrDbData) {
		strNum.Format(_T("%u"), row.m_nID);
		UINT rowIndex = m_lstGambel.InsertItem(-1, strNum);
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
		if (row.m_strResult.IsEmpty()) {
			m_lstGambel.SetItemText(rowIndex, ++colIndex, row.m_strCodes.Left(15) + _T(" ......"));
		}
	}
}