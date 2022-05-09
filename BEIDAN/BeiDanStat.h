#pragma once
#include "resource.h"
//#include "AsyncFuncDispatch.h"
#include "http/HttpClientMgr.h"

class BeiDanStat : 
	public CAxDialogImpl<BeiDanStat>,
	public CWinDataExchange<BeiDanStat> {
public:
	BeiDanStat(const std::shared_ptr<SQLite::Database>& db);

public:
	enum { IDD = IDD_DANLUE_STAT};

public:
	BEGIN_DDX_MAP(BeiDanStat)
		DDX_CONTROL(IDC_RESULT_LIST, m_lstResult)
		DDX_CONTROL(IDC_COMBO_IDS, m_coIDs)
		DDX_CONTROL(IDC_CHECK_IDS, m_chkIDs)

		DDX_CONTROL(IDC_EDIT_CAT, m_edCat)
		DDX_CONTROL(IDC_CHECK_CAT, m_chkCat)

		DDX_CONTROL(IDC_EDIT_TEAM, m_edTeam)
		DDX_CONTROL(IDC_CHECK_CAT3, m_chkTeamHome)
		DDX_CONTROL(IDC_CHECK_CAT4, m_chkTeamAway)
	END_DDX_MAP()

	BEGIN_MSG_MAP(BeiDanStat)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDC_BUSEARCH, OnQuery)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_COMBO_IDS, CBN_SELCHANGE, OnSelIdsChanged)

		CHAIN_MSG_MAP(CAxDialogImpl<BeiDanStat>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListRButtonDown)
		ALT_MSG_MAP(100) //normal
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnQuery(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelIdsChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void InitControls();
	void InitData();
	void DoQuery();

	void DoMatchListMenuCommand(UINT cmd, UINT index);

private:
	CSortListViewCtrlEx<BeiDanStat> m_lstResult;
	CContainedWindowT<CButton> m_buSearch;
	
	CContainedWindowT<CComboBox> m_coIDs;
	CContainedWindowT<CButton> m_chkIDs;

	CContainedWindowT<CEdit> m_edCat;
	CContainedWindowT<CButton> m_chkCat;

	CContainedWindowT<CEdit> m_edTeam;
	CContainedWindowT<CButton> m_chkTeamHome;
	CContainedWindowT<CButton> m_chkTeamAway;

	CFont mResultListFont;
private:
	std::shared_ptr<SQLite::Database> m_pDatabase;


};