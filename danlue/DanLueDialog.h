#pragma once
#include "resource.h"
#include "DanLueEngine.h"
#include "AsyncFuncDispatch.h"
#include "http/HttpClientMgr.h"

class DanLueDialog :
	public CAxDialogImpl<DanLueDialog>,
	public CAsyncFuncDispatcher,
	public CWinDataExchange<DanLueDialog> {

private:
	DanLueDialog();
	static DanLueDialog sInst;
public:
	static void PopUp();
	static void Destroy();

public:
	virtual bool AddOneAsyncFunc(talk_base::IAsyncFuncCall *pAsyncFunc);

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
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ASYNC_DISPATCH, OnAsyncDispatch)

		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)

		COMMAND_ID_HANDLER(IDC_BULOGIN, OnLoginIn)
		COMMAND_ID_HANDLER(IDC_BULOGOFF, OnLoginOff)
		COMMAND_ID_HANDLER(IDC_BUCALC, OnCalc)
		COMMAND_ID_HANDLER(IDC_BUUPLOAD, OnUpload)
		COMMAND_ID_HANDLER(IDC_BUCLEARALL, OnClearAll)
		COMMAND_ID_HANDLER(IDC_BUREFRESH, OnRefresh)


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
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAsyncDispatch(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	


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

private:
	void InitControls();
	void ReloadStatisData();
	void DoSaveResult(DanLueEngine& engine);

private:
	CSortListViewCtrlEx<DanLueDialog> m_lstMatch;
	CSortListViewCtrlEx<DanLueDialog> m_lstResult;
	CContainedWindowT<CStatic> m_stYZM;
	CContainedWindowT<CStatic> m_stBetArea;
	CContainedWindowT<CStatic> m_stBetAreaTitle;

	CContainedWindowT<CStatic> m_stSep1;
	CContainedWindowT<CStatic> m_stSep2;


	CContainedWindowT<CButton> m_buLogin;
	CContainedWindowT<CButton> m_buLogoff;
	CContainedWindowT<CButton> m_buRefresh;
	CContainedWindowT<CButton> m_buClear;
	CContainedWindowT<CButton> m_buCalc;
	CContainedWindowT<CButton> m_buUpload;
	
	CStringATL m_strQH;

private:
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

	struct JCMatchItem {
		std::string id;
		std::string descrition;
		std::string start_time;
		std::string last_buy_time;
		int64 hand;

		struct Subject {
			int64 tid;
			int64 betCode;
			double odds;
			std::string tip;
			bool checked;
		};
		std::vector<Subject> subjects;
	};
	std::multimap<std::string, JCMatchItem> m_JCMatchItems;
};