// PrintWnd.h: interface for the CPrintWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTWND_H__5AE5BBAA_D79B_4EF8_BA58_57EFFE3BD83C__INCLUDED_)
#define AFX_PRINTWND_H__5AE5BBAA_D79B_4EF8_BA58_57EFFE3BD83C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

class CPrintWnd  
	:public CPrintPreviewWindowImpl<CPrintWnd,CWindow,CFrameWinTraits >
{
public:
	CPrintWnd();
	virtual ~CPrintWnd();
	typedef CPrintPreviewWindowImpl<CPrintWnd,CWindow,CFrameWinTraits > _BaseClass;

public:
	void Hide();

public:
	DECLARE_WND_CLASS_EX(_T("WTL_PrintPreview"), CS_VREDRAW | CS_HREDRAW, -1)
	BEGIN_MSG_MAP(CPrintWnd)
		COMMAND_ID_HANDLER(IDM_NEXT, OnNext)
		COMMAND_ID_HANDLER(IDM_PREV, OnPrev)
	CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()
public:
	LRESULT OnNext(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPrev(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

#endif // !defined(AFX_PRINTWND_H__5AE5BBAA_D79B_4EF8_BA58_57EFFE3BD83C__INCLUDED_)
