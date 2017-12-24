#pragma once
#include "resource.h"
#include "MyPrintInfo.h"
#include "PrintWnd.h"

#define IDM_CACL_RESULT		WM_APP + 199
#define IDM_SAVE_RESULT		WM_APP + 207
#define IDM_DELETE_RESULT	WM_APP + 206

#define IDM_EDIT_CODES		WM_APP + 201
#define IDM_EDIT_SCRIPT		WM_APP + 202
#define IDM_COPY_SCRIPT		WM_APP + 204
#define IDM_DELETE_SCRIPT	WM_APP + 205

#define IDM_DELETE_ROW		WM_APP + 200
#define IDM_COPY_ROW		WM_APP + 210

class CDialogGambel :
	public CAxDialogImpl<CDialogGambel>,
	public CWinDataExchange<CDialogGambel>
{
public:
	enum { IDD = IDD_DIALOG_GAMBEL };

public:
	CDialogGambel(std::shared_ptr<SQLite::Database> db, const CStlString& qh);

public:
	BEGIN_DDX_MAP(CDialogGambel)
		//DDX_CONTROL(IDC_STATIS_LIST, m_lstStatis)
		DDX_CONTROL(IDC_GAMBEL_LIST, m_lstGambel)
		DDX_CONTROL(IDC_EDIT_OUTPUT, m_edOutput)
		DDX_CONTROL(IDC_EDIT_SEARCH, m_edSearch);
	END_DDX_MAP()

public:
	BEGIN_MSG_MAP(CDialogGambel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

		COMMAND_HANDLER(IDC_BUSEARCH, BN_CLICKED, OnClickedBuSearch)
		COMMAND_HANDLER(IDC_BUADDDS, BN_CLICKED, OnClickedBuAddDanShi)
		COMMAND_HANDLER(IDC_BUADDFS, BN_CLICKED, OnClickedBuAddFuShi)
		COMMAND_HANDLER(IDC_BUEMPTY, BN_CLICKED, OnClickedBuPreview)
		COMMAND_HANDLER(IDC_BUCALC, BN_CLICKED, OnClickedBuPrint)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedBuExit)
		NOTIFY_HANDLER(IDC_GAMBEL_LIST, LVN_ITEMCHANGED, OnListItemChanged)
		//COMMAND_RANGE_HANDLER(IDM_DELETE_FANGAN, IDM_DELETE_RESULT, OnClickedListMenu)

		CHAIN_MSG_MAP(CAxDialogImpl<CDialogGambel>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListRButtonDown)
		ALT_MSG_MAP(2)
		ALT_MSG_MAP(3)
	END_MSG_MAP()

public:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListItemChanged(int wParam, LPNMHDR lParam, BOOL& bHandled);;


	LRESULT OnClickedBuAddDanShi(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuAddFuShi(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuPreview(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuPrint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	CCheckListViewCtrlEx<CDialogGambel> m_lstGambel;
	CContainedWindowT<CEdit> m_edOutput;
	CContainedWindowT<CEdit> m_edSearch;

private:
	void InitControls();
	void ReloadFangAnData();
	void AppendOutputText(const TCHAR* text);
	void ClearOutputText();

private:
	CStlString m_strQH;
private:
	std::shared_ptr<SQLite::Database> m_pDatabase;

public:
	typedef struct DataRowTag {
		long m_nID;
		CStringATL m_strQH;
		long m_nInUse;
		long m_nCodesType;
		CStringATL m_strCodes;
		CStringATL m_strPL;
		CStringATL m_strMatchs;
		CStringATL m_strScript;
		CStringATL m_strResult;
	} DataRow;

private:
	std::vector<DataRow> m_arrDbData;
	CStlString m_SearchCodes;

private:
	void DoListMenuCommand(UINT cmd, UINT nItem);

	void DoCalcResult(const DataRow& data);
	void DoSaveResult(const DataRow& data);
	void DoDeleteResult(const DataRow& data);

	void DoEditCodes(const DataRow& data);
	void DoEditScript(const DataRow& data);
	void DoCopyScript(const DataRow& data);
	void DoDeleteScript(const DataRow& data);

	void DoRowInUse(UINT uItem, BOOL inuse);

	void DoCopyRow(const DataRow& data);
	void DoDeleteRow(const DataRow& data);


private:
	std::unique_ptr<CMyPrintInfo> m_pPrintInfo;
	std::unique_ptr<CPrinter> m_pPrinter;
	std::unique_ptr<CPrintWnd> m_wndPreview;
};