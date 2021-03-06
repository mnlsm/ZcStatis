#pragma once
#include "resource.h"
#include "DanLueEngine.h"
#include "AsyncFuncDispatch.h"
#include "http/HttpClientMgr.h"

class DanLueDialog :
	public CAxDialogImpl<DanLueDialog>,
	public CIdleHandler,
	public CAsyncFuncDispatcher,
	public CWinDataExchange<DanLueDialog> {

private:
	DanLueDialog();
	static DanLueDialog sInst;
public:
	static void PopUp(const std::shared_ptr<SQLite::Database>& db);
	static void Destroy();

public:
	virtual bool AddOneAsyncFunc(talk_base::IAsyncFuncCall *pAsyncFunc);
	virtual BOOL OnIdle();

public:
	enum { IDD = IDD_DANLUE_LOGIN, WM_ASYNC_DISPATCH = WM_APP + 0x360 };

	BEGIN_DDX_MAP(DanLueDialog)
		DDX_CONTROL(IDC_MATCH_LIST, m_lstMatch)
		DDX_CONTROL(IDC_RESULT_LIST, m_lstResult)

		DDX_CONTROL(IDC_YZM, m_stYZM)
		DDX_CONTROL(IDC_BETAREA, m_stBetArea)
		DDX_CONTROL(IDC_STBETAREA1, m_stBetAreaTitle)

		DDX_CONTROL(IDC_SEP1, m_stSep1)
		DDX_CONTROL(IDC_SEP2, m_stSep2)
		DDX_CONTROL(IDC_STATIC_RESULT, m_stResult)

		DDX_CONTROL(IDC_BULOGIN, m_buLogin)
		DDX_CONTROL(IDC_BULOGOFF, m_buLogoff)
		DDX_CONTROL(IDC_BUCALC, m_buCalc)
		DDX_CONTROL(IDC_BUUPLOAD, m_buUpload)
		DDX_CONTROL(IDC_BUCLEARALL, m_buClear)
		DDX_CONTROL(IDC_BUREFRESH, m_buRefresh)
		DDX_CONTROL(IDC_COPY_CHOICES, m_buCopy)

	END_DDX_MAP()

	BEGIN_MSG_MAP(DanLueDialog)
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
		
		CHAIN_MSG_MAP(CAxDialogImpl<DanLueDialog>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		//MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnListLButtonDbclk)
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
	LRESULT OnInitMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAsyncDispatch(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnListLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

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

private:
	CSortListViewCtrlEx<DanLueDialog> m_lstMatch;
	CSortListViewCtrlEx<DanLueDialog> m_lstResult;
	CContainedWindowT<CStatic> m_stYZM;
	CContainedWindowT<CStatic> m_stBetArea;
	CContainedWindowT<CStatic> m_stBetAreaTitle;

	CContainedWindowT<CStatic> m_stSep1;
	CContainedWindowT<CStatic> m_stSep2;
	CContainedWindowT<CStatic> m_stResult;

	CContainedWindowT<CButton> m_buLogin;
	CContainedWindowT<CButton> m_buLogoff;
	CContainedWindowT<CButton> m_buRefresh;
	CContainedWindowT<CButton> m_buClear;
	CContainedWindowT<CButton> m_buCopy;
	CContainedWindowT<CButton> m_buCalc;
	CContainedWindowT<CButton> m_buUpload;
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
	void OnLogOffReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	int doInfo();
	void OnInfoReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	int doRcToken();
	void OnRcTokenReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	int doFriendList();
	void OnFriendListReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	int doLotteryCategories();
	void OnLotteryCategoriesReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	int doJcMatchList();
	void OnJcMatchListReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	int doHeMai();
	int doHeMaiImpl(const CStlStrxyArray& records, const CStlStrArray& matchIDs, bool last);
	int doHeMaiImpl_FuShi(const CStlStrxyArray& records, const CStlStrArray& matchIDs, 
		const std::vector<JcBetItemSource>& vecFixedSources, bool last);
	void OnHeMaiReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

	int doBiFen();
	void OnBiFenReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

public:
	struct JCMatchItem {
		std::string id;
		std::string match_category;
		std::string descrition;
		std::string start_time;
		std::string last_buy_time;
		int64 hand;
		std::string result;

		struct Subject {
			int64 tid;
			int64 betCode;
			double odds;
			std::string tip;
			bool checked;
			void calcTip(int hand);
			std::string betStr();
			int getPan(int hand) const;
			//std::string oddsStr();
		};
		std::vector<Subject> subjects;
		Subject* get_subject(int tid, int betCode);
		Subject* get_subject(int tid, const char* tip);
	};

private:
	std::string m_LoginToken;
	std::string m_NickName;
	std::string m_UserID;
	std::string m_RcUserID;
	std::string m_RcUserToken;
	int64 m_slwId;
	struct LotteryCategories {
		std::string description;
		std::string id;
		std::string label;
		std::string path;
	};
	std::vector<LotteryCategories> m_LotteryCategories;


	std::multimap<std::string, std::shared_ptr<JCMatchItem>> m_JCMatchItems;
	std::shared_ptr<JCMatchItem> m_CurrentMatchItem;
	JCMatchItem::Subject* get_subjects(const std::string& id, int tid, int code);

	BOOL GetItemFromDB(const std::string& id, JCMatchItem& item);
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
	std::shared_ptr<DanLueEngine> m_Engine;
};