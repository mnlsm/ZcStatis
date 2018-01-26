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
#include <atlprint.h>

#include <atlfile.h>
#include <atltime.h>
#include <atlsync.h>
#include <ATLComTime.h>

//#define _WTL_USE_CSTRING
#include <atlddx.h>
#include "atlctrlsext.h"

#include "StlFiles.h" 
#include <SQLiteCpp/SQLiteCpp.h>
#include "./lua/lua.hpp"
typedef int(*lua_CFunction) (lua_State *L);

typedef ATL::CString CStringATL;

#define TOTO_COUNT 14
#define TOTO_MAXLOSE TOTO_COUNT - 1

#include <hash_map>
#include <hash_set>

#include "libjingleinc.h"
