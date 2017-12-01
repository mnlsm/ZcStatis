// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

typedef struct DataPLTag
{
	CStringWTL m_strPL1;
	CStringWTL m_strPL2;
	CStringWTL m_strPL3;
	CStringWTL m_strPLSum;
	CStringWTL m_strGvJ;
	CStringWTL m_strPlSCOPE;



}DataPL ;




class CMainDlg :
		public CAxDialogImpl<CMainDlg>, 
		public CDialogResize<CMainDlg>,
		public CWinDataExchange< CMainDlg >,
		public CMessageFilter, 
		public CIdleHandler
{

public:
	CMainDlg()
	{
		m_pDbSystem = NULL;
		m_pDbDatabase = NULL;

	}

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
		COMMAND_ID_HANDLER(IDM_REFRESH, OnRefresh)


		CHAIN_MSG_MAP(_BaseDlgResize)
		CHAIN_MSG_MAP(CAxDialogImpl<CMainDlg>)

		REFLECT_NOTIFICATIONS()

	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddRecord(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRefresh(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	



public:
	void InitializeStatisData();
	void ReloadStatisData();

	BOOL GetLianXu(const CStringWTL &strCode,long &nMaxSP,long &nMaxSF,long &nMaxPF);
	BOOL GetPL(const CStringWTL &strCode,const CStringWTL &strPL1,DataPL &dataPL);



private:
	CSortListViewCtrl m_lstStatis;

	
	CDoubleArray m_arrPLSCOPE;


private:
	IDbSystem *m_pDbSystem;
	IDbDatabase *m_pDbDatabase;


};
