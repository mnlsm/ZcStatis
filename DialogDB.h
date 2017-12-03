// DialogDB.h : CDialogDB 的声明

#pragma once

#include "resource.h"       // 主符号
#include <atlhost.h>


// CDialogDB
class CDialogDB :
    public CAxDialogImpl<CDialogDB>,
    public CWinDataExchange< CDialogDB >,
    public CDDX_Text_WTL<CDialogDB>

{
private:
    CDialogDB() {}

public:
    CDialogDB(IDbSystem *pDbSystem, IDbDatabase *pDbConnection) :
        m_lstQH(this, 1),
        m_edQH(this, 1),
        m_edCode(this, 1),
        m_edSales(this, 1) {

        m_strQH.Empty();
        m_strCode.Empty();
        m_strBonus.Empty();
        m_strPL.Empty();
        m_pDbDatabase = pDbConnection;
        m_pDbSystem = pDbSystem;
    }

    ~CDialogDB() {
    }

    enum { IDD = IDD_DIALOG_DBDATA };

public:
    BEGIN_DDX_MAP(CDialogDB)
//      DDX_CONTROL(IDC_STATIS_LIST,m_lstStatis)
    DDX_TEXT_WTLSTR(IDC_EDIT_QH, m_strQH)
    DDX_TEXT_WTLSTR(IDC_EDIT_BONUS, m_strBonus)
    DDX_TEXT_WTLSTR(IDC_EDIT_SALES, m_strSales)
    DDX_TEXT_WTLSTR(IDC_EDIT_RESULT, m_strCode)
    DDX_TEXT_WTLSTR(IDC_EDIT_PL, m_strPL)
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

// 处理程序原型：
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
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
    CStringWTL m_strQH;
    CStringWTL m_strCode;
    CStringWTL m_strBonus;
    CStringWTL m_strSales;
    CStringWTL m_strPL;
    CContainedWindowT<CListBox> m_lstQH;
    CContainedWindowT<CEdit> m_edQH;
    CContainedWindowT<CEdit> m_edCode;
    CContainedWindowT<CEdit> m_edSales;

private:
    void ClearDataShow();

private:
    IDbDatabase *m_pDbDatabase;
    IDbSystem *m_pDbSystem;


};


