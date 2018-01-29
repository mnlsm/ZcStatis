#pragma once
#include "resource.h"
#include "DanLueEngine.h"

class DanLueDialog :
	public CAxDialogImpl<DanLueDialog>,
	public CDialogResize<DanLueDialog>,
	public CWinDataExchange<DanLueDialog> {

public:
	DanLueDialog();

public:
	enum { IDD = IDD_DANLUE_LOGIN };
	typedef CDialogResize<DanLueDialog> _BaseDlgResize;

	BEGIN_DDX_MAP(DanLueDialog)
		DDX_CONTROL(IDC_STATIS_LIST, m_lstStatis)
	END_DDX_MAP()

	BEGIN_DLGRESIZE_MAP(DanLueDialog)
		DLGRESIZE_CONTROL(IDC_STATIS_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y | DLSZ_REPAINT)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(DanLueDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_INITMENU, OnInitMenu)

		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDM_ADDRECORD, OnAddRecord)
		COMMAND_ID_HANDLER(IDM_REFRESH, OnRefresh)

		CHAIN_MSG_MAP(_BaseDlgResize)
		CHAIN_MSG_MAP(CAxDialogImpl<DanLueDialog>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnListLButtonDbclk)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListRButtonDown)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAddRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	void InitControls();
	void ReloadStatisData();
	void DoSaveResult(DanLueEngine& engine);

private:
	CSortListViewCtrlEx<DanLueDialog> m_lstStatis;
	CStringATL m_strQH;

private:
	CStringATL m_strWorkDir;
	void CreateWorkDir();


};