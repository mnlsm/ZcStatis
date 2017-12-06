#pragma once
#include "resource.h"

class CDialogTZ 
	: public CAxDialogImpl<CDialogTZ> {
public:
	enum { IDD = IDD_DIALOG_TZ };
	CDialogTZ(IDbSystem *pDbSystem, IDbDatabase *pDbDatabase, const CStlString& qh, int gambleID = -1);

	BEGIN_MSG_MAP(CDialogTZ)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_HANDLER(IDC_BUADD, BN_CLICKED, OnClickedBuAdd)
		COMMAND_HANDLER(IDC_BUCLEAR, BN_CLICKED, OnClickedBuClear)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedBuExit)
		COMMAND_RANGE_CODE_HANDLER(IDC_CORESULT1, IDC_CORESULT42, CBN_SELCHANGE, OnResultSelChange)
		/*
		COMMAND_ID_HANDLER(IDM_ADD_MATCH, OnAddMatch)
		COMMAND_ID_HANDLER(IDM_CUR_MATCH, OnCurMatch)
		COMMAND_ID_HANDLER(IDM_DEL_MATCH, OnDelMatch)
		COMMAND_ID_HANDLER(IDM_CHANGE_TEAMS, OnChangeTeam)
		COMMAND_ID_HANDLER(IDM_CHANGE_PARAMETER, OnWinResult)
		COMMAND_ID_HANDLER(IDM_ABOUT_THIS, OnAbout)
		COMMAND_ID_HANDLER(IDM_SYS_SETTING, OnSetting)
		COMMAND_ID_HANDLER(IDC_BUTJ, OnTJ)
		COMMAND_ID_HANDLER(IDM_WBW, OnWBW)
		COMMAND_ID_HANDLER(IDM_JQC, OnJQC)
		*/
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	

	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClickedBuClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnResultSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	/*
	LRESULT OnCurMatch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDelMatch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnChangeTeam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSetting(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnWinResult(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnTJ(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnWBW(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnJQC(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	*/


private:
	void initConctrol();
	BOOL ReadUserChoice(CStlString &strResults, CStringATL& strErrInfo);
	BOOL ReLoadDataToShow();
	void ClearUserChoice();
	BOOL DoUpdateDatabase(const CStlString &strResults);

private:

private:
	CStlString m_strQH;
	CStlString m_strPL;
	CStlString m_strMatchs;

	int m_GambleID;
	IDbDatabase *m_pDbDatabase;
	IDbSystem *m_pDbSystem;

private:
	CStatic m_wndQI;
	CBrush m_brush;
	
};
