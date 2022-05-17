#pragma once
#include "resource.h"
#include "JQCEngine.h"
#include "MiscHelper.h"
#include "AsyncFuncDispatch.h"
#include "http/HttpClientMgr.h"



class JQCDialog :
	public CAxDialogImpl<JQCDialog>,
	public CDialogResize<JQCDialog>,
	public CAsyncFuncDispatcher,
	public CIdleHandler,
	public CWinDataExchange<JQCDialog> {

public:
	JQCDialog();

public:
	virtual bool AddOneAsyncFunc(talk_base::IAsyncFuncCall* pAsyncFunc);
	virtual BOOL OnIdle();


public:
	enum { IDD = IDD_JQC, WM_ASYNC_DISPATCH = WM_APP + 0x360};
	typedef CDialogResize<JQCDialog> _BaseDlgResize;

	BEGIN_DDX_MAP(JQCDialog)
		DDX_CONTROL(IDC_STATIS_LIST, m_lstStatis)
		DDX_CONTROL(IDC_INFOS, m_stInfo)
	END_DDX_MAP()

	BEGIN_DLGRESIZE_MAP(JQCDialog)
		DLGRESIZE_CONTROL(IDC_STATIS_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y | DLSZ_REPAINT)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(JQCDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_INITMENU, OnInitMenu)
		MESSAGE_HANDLER(WM_ASYNC_DISPATCH, OnAsyncDispatch)

		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDM_ADDRECORD, OnAddRecord)
		COMMAND_ID_HANDLER(IDM_REFRESH, OnRefresh)

		CHAIN_MSG_MAP(_BaseDlgResize)
		CHAIN_MSG_MAP(CAxDialogImpl<JQCDialog>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnListLButtonDbclk)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListRButtonDown)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAddRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnAsyncDispatch(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	static void PopUp(const std::shared_ptr<SQLite::Database>& db);
	static void Destroy();
	static JQCDialog sInst;

private:
	static void OnHttpReturnGlobal(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);
	void OnHttpReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);
	CHttpRequestPtr CreateGetRequest(const std::string& url, const std::string& idprefix);
	std::shared_ptr<CHttpClientMgr> httpMgr_;

	

private:
	void InitControls();
	void ReloadStatisDataFromLocal(int from);
	void DoSaveResult(JQCEngine& engine);

private:
	CSortListViewCtrlEx<JQCDialog> m_lstStatis;
	CContainedWindowT<CStatic> m_stInfo;
	CStringATL m_strQH;

private:
	CStringATL m_strWorkDir;
	void CreateWorkDir();

private:
	void doNetUpdateResults();
	void OnNetUpdateResultsFirst(
		const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);
	void OnNetUpdateResultsSecond(
		const CHttpRequestPtr& request, const CHttpResponseDataPtr& response);

private:
	std::map<std::string, std::string> m_localids;
	int m_request_result_pos;
	int m_request_result_count;



};