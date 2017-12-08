// MyPrintInfo.cpp: implementation of the CMyPrintInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyPrintInfo.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyPrintInfo::CMyPrintInfo() {
    int nFontSize = -9;
    CStringATL strFontName = "Times New Roman";
    HFONT hF = m_PrinterFont.CreateFont(nFontSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                                        OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                                        DEFAULT_PITCH | FF_DONTCARE, strFontName);

}
CMyPrintInfo::~CMyPrintInfo() {
    m_PrinterFont.DeleteObject();
}
void CMyPrintInfo::BeginPrintJob(HDC hDC) {
    CDCHandle dc(hDC);
    if(dc.IsNull()) return;
    HFONT OldFont = dc.SelectFont(m_PrinterFont);
    dc.GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"), 52, &m_CharSize);
    m_CharSize.cx /= 52;
    int nMargins = (LEFT_MARGIN + RIGHT_MARGIN) * m_CharSize.cx;
    m_PaperSize = CSize(dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));
    m_LogicalPageSize.cx = PAGEWIDTH * m_CharSize.cx + nMargins;
    m_LogicalPageSize.cy = (m_LogicalPageSize.cx * m_PaperSize.cy) / m_PaperSize.cx;
    m_nPageHeight = m_LogicalPageSize.cy - 0/*GetFixedRowHeight()*/
                    - (HEADER_HEIGHT + FOOTER_HEIGHT + 2 * GAP) * m_CharSize.cy;
    dc.SelectFont(OldFont);

}
void CMyPrintInfo::EndPrintJob(HDC hDC, bool bAborted) {
//  m_PrinterFont.DeleteObject();
}
bool CMyPrintInfo::PrintPage(UINT nPage, HDC hDC) {
    if(!IsValidPage(nPage)) return FALSE;
    CStlStrArray arrData = GetCurPageData(nPage);
    if(arrData.size() <= 0) return FALSE;

    int nMargins = (LEFT_MARGIN + RIGHT_MARGIN) * m_CharSize.cx;
    int nPageWidth = PAGEWIDTH * m_CharSize.cx;

    CDCHandle dc(hDC);
    HFONT OldFont = dc.SelectFont(m_PrinterFont);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(0, 0, 0));
