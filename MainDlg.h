#pragma once

#define IDM_EDIT_FANGANS   WM_APP + 300
#define IDM_DELETE_PLDATA   WM_APP + 301

class CMainDlg :
		public CAxDialogImpl<CMainDlg>, 
		public CDialogResize<CMainDlg>,
		public CWinDataExchange< CMainDlg >,
		public CMessageFilter, 
		public CIdleHandler
{

public:
	CMainDlg();

public:
	BEGIN_DDX_MAP(CMainDlg)
		DDX_CONTROL(IDC_STATIS_LIST,m_lstStatis)
	END_DDX_MAP()

	typedef CDialogResize<CMainDlg> _BaseDlgResize;

	BEGIN_DLGRESIZE_MAP(CMainDlg)
		DLGRESIZE_CONTROL(IDC_STATIS_LIST,DLSZ_SIZE_X|DLSZ_SIZE_Y|DLSZ_REPAINT)
	END_DLGRESIZE_MAP()


public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();


	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)

		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDM_ADDRECORD, OnAddRecord)
		COMMAND_ID_HANDLER(IDM_JQC, OnMenuJQC)
		COMMAND_ID_HANDLER(IDM_REFRESH, OnRefresh)
		COMMAND_ID_HANDLER(IDM_OKOOO, OnOkooo)
		COMMAND_ID_HANDLER(IDM_BEIDAN, OnBeiDan)
		COMMAND_ID_HANDLER(IDM_ZUCAI, OnZuCai)



		CHAIN_MSG_MAP(_BaseDlgResize)
		CHAIN_MSG_MAP(CAxDialogImpl<CMainDlg>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnListLButtonDbclk)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListRButtonDown)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddRecord(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRefresh(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMenuJQC(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOkooo(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBeiDan(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnZuCai(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	
private:
	typedef struct DataRowTag {
		CStringATL m_strCodeZongShu;
		CStringATL m_strCodeLianOne;
		CStringATL m_strCodeLianTwo;
		CStringATL m_strCodeDuanDian;
		CStringATL m_strPL1;
		CStringATL m_strPL2;
		CStringATL m_strPL3;
		CStringATL m_strPLSum;
		CStringATL m_strGvJ;
		CStringATL m_strPlSCOPE;
		CStringATL m_strPLAvgCount;
		CStringATL m_strPLAvg;
	} DataRow;

private:
	void InitControls();
	void InitializeStatisData();
	void ReloadStatisData();
	BOOL GetPL(const CStringATL &strCode, const CStringATL &strPL1, DataRow &dataPL);

private:
	void DoListMenuCommand(UINT cmd, UINT nItem);

private:
	CSortListViewCtrlEx<CMainDlg> m_lstStatis;
	CDoubleArray m_arrPLSCOPE;

private:
	std::shared_ptr<SQLite::Database> m_pDatabase;

};
