// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS

// Change these values to use different versions
#define WINVER		0x0501
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0400
#define _RICHEDIT_VER	0x0100

//#define _WTL_USE_CSTRING
//#define _WTL_FORWARD_DECLARE_CSTRING

//#define  __ATLSTR_H__

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlcom.h>
#include <atlhost.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atldlgs.h>
#include <atlMisc.h>

//#define _WTL_USE_CSTRING
#include <atlddx.h>
#include "atlctrlsext.h"

#include "StlFiles.h" 
#include "./db/DbOledb.h"
#include "./lua/lua.hpp"
typedef int(*lua_CFunction) (lua_State *L);

typedef ATL::CString CStringATL;

#define TOTO_COUNT 14
#define TOTO_MAXLOSE TOTO_COUNT - 1

/*
namespace WTL
{

#define DDX_TEXT_WTLSTR(nID, var) \
		if(nCtlID == (UINT)-1 || nCtlID == nID) \
		{ \
			if(!DDX_Text_WTL(nID, var, sizeof(var), bSaveAndValidate)) \
				return FALSE; \
		}

template<class T> 
class CDDX_Text_WTL
{
public:	
	BOOL DDX_Text_WTL(UINT nID, CStringATL& strText, int bSize, BOOL bSave, BOOL bValidate = FALSE, int nLength = 0)
	{
		T* pT = static_cast<T*>(this);
		BOOL bSuccess = TRUE;

		if(bSave)
		{
			HWND hWndCtrl = pT->GetDlgItem(nID);
			int nLen = ::GetWindowTextLength(hWndCtrl);
			int nRetLen = -1;
			LPTSTR lpstr = strText.GetBufferSetLength(nLen);
			if(lpstr != NULL)
			{
				nRetLen = ::GetWindowText(hWndCtrl, lpstr, nLen + 1);
				strText.ReleaseBuffer();
			}
			if(nRetLen < nLen)
				bSuccess = FALSE;
		}
		else
		{
			bSuccess = pT->SetDlgItemText(nID, strText);
		}

		if(!bSuccess)
		{
			pT->OnDataExchangeError(nID, bSave);
		}
		else if(bSave && bValidate)   // validation
		{
			ATLASSERT(nLength > 0);
			if(strText.GetLength() > nLength)
			{
				T::_XData data = { T::ddxDataText };
				data.textData.nLength = strText.GetLength();
				data.textData.nMaxLength = nLength;
				pT->OnDataValidateError(nID, bSave, data);
				bSuccess = FALSE;
			}
		}
		return bSuccess;
	}


};

};

*/


