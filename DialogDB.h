// DialogDB.h : CDialogDB ������

#pragma once

#include "resource.h"       // ������
#include <atlhost.h>


// CDialogDB
class CDialogDB :
    public CAxDialogImpl<CDialogDB>,
    public CWinDataExchange< CDialogDB >
    //public CDDX_Text_WTL<CDialogDB>

{
private:
    CDialogDB() {}

public:
	CDialogDB(std::shared_ptr<SQLite::Database> db) :
        m_lstQH(this, 1),
        m_edQH(this, 1),
        m_edCode(this, 1),
        m_edSales(this, 1) {

        m_strQH.Empty();
        m_strCode.Empty();
        m_strBonus.Empty();
        m_strPL.Empty();
		m_pDatabase = db;
    }

    ~CDialogDB() {
    }

    enum { IDD = IDD_DIALOG_DBDATA };

public:
    BEGIN_DDX_MAP(CDialogDB)
		DDX_TEXT(IDC_EDIT_QH, m_strQH)
		DDX_TEXT(IDC_EDIT_BONUS, m_strBonus)
		DDX_TEXT(IDC_EDIT_SALES, m_strSales)
		DDX_TEXT(IDC_EDIT_RESULT, m_strCode)
		DDX_TEXT(IDC_EDIT_PL, m_strPL)
		DDX_CONTROL(IDC_LIST_QH, m_lstQH)
		DDX_CONTROL(IDC_EDIT_QH, m_edQH)
		DDX_CONTROL(IDC_EDIT_SALES, m_edSales)
		DDX_CONTROL(IDC_EDIT_RESULT, m_edCode)
    END_DDX_MAP()


    BEGIN_MSG_MAP(CDialogDB)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedAdd)
    COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedExit)

    COMMAND_HANDLER(IDC_BUTTON_CLEAR, BN_CLICKED, OnClickedClear)
    COMMAND_HANDLER(IDC_BUTTON_DEL, BN_CLICKED, OnClickedDelete)
    COMMAND_HANDLER(ID_IMPORT_EXCEL_PL, BN_CLICKED, OnClickedExcel)

    COMMAND_HANDLER(IDC_LIST_QH, LBN_SELCHANGE, OnListQHSelChange)

    CHAIN_MSG_MAP(CAxDialogImpl<CDialogDB>)
    ALT_MSG_MAP(1)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnClickedAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnClickedExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnClickedClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnClickedDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

    LRESULT OnClickedExcel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnListQHSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
    void LoadData();

public:
    CStringATL m_strQH;
    CStringATL m_strCode;
    CStringATL m_strBonus;
    CStringATL m_strSales;
    CStringATL m_strPL;
	CStringATL m_strMatchs;
    CContainedWindowT<CListBox> m_lstQH;
    CContainedWindowT<CEdit> m_edQH;
    CContainedWindowT<CEdit> m_edCode;
    CContainedWindowT<CEdit> m_edSales;

private:
    void ClearDataShow();

private:
	std::shared_ptr<SQLite::Database> m_pDatabase;


};


