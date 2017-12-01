// DialogDB.cpp : CDialogDB 的实现

#include "stdafx.h"
#include "DialogDB.h"


void CDialogDB::LoadData() {
    m_lstQH.ResetContent();

    CStringWTL strSQL = _T("SELECT ID FROM PLDATA ORDER BY ID DESC");
    IDbRecordset *pRS = m_pDbSystem->CreateRecordset(m_pDbDatabase);
    pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY);

    while(!pRS->IsEOF()) {
        CStringWTL strID;
        pRS->GetField(0, strID);
        strID.TrimLeft();
        strID.TrimRight();

        if(!strID.IsEmpty()) {
            m_lstQH.AddString(strID);
        }

        pRS->MoveNext();
    }

    pRS->Close();
    delete pRS;
}


// CDialogDB
LRESULT CDialogDB::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CAxDialogImpl<CDialogDB>::OnInitDialog(uMsg, wParam, lParam, bHandled);
    DoDataExchange(FALSE);

    //GetDlgItem(IDC_EDIT_QH)->
    m_edQH.SetLimitText(5);
    m_edCode.SetLimitText(14);


    LoadData();

    CenterWindow(::GetDesktopWindow());
    return 1;  // 使系统设置焦点
}

LRESULT CDialogDB::OnClickedAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    DoDataExchange(TRUE);


    int nLenQH = m_strQH.GetLength();
    if(nLenQH != 5) {
        MessageBox("期号输入错误，请检查！", "错误", MB_OK);
        return 1L;
    }

    for(int i = 0 ; i < nLenQH ; i++) {
        if(m_strQH[i] < '0' || m_strQH[i] > '9') {
            MessageBox("期号输入错误，请检查！", "错误", MB_OK);
            return 1L;
        }

    }


    int nLenCode = m_strCode.GetLength();
    if(nLenCode != 14) {
        MessageBox(_T("结果输入错误，请检查！"), _T("错误"), MB_OK);
        return 1L;
    }
    for(int i = 0 ; i < nLenCode ; i++) {
        if(m_strCode[i] != '0' && m_strCode[i] != '1' && m_strCode[i] != '3') {
            MessageBox("结果输入错误，请检查！", "错误", MB_OK);
            return 1L;
        }

    }





    //CStringWTL strSQL;
    //strSQL.Format( _T(" delete from PLDATA where ID = '%s' "),m_strQH);
    //m_pDbDatabase->ExecuteSQL(m_strQH);


    CStringWTL strSQL =  _T(" delete from PLDATA where ID = ? ");
    IDbCommand *pCmd1 = m_pDbSystem->CreateCommand(m_pDbDatabase);
    pCmd1->Create(strSQL);
    pCmd1->SetParam(0, m_strQH);
    pCmd1->Execute(NULL);

    pCmd1->Close();
    delete pCmd1;



    float fBonus = atof(m_strBonus);
    strSQL = _T(" INSERT INTO PLDATA (ID, BONUS,RESULT,PLDATA) values(?,?,?,?)");
    IDbCommand *pCmd = m_pDbSystem->CreateCommand(m_pDbDatabase);
    pCmd->Create(strSQL);
    pCmd->SetParam(0, m_strQH);
    pCmd->SetParam(1, &fBonus);
    pCmd->SetParam(2, m_strCode);
    pCmd->SetParam(3, m_strPL);

    pCmd->Execute(NULL);


    pCmd->Close();


    delete pCmd;


    if(m_lstQH.FindString(0, m_strQH) < 0)
        m_lstQH.AddString(m_strQH);

    return 0;
}

LRESULT CDialogDB::OnClickedExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    EndDialog(wID);
    return 0;
}


LRESULT CDialogDB::OnClickedClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {

    m_strQH.Empty();
    m_strCode.Empty();
    m_strBonus.Empty();
    m_strPL.Empty();


    DoDataExchange(FALSE);

    return 0;
}

LRESULT CDialogDB::OnClickedDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    int nSel = m_lstQH.GetCurSel();
    if(nSel < 0) return 1L;

    CStringWTL strID;
    m_lstQH.GetText(nSel, strID.GetBuffer(1024));
    strID.ReleaseBuffer();

    CStringWTL strSQL =  _T(" delete from PLDATA where ID = ? ");
    IDbCommand *pCmd = m_pDbSystem->CreateCommand(m_pDbDatabase);
    pCmd->Create(strSQL);
    pCmd->SetParam(0, strID);
    pCmd->Execute(NULL);

    pCmd->Close();
    delete pCmd;

    m_lstQH.DeleteString(nSel);
    return 0;
}

LRESULT CDialogDB::OnListQHSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    int nSel = m_lstQH.GetCurSel();
    if(nSel < 0) return 1L;

    CStringWTL strID;
    m_lstQH.GetText(nSel, strID.GetBuffer(1024));
    strID.ReleaseBuffer();


    CStringWTL strSQL;
    strSQL.Format(_T(" select ID,BONUS,RESULT,PLDATA from PLDATA where ID = '%s' "), strID);
    IDbRecordset *pRS = m_pDbSystem->CreateRecordset(m_pDbDatabase);
    pRS->Open(strSQL, DB_OPEN_TYPE_FORWARD_ONLY);

    if(!pRS->IsEOF()) {
        float fBonus = 0;
        pRS->GetField(0, m_strQH);
        pRS->GetField(1, fBonus);
        //m_strBonus.Format( "%.2f",fBonus );
        sprintf(m_strBonus.GetBuffer(255), "%.2f", fBonus);
        m_strBonus.ReleaseBuffer();
        pRS->GetField(2, m_strCode);
        pRS->GetField(3, m_strPL);
    }

    pRS->Close();
    delete pRS;

    DoDataExchange(FALSE);

    return 0;

}
