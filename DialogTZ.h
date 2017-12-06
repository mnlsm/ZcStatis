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

private:
	void initConctrol();
	BOOL ReadUserChoice(CStlString &strResults, CStringATL& strErrInfo);
	BOOL ReLoadDataToShow();
	void ClearUserChoice();
	BOOL DoUpdateDatabase(const CStlString &strResults);

private:
	CStlString m_strQH;
	CStlString m_strPL;
	CStlString m_strMatchs;

	int m_GambleID;
	IDbDatabase *m_pDbDatabase;
	IDbSystem *m_pDbSystem;

	BOOL m_bDataChanged;
private:
	CStatic m_wndQI;
	CBrush m_brush;
	
};
