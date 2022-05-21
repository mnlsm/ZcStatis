#pragma once

#include "resource.h"
#include "BeiDanEngine.h"
#include "AsyncFuncDispatch.h"
#include "http/HttpClientMgr.h"
#include "browser/WebBrowser.h"

class BeiDanDialog :
	public CAxDialogImpl<BeiDanDialog>,
	public CIdleHandler,
	public CAsyncFuncDispatcher,
	public CWinDataExchange<BeiDanDialog>,
	public CDialogResize<BeiDanDialog>,
	public IWebBrowserCallback {

private:
	BeiDanDialog();
	static BeiDanDialog sInst;
public:
	static void PopUp(const std::shared_ptr<SQLite::Database>& db);
	static void Destroy();

public:
	virtual bool AddOneAsyncFunc(talk_base::IAsyncFuncCall* pAsyncFunc);
	virtual BOOL OnIdle();

public:
	enum { IDD = IDD_OKOOO_BD, WM_ASYNC_DISPATCH = WM_APP + 0x360 };

	typedef CDialogResize<BeiDanDialog> _BaseDlgResize;
	BEGIN_DLGRESIZE_MAP(BeiDanDialog)
		DLGRESIZE_CONTROL(IDC_BETAREA, DLSZ_MOVE_X | DLSZ_REPAINT)
		DLGRESIZE_CONTROL(IDC_MATCH_LIST, DLSZ_SIZE_X | DLSZ_REPAINT)
		DLGRESIZE_CONTROL(IDC_RESULT_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y | DLSZ_REPAINT)
		DLGRESIZE_CONTROL(IDC_SEP1, DLSZ_SIZE_X | DLSZ_REPAINT)
		DLGRESIZE_CONTROL(IDC_SEP2, DLSZ_SIZE_X | DLSZ_REPAINT)
		DLGRESIZE_CONTROL(IDC_STBETAREA1, DLSZ_MOVE_X | DLSZ_REPAINT)
	END_DLGRESIZE_MAP()


	BEGIN_DDX_MAP(BeiDanDialog)
		DDX_CONTROL(IDC_MATCH_LIST, m_lstMatch)
		DDX_CONTROL(IDC_RESULT_LIST, m_lstResult)

		DDX_CONTROL(IDC_YZM, m_stYZM)
		DDX_CONTROL(IDC_BETAREA, m_stBetArea)
		DDX_CONTROL(IDC_STBETAREA1, m_stBetAreaTitle)

		DDX_CONTROL(IDC_SEP1, m_stSep1)
		DDX_CONTROL(IDC_SEP2, m_stSep2)
		DDX_CONTROL(IDC_STATIC_RESULT, m_stResult)
		DDX_CONTROL(IDC_BD_PROGRESS, m_stProgress)

		DDX_CONTROL(IDC_BULOGIN, m_buLogin)
		DDX_CONTROL(IDC_BULOGOFF, m_buLogoff)
		DDX_CONTROL(IDC_BUCALC, m_buCalc)
		DDX_CONTROL(IDC_BUUPLOAD, m_buUpload)
		DDX_CONTROL(IDC_BUCLEARALL, m_buClear)
		DDX_CONTROL(IDC_BUREFRESH, m_buRefresh)
		DDX_CONTROL(IDC_COPY_CHOICES, m_buCopy)
		DDX_CONTROL(IDC_EXTRACT_LUA, m_buExtractLua)
		DDX_CONTROL(IDC_MATCH_FILTER, m_coMatchFilter)
		
		
	END_DDX_MAP()

	BEGIN_MSG_MAP(BeiDanDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ASYNC_DISPATCH, OnAsyncDispatch)

		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)

		COMMAND_ID_HANDLER(IDC_BULOGIN, OnLoginIn)
		COMMAND_ID_HANDLER(IDC_BULOGOFF, OnLoginOff)
		COMMAND_ID_HANDLER(IDC_BUCALC, OnCalc)
		COMMAND_ID_HANDLER(IDC_BUUPLOAD, OnUpload)
		COMMAND_ID_HANDLER(IDC_BUCLEARALL, OnClearAll)
		COMMAND_ID_HANDLER(IDC_BUREFRESH, OnRefresh)
		COMMAND_ID_HANDLER(IDC_COPY_CHOICES, OnCopyChoices)
		COMMAND_ID_HANDLER(IDC_BUBIFEN, OnRefreshBiFen)
		COMMAND_ID_HANDLER(IDC_EXTRACT_LUA, OnExtractLua)
		COMMAND_HANDLER(IDC_MATCH_FILTER, CBN_SELCHANGE, OnMatchFilterChange)

		CHAIN_MSG_MAP(_BaseDlgResize)
		CHAIN_MSG_MAP(CAxDialogImpl<BeiDanDialog>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnListLButtonDbclk)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnListLButtonDown)
		ALT_MSG_MAP(2) //result
		ALT_MSG_MAP(3) //yzm
		ALT_MSG_MAP(4) //bet area
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnBetAreaLButtonDown)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnBetAreaEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnBetAreaPaint)

		ALT_MSG_MAP(100) //normal
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAsyncDispatch(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnListLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//LRESULT OnAddRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnLoginIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnLoginOff(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCalc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnUpload(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClearAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCopyChoices(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRefreshBiFen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnExtractLua(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMatchFilterChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	

	
	LRESULT OnBetAreaLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBetAreaEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBetAreaPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void DrawBetAreaTitle(CDCHandle dc, const CRect& rcc, int& yTop);
	void DrawSPF(CDCHandle dc, const CRect& rcc, int& yTop);
	void DrawJQZS(CDCHandle dc, const CRect& rcc, int& yTop);
	void DrawBQC(CDCHandle dc, const CRect& rcc, int& yTop);
	void DrawBF(CDCHandle dc, const CRect& rcc, int& yTop);

private:
	void InitControls();
	void ReloadMatchListData();

	void DoMatchListMenuCommand(UINT cmd, UINT index);
	void DoRefreshMatchListResults();
	CStringATL DoRefreshResultListResults(std::string& buyLines, std::string& checkLines);
	void DoRefreshBetArea();

	void DoReloadBackupResult();
	void GetBuyLinesData(std::string& abuyLines);

	CStringATL CopyChoicesText(int& choice_count);

private:
	CSortListViewCtrlEx<BeiDanDialog> m_lstMatch;
	CSortListViewCtrlEx<BeiDanDialog> m_lstResult;
	CContainedWindowT<CStatic> m_stYZM;
	CContainedWindowT<CStatic> m_stBetArea;
	CContainedWindowT<CStatic> m_stBetAreaTitle;

	CContainedWindowT<CStatic> m_stSep1;
	CContainedWindowT<CStatic> m_stSep2;
	CContainedWindowT<CStatic> m_stResult;
	CContainedWindowT<CStatic> m_stProgress;

	CContainedWindowT<CButton> m_buLogin;
	CContainedWindowT<CButton> m_buLogoff;
	CContainedWindowT<CButton> m_buRefresh;
	CContainedWindowT<CButton> m_buClear;
	CContainedWindowT<CButton> m_buCopy;
	CContainedWindowT<CButton> m_buCalc;
	CContainedWindowT<CButton> m_buUpload;
	CContainedWindowT<CButton> m_buExtractLua;
	CContainedWindowT<CComboBox> m_coMatchFilter;

	CFont mMatchListFont;
	CFont mBetAreaFont;


private:
	CStringATL m_strQH;
	CStringATL m_strRootDir;
	CStringATL m_strWorkDir;
	void CreateWorkDir();

	//network
private:
	static void OnHttpReturnGlobal(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);
	void OnHttpReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);
	CHttpRequestPtr CreatePostRequest(const std::string& url, const std::string& idprefix, const std::string& data);
	CHttpRequestPtr CreateGetRequest(const std::string& url, const std::string& idprefix);
	std::shared_ptr<CHttpClientMgr> httpMgr_;