//  dc.SetTextColor(RGB(1,1,255));



    dc.SetMapMode(MM_ANISOTROPIC);
    dc.SetWindowExt(m_LogicalPageSize, &CSize());
    dc.SetViewportExt(m_PaperSize);
    dc.SetWindowOrg(-LEFT_MARGIN * m_CharSize.cx, 0);

    m_rectDraw.top    = 0;
    m_rectDraw.left   = 0;
    m_rectDraw.right  = m_LogicalPageSize.cx - (LEFT_MARGIN + RIGHT_MARGIN) * m_CharSize.cx;
    m_rectDraw.bottom = HEADER_HEIGHT * m_CharSize.cy;
    PrintHeader(nPage, arrData.size(), hDC);

    dc.OffsetWindowOrg(0, -HEADER_HEIGHT * m_CharSize.cy);
    // Gap between header and column headings
    dc.OffsetWindowOrg(0, -GAP * m_CharSize.cy);

    int iCurRow = 0;
    CRect rect;
    rect.bottom = -1;
    int iMaxRow = fmod(arrData.size(), 3) == 0 ? arrData.size() / 3 : arrData.size() + 1;
    while(iCurRow < iMaxRow) {
        rect.top = rect.bottom + 1;
        rect.bottom = rect.top + m_nPageHeight / 23 - 1;
        if(rect.bottom > m_nPageHeight) break;
        rect.right = -1;
        for(int col = 0; col < 3; col++) {
            rect.left = rect.right + 1;
            rect.right = rect.left + nPageWidth / 3 - 1;

            CStringATL strItem = " ";
            int iIndex = iCurRow * 3 + col;
            if(iIndex < arrData.size()) strItem = arrData[iIndex].c_str();

            dc.DrawText((LPCTSTR)strItem, -1, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        }
        if(fmod(iCurRow + 1, 5) == 0 && iCurRow + 1 != 20 && iCurRow != 0) {
            rect.left = 0;
            rect.right = nPageWidth - 1;
            rect.top = rect.bottom + 1;
            rect.bottom = rect.top + m_nPageHeight / 23 - 1;
            if(rect.bottom > m_nPageHeight) break;
            dc.DrawText(" ", -1, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }
        iCurRow++;
    }

    //print footer
    m_rectDraw.bottom = FOOTER_HEIGHT * m_CharSize.cy;
    dc.SetWindowOrg(-LEFT_MARGIN * m_CharSize.cx, -m_LogicalPageSize.cy + FOOTER_HEIGHT * m_CharSize.cy);
    PrintFooter(nPage, hDC);

    // SetWindowOrg back for next page
    dc.SetWindowOrg(0, 0);
    dc.SelectFont(OldFont);
    return TRUE;
}
void CMyPrintInfo::PrePrintPage(UINT nPage, HDC hDC) {
    //BeginPrintJob(hDC);
}
void CMyPrintInfo::PostPrintPage(UINT nPage, HDC hDC) {
}

DEVMODE* CMyPrintInfo::GetNewDevModeForPage(UINT nLastPage, UINT nPage) {
    return NULL;
}
bool CMyPrintInfo::IsValidPage(UINT nPage) {
    return nPage > 0 && m_iCount > 0;
}
void CMyPrintInfo::PrintHeader(UINT nPage, ULONG nSize, HDC hDC) {
    CRect   rc(m_rectDraw);
    CStringATL strHeaderString;
    CFont   BoldFont;
    LOGFONT lf;

    CDCHandle dc(hDC);
    if(dc.IsNull()) return;
    //create bold font for header and footer
    m_PrinterFont.GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    BoldFont.CreateFontIndirect(&lf);

    HFONT pNormalFont = dc.SelectFont(BoldFont);
    int nPrevBkMode = dc.SetBkMode(TRANSPARENT);

    // print title on top center margin
    //strHeaderString.Format("(%d-%d)",(nPage-1)*60+1,(nPage-1)*60+nSize);
    //  dc.DrawText(strHeaderString,-1, rc, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

    dc.SetBkMode(nPrevBkMode);
    dc.SelectFont(pNormalFont);
    BoldFont.DeleteObject();

    dc.MoveTo(rc.left, rc.bottom);
    dc.LineTo(rc.right, rc.bottom);
}

void CMyPrintInfo::PrintFooter(UINT nPage, HDC hDC) {
    CDCHandle dc(hDC);
    CRect rc(m_rectDraw);
    CFont BoldFont;
    LOGFONT lf;

    //draw line
    dc.MoveTo(rc.left, rc.top);
    dc.LineTo(rc.right, rc.top);

    //create bold font for header and footer
    m_PrinterFont.GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    BoldFont.CreateFontIndirect(&lf);

    HFONT pNormalFont = dc.SelectFont(BoldFont);
    int nPrevBkMode = dc.SetBkMode(TRANSPARENT);

    // draw page number
    CStringATL   sTemp ;
    rc.OffsetRect(0, m_CharSize.cy / 2);
    sTemp.Format(_T("%d_A"), nPage);
    dc.DrawText(sTemp, -1, rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

    sTemp.Format(_T("%d_B"), nPage);
    dc.DrawText(sTemp, -1, rc, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

    sTemp.Format(_T("%d_C"), nPage);
    dc.DrawText(sTemp, -1, rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);


    //CTime t =::GetCurrentTime();
//  SYSTEMTIME tm;
//  GetLocalTime(&tm);

//    sTemp.Format("%d/%d/%d %d:%d",tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute);
//   dc.DrawText(sTemp,-1,rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

    dc.SetBkMode(nPrevBkMode);
    dc.SelectFont(pNormalFont);
    BoldFont.DeleteObject();
}