#pragma once
#include "resource.h"
#include "DanLueEngine.h"

class DanLueDialog :
	public CAxDialogImpl<DanLueDialog>,
	public CWinDataExchange<DanLueDialog> {

public:
	DanLueDialog();

public:
	enum { IDD = IDD_DANLUE_LOGIN };
	typedef CDialogResize<DanLueDialog> _BaseDlgResize;

	BEGIN_DDX_MAP(DanLueDialog)
		DDX_CONTROL(IDC_MATCH_LIST, m_lstMatch)
		DDX_CONTROL(IDC_RESULT_LIST, m_lstResult)

		DDX_CONTROL(IDC_YZM, m_stYZM)
		DDX_CONTROL(IDC_BETAREA, m_stBetArea)

		DDX_CONTROL(IDC_BULOGIN, m_buLogin)
		DDX_CONTROL(IDC_BULOGOFF, m_buLogoff)
		DDX_CONTROL(IDC_BUCALC, m_buCalc)
		DDX_CONTROL(IDC_BUUPLOAD, m_buUpload)
		DDX_CONTROL(IDC_BUCLEARALL, m_buClear)
		DDX_CONTROL(IDC_BUREFRESH, m_buRefresh)

	END_DDX_MAP()

	BEGIN_MSG_MAP(DanLueDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_INITMENU, OnInitMenu)

		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDM_ADDRECORD, OnAddRecord)
		COMMAND_ID_HANDLER(IDM_REFRESH, OnRefresh)

		CHAIN_MSG_MAP(CAxDialogImpl<DanLueDialog>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnListLButtonDbclk)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListRButtonDown)
		ALT_MSG_MAP(2) //result
		ALT_MSG_MAP(3) //yzm
		ALT_MSG_MAP(4) //bet area
		ALT_MSG_MAP(100) //normal
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
	CSortListViewCtrlEx<DanLueDialog> m_lstMatch;
	CSortListViewCtrlEx<DanLueDialog> m_lstResult;
	CContainedWindowT<CStatic> m_stYZM;
	CContainedWindowT<CStatic> m_stBetArea;


	CContainedWindowT<CButton> m_buLogin;
	CContainedWindowT<CButton> m_buLogoff;
	CContainedWindowT<CButton> m_buRefresh;
	CContainedWindowT<CButton> m_buClear;
	CContainedWindowT<CButton> m_buCalc;
	CContainedWindowT<CButton> m_buUpload;



	CStringATL m_strQH;

private:
	CStringATL m_strWorkDir;
	void CreateWorkDir();


};