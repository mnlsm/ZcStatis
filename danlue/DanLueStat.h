#pragma once
#include "resource.h"
//#include "AsyncFuncDispatch.h"
#include "http/HttpClientMgr.h"

class DanLueStat : 
	public CAxDialogImpl<DanLueStat>,
	public CWinDataExchange<DanLueStat> {
public:
	DanLueStat(const std::shared_ptr<SQLite::Database>& db);

public:
	enum { IDD = IDD_DANLUE_STAT};

public:
	BEGIN_DDX_MAP(DanLueStat)
		DDX_CONTROL(IDC_RESULT_LIST, m_lstResult)
		DDX_CONTROL(IDC_COMBO_IDS, m_coIDs)
		DDX_CONTROL(IDC_CHECK_IDS, m_chkIDs)

		DDX_CONTROL(IDC_EDIT_CAT, m_edCat)
		DDX_CONTROL(IDC_CHECK_CAT, m_chkCat)
		
	END_DDX_MAP()

	BEGIN_MSG_MAP(DanLueStat)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CAxDialogImpl<DanLueStat>)
		REFLECT_NOTIFICATIONS()

		ALT_MSG_MAP(100) //normal
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);


private:
	void InitControls();

private:
	CSortListViewCtrlEx<DanLueStat> m_lstResult;
	CContainedWindowT<CButton> m_buSearch;
	
	CContainedWindowT<CComboBox> m_coIDs;
	CContainedWindowT<CButton> m_chkIDs;

	CContainedWindowT<CEdit> m_edCat;
	CContainedWindowT<CButton> m_chkCat;


private:
	std::shared_ptr<SQLite::Database> m_pDatabase;


};