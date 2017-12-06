#pragma once
#include "resource.h"

#define IDM_DELETE_FANGAN	WM_APP + 200
#define IDM_EDIT_FANGAN		WM_APP + 201
#define IDM_EDIT_SCRIPT		WM_APP + 202
#define IDM_DELETE_RESULT	WM_APP + 203

template <class T>
class CCheckListViewCtrlEx
	: public CCheckListViewCtrlImpl<CCheckListViewCtrlEx<T>>
{
public:
	CCheckListViewCtrlEx(T *owner) {
		m_owner = owner;
	}
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_CheckListViewCtrl_Ex"), GetWndClassName())
	BEGIN_MSG_MAP(CCheckListViewCtrlEx<T>)
		CHAIN_MSG_MAP_ALT_MEMBER((*m_owner), 1)
		CHAIN_MSG_MAP(CCheckListViewCtrlImpl<CCheckListViewCtrlEx<T>>)
	END_MSG_MAP()

private:
	T* m_owner;
};

class CDialogGambel : 
	public CAxDialogImpl<CDialogGambel>,
	public CWinDataExchange<CDialogGambel>
{
public:
	enum { IDD = IDD_DIALOG_GAMBEL };

public:
	CDialogGambel(IDbSystem *pDbSystem, IDbDatabase *pDbDatabase, const CStlString& qh);

public:
	BEGIN_DDX_MAP(CDialogGambel)
		//DDX_CONTROL(IDC_STATIS_LIST, m_lstStatis)
		DDX_CONTROL(IDC_GAMBEL_LIST, m_lstGambel)
	END_DDX_MAP()

public:
	BEGIN_MSG_MAP(CDialogGambel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

		COMMAND_HANDLER(IDC_BUADDDS, BN_CLICKED, OnClickedBuAddDanShi)
		COMMAND_HANDLER(IDC_BUADDFS, BN_CLICKED, OnClickedBuAddFuShi)
		COMMAND_HANDLER(IDC_BUEMPTY, BN_CLICKED, OnClickedBuEmpty)
		COMMAND_HANDLER(IDC_BUCALC, BN_CLICKED, OnClickedBuCalc)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedBuExit)

		COMMAND_RANGE_HANDLER(IDM_DELETE_FANGAN, IDM_DELETE_RESULT, OnClickedListMenu)

		CHAIN_MSG_MAP(CAxDialogImpl<CDialogGambel>)
		REFLECT_NOTIFICATIONS()
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnListLButtonDown)

	END_MSG_MAP()

public:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


	LRESULT OnClickedBuAddDanShi(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuAddFuShi(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuEmpty(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuCalc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnClickedListMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);


private:
	CCheckListViewCtrlEx<CDialogGambel> m_lstGambel;

private:
	void InitControls();
	void ReloadFangAnData();

private:
	CStlString m_strQH;
	IDbDatabase *m_pDbDatabase;
	IDbSystem *m_pDbSystem;

private:
	typedef struct DataRowTag {
		long m_nID;
		long m_nInUse;
		long m_nCodesType;
		CStringATL m_strCodes;
		CStringATL m_strPL;
		CStringATL m_strScript;
		CStringATL m_strResult;
	} DataRow;
	std::vector<DataRow> m_arrDbData;
};