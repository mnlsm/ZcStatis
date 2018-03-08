#include "stdafx.h"
#include "DanLueStat.h"

DanLueStat::DanLueStat(const std::shared_ptr<SQLite::Database>& db) : 
	m_lstResult(this, 100),
	m_buSearch(this, 100),
	m_coIDs(this, 100),
	m_chkIDs(this, 100),
	m_edCat(this, 100),
	m_chkCat(this, 100)
{
	m_pDatabase = db;
}

LRESULT DanLueStat::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DoDataExchange(FALSE);
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	int gapX = rcDesktop.Width() / 50;
	int gapY = rcDesktop.Height() / 50;
	rcDesktop.DeflateRect(gapX, gapY, gapX, gapY);
	SetWindowPos(NULL, &rcDesktop, SWP_NOZORDER);
	InitControls();
	return TRUE;
}

LRESULT DanLueStat::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = CAxDialogImpl<DanLueStat>::OnDestroy(uMsg, wParam, lParam, bHandled);
	return lRet;
}

LRESULT DanLueStat::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
	pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left - 10;
	pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top - 10;
	return 1L;
}

void DanLueStat::InitControls() {
	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconSmall, FALSE);
	DWORD dwStyleEx = LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL;
	ListView_SetExtendedListViewStyle(m_lstResult.m_hWnd, dwStyleEx);

	CRect rcItem, rcc;
	GetClientRect(rcc);
	m_lstResult.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcc.right - 10;
	rcItem.bottom = rcc.bottom - 10;
	m_lstResult.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER | SWP_NOMOVE);
}

LRESULT DanLueStat::OnSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {

	return 1L;
}