private:
	int doLogin();
	void OnLoginReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	int doLogOff();

	int doJcMatchList();
	void OnJcMatchListReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	void OnBeiDanWDLReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);
	void OnBeiDanDSReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);
	void OnBeiDanJQReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);



	int doBiFen();
	void OnBiFenReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

private:
	DWORD m_compose_timestamp;
	CWaitCursor m_waitCursor;
	int m_pending_request;
	std::map<std::string, std::shared_ptr<JCMatchItem>> m_order_items;
	std::multimap<std::string, std::shared_ptr<JCMatchItem>> m_JCMatchItems;
	std::shared_ptr<JCMatchItem> m_CurrentMatchItem;
	JCMatchItem::Subject* get_subjects(const std::string& id, int tid, int code);

	BOOL GetItemFromDB(const JCMatchItem& new_item, JCMatchItem& item);

	BOOL InsertItemToDB(const JCMatchItem& item);
	BOOL UpdateItemResultToDB(const std::string& id, const std::string& result);


private:
	struct DrawBetItem {
		int64 tid;
		int64 betCode;
		CRect rect;
	};
	DrawBetItem m_TitleDrawBetItem;
	DrawBetItem m_SPFDrawBetItems[6];
	DrawBetItem m_JQZSDrawBetItems[8];
	DrawBetItem m_BQCDrawBetItems[9];
	DrawBetItem m_BFDrawBetItems[31];
	bool m_FirstDrawBetArea;

private:
	std::shared_ptr<SQLite::Database> m_pDatabase;

private:
	std::shared_ptr<BeiDanEngine> m_Engine;

private:
	std::map<std::string, std::shared_ptr<WebBrowser>> m_Browsers;
	std::vector<std::shared_ptr<WebBrowser>> m_delayDeleteBrowsers;

	void ShowMatchWebBrowser(const CStringATL& title);
	void CloseMatchWebBrowsers();
public:
	virtual void onWebBrowserClose(const std::string& url);

};