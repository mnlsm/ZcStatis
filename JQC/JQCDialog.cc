#include "stdafx.h"
#include "JQCDialog.h"
#include "Global.h"

JQCDialog::JQCDialog()
	:m_lstStatis(this, 1) {

}

LRESULT JQCDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CenterWindow();
	InitControls();
	DlgResize_Init();

	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	int gapX = rcDesktop.Width() / 10;
	int gapY = rcDesktop.Height() / 10;
	rcDesktop.DeflateRect(gapX, gapY, gapX, gapY);
	SetWindowPos(NULL, &rcDesktop, SWP_NOZORDER);

	ReloadStatisData();
	return TRUE;
}

LRESULT JQCDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
	pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left;
	pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top;
	return 1L;
}

LRESULT JQCDialog::OnInitMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CMenuHandle menu((HMENU)wParam);
	menu.RemoveMenu(IDM_ADDRECORD, MF_BYCOMMAND);
	menu.RemoveMenu(IDM_JQC, MF_BYCOMMAND);
	return 1L;
}


LRESULT JQCDialog::OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstStatis.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}

LRESULT JQCDialog::OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstStatis.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}

LRESULT JQCDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	EndDialog(0);
	return 1L;
}

LRESULT JQCDialog::OnAddRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT JQCDialog::OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ReloadStatisData();
	return 1L;
}

void JQCDialog::InitControls() {
	DoDataExchange(FALSE);

	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);

	DWORD dwStyleEx = LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL;
	ListView_SetExtendedListViewStyle(m_lstStatis.m_hWnd, dwStyleEx);

	//insert header;
	int colIndex = 0;
	m_lstStatis.InsertColumn(colIndex, "期号", LVCFMT_CENTER, 100);    //70
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);

	m_lstStatis.InsertColumn(colIndex, "销量(万)", LVCFMT_CENTER, 100);    //70
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstStatis.InsertColumn(colIndex, "奖金(万)", LVCFMT_LEFT, 100);    //170
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_DOUBLE);

	m_lstStatis.InsertColumn(colIndex, "号 码", LVCFMT_CENTER, 100);    //270
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	m_lstStatis.InsertColumn(colIndex, "号码和", LVCFMT_CENTER, 85);    //70
	m_lstStatis.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	//set sort type
	m_lstStatis.SetSortColumn(0);
}

void JQCDialog::ReloadStatisData() {
	CStlString strTextFile = Global::GetAppPath() + _T("jqc.txt");
	if (PathFileExists(strTextFile.c_str())) {
		m_lstStatis.DeleteAllItems();
		std::string filedate;
		Global::ReadFileData(strTextFile, filedate);
		std::vector<CStlString> arrLines;
		Global::DepartString(filedate, _T("\r\n"), arrLines);
		int iIndex = 0;
		for (const auto& line : arrLines) {
			int colIndex = 0;
			std::vector<CStlString> arrParts;
			Global::DepartString(line, _T(","), arrParts);
			if (arrParts.size() != 4) {
				continue;
			}
			if (arrParts[1].size() != 8) {
				continue;
			}
			iIndex = m_lstStatis.InsertItem(iIndex, arrParts[0].c_str());
			m_lstStatis.SetItemText(iIndex, ++colIndex, arrParts[3].c_str());
			m_lstStatis.SetItemText(iIndex, ++colIndex, arrParts[2].c_str());
			m_lstStatis.SetItemText(iIndex, ++colIndex, arrParts[1].c_str());
			CStlString& codes = arrParts[1];
			int sum = 0;
			for (int j = 0; j < 8; j++) {
				int code = (codes[j] - _T('0'));
				sum += code;
			}
			CStringATL strNum;
			sprintf(strNum.GetBuffer(255), "%u", sum);
			strNum.ReleaseBuffer();
			m_lstStatis.SetItemText(iIndex, ++colIndex, strNum);
		}
	}
}