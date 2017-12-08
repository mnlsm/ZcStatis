// MyPrintInfo.h: interface for the CMyPrintInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYPRINTINFO_H__8463F5C5_A914_4F32_AB94_8FB1DEE34C27__INCLUDED_)
#define AFX_MYPRINTINFO_H__8463F5C5_A914_4F32_AB94_8FB1DEE34C27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HEADER_HEIGHT       2    // For printing
#define FOOTER_HEIGHT       2
#define LEFT_MARGIN         4
#define RIGHT_MARGIN        4
#define TOP_MARGIN          1
#define BOTTOM_MARGIN       1 
#define GAP                 1
#define PAGEWIDTH			51

#include <math.h>

class CMyPrintInfo 
	:public IPrintJobInfo
{
public:
	CMyPrintInfo();
	CMyPrintInfo(const CStlStrArray &arrS)
		:m_arrData(arrS)
	{
			int nFontSize = -9;
			CStringATL strFontName = "Times New Roman";
			HFONT hF=m_PrinterFont.CreateFont(nFontSize, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                             OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, strFontName);		 
			m_iCount=m_arrData.size();
	}


	virtual ~CMyPrintInfo();
public:
	virtual void BeginPrintJob(HDC hDC);		// allocate handles needed, etc.
	virtual void EndPrintJob(HDC hDC, bool bAborted);	// free handles, etc.
	virtual void PrePrintPage(UINT nPage, HDC hDC);
	virtual bool PrintPage(UINT nPage, HDC hDC);
	virtual void PostPrintPage(UINT nPage, HDC hDC);
	virtual DEVMODE* GetNewDevModeForPage(UINT nLastPage, UINT nPage);
	virtual bool IsValidPage(UINT nPage);
public:
	ULONG GetMinPage()
	{
		if(m_iCount <= 0) return 0;
		return 1;
	}
	ULONG GetMaxPage()
	{
		if(m_iCount <= 0) return 0;
		ULONG iMax=m_iCount/60;
		if(fmod(m_iCount,60) != 0) ++iMax;
		return iMax;
	}
	void PrintFooter(UINT nPage,HDC hDC);
	void PrintHeader(UINT nPage,ULONG nSize,HDC hDC);

	CStlStrArray GetCurPageData(ULONG nPage)
	{
		CStlStrArray arrRet;
		if(nPage >=GetMinPage() && nPage<=GetMaxPage() && nPage>0)
		{
			int iPos=(nPage-1)*60;
			CStlStrArray::iterator iterB=m_arrData.begin()+iPos;
			CStlStrArray::iterator iterE=iterB+60;
			if(iterB < m_arrData.end())
			{
				iterE=iterE>=m_arrData.end()?m_arrData.end():iterE;
				arrRet.assign(iterB,iterE);
			}
		}
		return arrRet;
	}
	int setData(const CStlStrArray &arrData)
	{
		m_arrData=arrData;
		m_iCount=m_arrData.size();
		return m_iCount;
	}
public:
	int m_nPJState;
	CFont m_PrinterFont;
	CSize m_CharSize,m_PaperSize,m_LogicalPageSize;
	long m_nPageHeight;
	CRect m_rectDraw;
	LOGFONT m_Logfont;
	int m_iCount;
private:
	CStlStrArray m_arrData;
};

#endif // !defined(AFX_MYPRINTINFO_H__8463F5C5_A914_4F32_AB94_8FB1DEE34C27__INCLUDED_)
