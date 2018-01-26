// stdafx.cpp : source file that includes just the standard includes
//	ZcStatis.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)
/*
#import "C:\\Program Files (x86)\\Common Files\\microsoft shared\\OFFICE12\\MSO.DLL" \
    rename("RGB","MsoRGB") \
    rename("SearchPath","MsoSearchPath")

#import "C:\\Program Files (x86)\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

#import "D:\\Program Files (x86)\\Microsoft Office\\Office12\\EXCEL.EXE" \
    rename( "DialogBox", "ExcelDialogBox" ) \
    rename( "RGB", "ExcelRGB" ) \
    rename( "CopyFile", "ExcelCopyFile" ) \
    rename( "ReplaceText", "ExcelReplaceText" ) \
    exclude( "IFont", "IPicture" ) raw_interface_only named_guids
*/
#ifdef DEBUG
#pragma comment(lib, "comsuppd.lib")
#else
#pragma comment(lib, "comsupp.lib")
#endif

#ifdef _DEBUG
//#pragma comment( lib , "libexpatMTd.lib")
//#ifdef USE_SSLSTREAM
//#pragma comment( lib , "libjingle_based.lib")
//#else
#pragma comment( lib , "libjingle_baseNoTLSd.lib")
//#endif
//#pragma comment( lib , "libprotobuf-lited.lib")
#pragma comment( lib , "Ws2_32.lib")
#pragma comment(lib, "zlibd.lib")

#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Wininet.lib")
#else
//#pragma comment( lib , "libexpatMT.lib")
//#ifdef USE_SSLSTREAM
//#pragma comment( lib , "libjingle_base.lib")
//#else
#pragma comment( lib , "libjingle_baseNoTLS.lib")
//#endif
//#pragma comment( lib , "libprotobuf-lite.lib")
#pragma comment( lib , "Ws2_32.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Wininet.lib")
#endif