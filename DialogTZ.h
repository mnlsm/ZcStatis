#pragma once
#include "resource.h"
#include "ButtonMenu.h"

#define IDM_RECOMMEND_ONE WM_APP + 9909
#define IDM_RECOMMEND_TWO IDM_RECOMMEND_ONE + 1
#define IDM_RECOMMEND_THREE IDM_RECOMMEND_TWO + 1
#define IDM_RECOMMEND_RECOMMEND_TWO IDM_RECOMMEND_THREE + 1





class CDialogTZ 
	: public CAxDialogImpl<CDialogTZ> {
public:
	enum { IDD = IDD_DIALOG_TZ };
	CDialogTZ(const std::shared_ptr<SQLite::Database>& db, const CStlString& strWorkDir, 
		const CStlString& qh, int gambleID = -1);

	BEGIN_MSG_MAP(CDialogTZ)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_HANDLER(IDC_BUADD, BN_CLICKED, OnClickedBuAdd)
		COMMAND_HANDLER(IDC_BUCLEAR, BN_CLICKED, OnClickedBuClear)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedBuExit)
		COMMAND_RANGE_CODE_HANDLER(IDC_CORESULT1, IDC_CORESULT42, CBN_SELCHANGE, OnResultSelChange)
		COMMAND_RANGE_HANDLER(IDM_RECOMMEND_ONE, IDM_RECOMMEND_RECOMMEND_TWO, OnRecommendMenu)
		CHAIN_MSG_MAP(CAxDialogImpl<CDialogTZ>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	BOOL IsDbDataChanged();

public:
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClickedBuClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnResultSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRecommendMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	void initConctrols();
	void initComboxes();
	BOOL ReadUserChoice(CStlString &strResults, CStringATL& strErrInfo);
	BOOL ReLoadDataToShow(BOOL first);
	void ClearUserChoice();
	BOOL DoUpdateDatabase(const CStlString &strResults);

	void DoRecommendTwoChoice();

private:
	CStlString m_strQH;
	CStlString m_strPL;
	CStlString m_strMatchs;

	int m_GambleID;
	std::shared_ptr<SQLite::Database> m_pDatabase;
	BOOL m_bDataChanged;

	CStlString m_strWorkDir;
	CStlString m_strRecommDir;

private:
	CStatic m_wndQI;
	CBrush m_brush;
	CButtonMenu m_MenuButton;


	
};
