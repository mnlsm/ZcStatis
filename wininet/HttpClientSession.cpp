#include "StdAfx.h"
#include "HttpClientSession.h"
#include <sstream>
#include "Global.h"
//

namespace pcutil
{
//#ifdef _DEBUG
//volatile long g_nHttpClientSessionCount = 0;
//CComAutoCriticalSection g_csHttpClientSessionCount;
//#endif
CHttpClientSession::CHttpClientSession()
        : m_dwMaxResponseContentSize(1024 * 100),
        m_hHTTPOpen(NULL),
        m_hHTTPConnect(NULL),
        m_hHTTPRequest(NULL),
        m_strAgentName(_T("IIC2.0/PC 4.0.0000")),
        m_strAcceptType(_T("Accept: */*\r\n")),
        m_strBoundaryName(_T("-----xz4BBcV-5C1C9B2B-95AC-4752-AF91-10B280A68D3B---")),
        m_bHttps(FALSE),
        m_bTwoStepRequest(FALSE)
{
	m_strContentType = _T("Content-Type: application/oct-stream\r\n");
    m_bufResponseContent.reserve(m_dwMaxResponseContentSize);
    m_dwProxyType = INTERNET_OPEN_TYPE_PRECONFIG;
	m_dwProxyType = INTERNET_OPEN_TYPE_DIRECT;
	m_ptrResponseHeaders.reset(new CHttpResponseHeaders());
	m_bConnectionKeepAlive = FALSE;

//#ifdef _DEBUG
//	g_csHttpClientSessionCount.Lock();
//	ATLTRACE(_T("\r\nHttp Request Count = %d increasement") , ++g_nHttpClientSessionCount);
//	g_csHttpClientSessionCount.Unlock();
//#endif
}

CHttpClientSession::~CHttpClientSession()
{
    Close();
//#ifdef _DEBUG
//	g_csHttpClientSessionCount.Lock();
//	ATLTRACE(_T("\r\nHttp Request Count = %d decreasement") , --g_nHttpClientSessionCount);
//	g_csHttpClientSessionCount.Unlock();
//#endif
}

void CHttpClientSession::SetProxy(DWORD dwType ,const CStringATL &strProxyNames ,
								  const CStringATL &strProxyUser , const CStringATL &strProxyPwd)
{
    m_dwProxyType = dwType;
	m_strProxyNames = strProxyNames;
	m_strProxyUser = strProxyUser;
	m_strProxyPwd = strProxyPwd;
}

void CHttpClientSession::SetConnectionKeepAlive(BOOL bConnectionKeepAlive)
{
	m_bConnectionKeepAlive = bConnectionKeepAlive;
}

void CHttpClientSession::SetMaxResponseContentSize(DWORD dwMaxResponseContentSize)
{
    ATLASSERT(m_hHTTPOpen == NULL);
    if (dwMaxResponseContentSize < m_dwMaxResponseContentSize)
        return ;
    m_dwMaxResponseContentSize = dwMaxResponseContentSize;
    m_bufResponseContent.reserve(dwMaxResponseContentSize);
}

DWORD CHttpClientSession::GetMaxResponseContentSize()
{
    return m_dwMaxResponseContentSize;
}

void CHttpClientSession::SetAgentName(const CStringATL &strAgentName)
{
    ATLASSERT(!strAgentName.IsEmpty());
    m_strAgentName = strAgentName;
}

CStringATL CHttpClientSession::GetAgentName()
{
    return m_strAgentName;
}

void CHttpClientSession::SetReferrerName(const CStringATL &strReferrerName)
{
    m_strReferrerName = strReferrerName;
}

CStringATL CHttpClientSession::GetReferrerName()
{
    return m_strReferrerName;
}

std::string& CHttpClientSession::GetResponseContent()
{
    return m_bufResponseContent;
}

CStringATL CHttpClientSession::GetResponseTextContent()
{
	return Global::fromUTF8(m_bufResponseContent).c_str();
}

CStringATL& CHttpClientSession::GetResponseHeader()
{
    return m_strResponseHeaders;
}

void CHttpClientSession::SetBoundaryName(const CStringATL &strBoundaryName)
{
    m_strBoundaryName = strBoundaryName;
}

CStringATL CHttpClientSession::GetBoundaryName()
{
    return m_strBoundaryName;
}

void CHttpClientSession::SetContentType(const CStringATL &strContentType)
{
	m_strContentType.Format(_T("Content-Type: %s\r\n") , strContentType); 
}



CHttpResponseHeadersPtr CHttpClientSession::GetResponseHeaderPtr()
{
	return m_ptrResponseHeaders;
}


void CHttpClientSession::SetTwoStepRequest(BOOL bTwoStepRequest)
{
    m_bTwoStepRequest = bTwoStepRequest;
}


BOOL CHttpClientSession::Connect(const CStringATL &strAddress,
                                  unsigned short nPort,
                                  LPCTSTR szUserAccount,
                                  LPCTSTR szPassword
                               )
{
	if(m_hHTTPOpen == NULL)
	{
		if(INTERNET_OPEN_TYPE_PRECONFIG == m_dwProxyType || INTERNET_OPEN_TYPE_DIRECT == m_dwProxyType)
		{
			m_hHTTPOpen =::InternetOpen(m_strAgentName,					// agent name
									 m_dwProxyType,							// proxy option
									 NULL,								// proxy
									 NULL,								// proxy bypass
									 0);								// flags
		}
		else if(INTERNET_OPEN_TYPE_PROXY == m_dwProxyType)
		{
			m_hHTTPOpen =::InternetOpen(m_strAgentName,					// agent name
									 m_dwProxyType,							// proxy option
									 m_strProxyNames,						// proxy
									 NULL,//_T("<local>"),									// proxy bypass
									 0);									// flags
		
		}
		if (m_hHTTPOpen == NULL)
		{
			return FALSE;
		}
		DWORD dwConnInfoSize = sizeof(INTERNET_CONNECTED_INFO) ;//解决IE设置成脱机工作的bug
		INTERNET_CONNECTED_INFO InetConnInfo = {0};
		if(InternetQueryOption(m_hHTTPOpen , INTERNET_OPTION_CONNECTED_STATE , (LPBYTE)&InetConnInfo , &dwConnInfoSize))
		{
			if((InetConnInfo.dwConnectedState & INTERNET_STATE_CONNECTED) == 0)
			{
				InetConnInfo.dwConnectedState = INTERNET_STATE_CONNECTED;
				InternetSetOption(m_hHTTPOpen , INTERNET_OPTION_CONNECTED_STATE , (LPBYTE)&InetConnInfo , sizeof(InetConnInfo));
			}
		}
	}

    DWORD dwFlag = 0;//(INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE);
    if(m_hHTTPConnect == NULL)
	{
		m_hHTTPConnect =::InternetConnect(
							m_hHTTPOpen,			// internet opened handle
							strAddress,				// server name
							nPort,					// ports
							szUserAccount,			// user name
							szPassword,				// password
							INTERNET_SERVICE_HTTP,	// service type
							dwFlag,	// service option
							0);					// context call-back option
		if (m_hHTTPConnect == NULL)
		{
			return FALSE;
		}


		if(INTERNET_OPEN_TYPE_PROXY == m_dwProxyType)
		{
			if(!m_strProxyUser.IsEmpty() && !m_strProxyPwd.IsEmpty())
			{
				InternetSetOption(m_hHTTPConnect , INTERNET_OPTION_PROXY_USERNAME ,
					m_strProxyUser.GetBuffer(), m_strProxyUser.GetLength());
				InternetSetOption(m_hHTTPConnect , INTERNET_OPTION_PROXY_PASSWORD  ,
					m_strProxyPwd.GetBuffer(), m_strProxyPwd.GetLength());
				m_strProxyUser.ReleaseBuffer();
				m_strProxyPwd.ReleaseBuffer();
			}
		}

		if(!m_bHttps)
		{
			m_bConnectionKeepAlive = TRUE;
			//AddOneRequestHeader((m_bConnectionKeepAlive ) ? _T("Connection: Keep-Alive") : _T("Connection: Close"));
		}

		//测试连接可用?
		if (::InternetAttemptConnect(NULL) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		DWORD dwFlags = INTERNET_CONNECTION_OFFLINE ;
		CStringATL strConnectionName;
		BOOL bOK = InternetGetConnectedStateEx(&dwFlags , strConnectionName.GetBuffer(1024), 1023 , 0);
		strConnectionName.ReleaseBuffer();
		if (!bOK || (dwFlags & INTERNET_CONNECTION_OFFLINE) == INTERNET_CONNECTION_OFFLINE)
		{
			//没有网络连接
			return TRUE;//不能返回FALSE,IE设置成脱机状态时造成BUG
		}
	}

    return TRUE;
}

BOOL CHttpClientSession::Close()
{
	SetLastError(0);
    if (m_hHTTPRequest != NULL)
    {
        ::InternetCloseHandle(m_hHTTPRequest);
        m_hHTTPRequest = NULL;
    }
    if (m_hHTTPConnect != NULL)
    {
        ::InternetCloseHandle(m_hHTTPConnect);
        m_hHTTPConnect = NULL;
    }
    if (m_hHTTPOpen !=  NULL)
    {
        ::InternetCloseHandle(m_hHTTPOpen);
        m_hHTTPOpen = NULL;
    }

#ifdef _DEBUG
	if(GetLastError() == WSAEWOULDBLOCK)
	{
		CStringATL strLog;
		strLog.Format(_T("m_strRequestURL = %s | m_emRequestMethod = %u") , m_strRequestURL , m_emRequestMethod);
//		ClientLogger::CommInfo(_T("wininet.http") , _T("") , strLog);
	}
#endif

    return TRUE;
}

void CHttpClientSession::InitilizePostArguments()
{
    m_vArguments.clear();
}

void CHttpClientSession::AddPostArguments(LPCTSTR lpName, DWORD nValue) {
    ATLASSERT(m_hHTTPOpen == NULL);
	std::string szName = Global::toUTF8(lpName);
	std::ostringstream oss;
	oss << nValue;
    m_vArguments.push_back(CHttpPostArgument(szName , oss.str(), CHttpPostArgument::TypeNormal));
}

void CHttpClientSession::AddPostArguments(LPCTSTR lpName, LPCTSTR lpValue) {
    ATLASSERT(m_hHTTPOpen == NULL);
	std::string szName = Global::toUTF8(lpName);
	std::string szValue = Global::toUTF8(lpValue);
    m_vArguments.push_back(CHttpPostArgument(szName , szValue, CHttpPostArgument::TypeNormal));
}

void CHttpClientSession::AddPostBinaryRefArgument(LPCTSTR argName, const CStringATL& fileName, 
	LPCSTR pBuffer, DWORD len, LPCTSTR contentType /*= _T("application/octet-stream") */) {
	ATLASSERT(m_hHTTPOpen == NULL);
	m_vBinaryRefArguments.push_back(CHttpPostBinaryRefArgument(argName, fileName, pBuffer, len, contentType));
}

void CHttpClientSession::AddPostRawText( const std::string& szValue)
{
    DWORD dwRandom = GetTickCount();
    std::ostringstream os;
    os << dwRandom;
    m_vArguments.push_back(CHttpPostArgument(os.str() , szValue , CHttpPostArgument::TypeRawText));
}

void CHttpClientSession::AddPostCStringATLBody(const CStringATL& strBody)
{
	AddPostRawText(Global::toUTF8((LPCSTR)strBody));
}

BOOL CHttpClientSession::AddOneRequestHeader(const CStringATL &strHeader, DWORD dwFlag )
{
    CStringATL strTemp = strHeader;
    strTemp.Trim();
    if (strTemp.Right(2) != _T("\r\n"))
    {
        strTemp += _T("\r\n");
    }
    m_vRequestHeader.push_back(std::pair< CStringATL , DWORD >(strTemp , dwFlag));
    return TRUE;
}

void CHttpClientSession::RemoveAllRequestHeader()
{
	m_vRequestHeader.clear();
}

BOOL CHttpClientSession::Request(LPCTSTR szURL,
                                  CHttpClientSession::RequestMethod Method ,
								  BOOL bAutoClose
                               )
{

#ifdef _DEBUG
	m_strRequestURL = szURL;
	m_emRequestMethod = Method;

	
	ATLTRACE(_T("\r\nHttp Request Start : URL = %s | RequestMethod = %d。 \r\n") , m_strRequestURL , m_emRequestMethod );

#endif

    BOOL bReturn = FALSE;
    try
    {
        CStringATL strProtocol;
        CStringATL strAddress;
        CStringATL strURI;
        unsigned short nPort = 0;

        m_bufResponseContent.clear();
        m_strResponseHeaders.Empty();

        this->ParseURL(szURL, strProtocol, strAddress, nPort, strURI);

        if (strProtocol.CompareNoCase(_T("https")) == 0)
        {
            m_bHttps = TRUE;
        }
        if (strProtocol.CompareNoCase(_T("https")) != 0 && strProtocol.CompareNoCase(_T("http")) != 0 )
        {
            return FALSE;
        }

        CStringATL strHostHeader;
        strHostHeader.Format(_T("Host: %s\r\n") , strAddress);
        AddOneRequestHeader(strHostHeader);

        if (Connect(strAddress, nPort))
        {
            if (RequestOfURI(strURI, Method))
            {
                DWORD dwRealResponseSize = 0 ;
                if (Response( dwRealResponseSize))
                {
                    bReturn = TRUE;
                }
            }
        }
		if(bAutoClose)
		{
			Close();
		}
    }
    catch (...)
    {
    }
    return bReturn;

}

CStringATL CHttpClientSession::GetContentType(const CStringATL &strExtName)
{
    CStringATL strReturn = _T("application/octet-stream");
    DWORD dwDot = strExtName.ReverseFind(_T('.'));
    if (dwDot <= 0)
    {
        return strReturn;
    }
    CStringATL strWord = strExtName.Mid(dwDot);
    HKEY	hKEY;
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, strWord, 0, KEY_QUERY_VALUE, &hKEY) == ERROR_SUCCESS)
    {
        DWORD dwLen = 1024 ;
        CStringATL strRegValue ;
        LONG lRet =  RegQueryValueEx(	hKEY,
                                      _T("Content Type"),
                                      NULL,
                                      NULL,
                                      (LPBYTE)strRegValue.GetBuffer(dwLen) ,
                                      (LPDWORD) & dwLen);
        strRegValue.ReleaseBuffer();
        if (lRet == ERROR_SUCCESS)
        {
            strReturn = strRegValue;
        }
        RegCloseKey(hKEY);
    }
    return strReturn;
}

void CHttpClientSession::ParseURL(const CStringATL &strURL, CStringATL &strProtocol, CStringATL &strAddress,
                                   unsigned short &nPort, CStringATL &strURI)
{
    CStringATL strRealURL = strURL;
    int pos = strRealURL.Find(_T(":/"));
    if (pos > 0)
    {
        strProtocol = strRealURL.Left(pos);
        strRealURL = strRealURL.Mid(pos + 3);
    }
    else
    {
        strProtocol = _T("http");
    }
    strAddress = strRealURL;
    pos = strRealURL.Find(_T("/"));
    if (pos >= 0)
    {
        strAddress = strRealURL.Left(pos);
        strURI = strRealURL.Mid(pos );
    }
    strRealURL.Empty();

    pos = strAddress.Find(_T(":")) ;
    if (pos >= 0)
    {
        nPort = (unsigned short)_ttol(strAddress.Mid(pos + 1));
        strAddress = strAddress.Left(pos);
    }
    else
    {
		if (strProtocol.CompareNoCase(_T("https")) == 0)
        {
            nPort = INTERNET_DEFAULT_HTTPS_PORT;
        }
        else if (strProtocol.CompareNoCase(_T("http"))== 0)
        {
            nPort = INTERNET_DEFAULT_HTTP_PORT;
        }
    }
}

BOOL CHttpClientSession::RequestOfURI(const CStringATL &strURI , CHttpClientSession::RequestMethod Method)
{
    BOOL bReturn = TRUE;
    try
    {
        switch (Method)
        {
            case CHttpClientSession::RequestPostMethod:
                bReturn = RequestPost(strURI);
                break;
            case CHttpClientSession::RequestPostMethodMultiPartsFormData:
                bReturn = RequestPostMultiParts(strURI);
                break;
            case CHttpClientSession::RequestGetMethod:
            default:
                bReturn = RequestGet(strURI);
                break;
        }
    }
    catch (...)
    {
    }
    return bReturn;
}

BOOL CHttpClientSession::RequestGet(const CStringATL& strURI)
{
    LPCTSTR ppszAcceptTypes[2];
    ppszAcceptTypes[0] = _T("*/*");  //We support accepting any mime file type since this is a simple download of a file
    ppszAcceptTypes[1] = NULL;

    if(!OpenRequest(FALSE, strURI, ppszAcceptTypes))
    {
        return FALSE;
    }

    // REPLACE HEADER
    if (!::HttpAddRequestHeaders(m_hHTTPRequest, m_strAcceptType, m_strAcceptType.GetLength(), HTTP_ADDREQ_FLAG_REPLACE))
    {
        return FALSE;
    }

    //add user header
    for (unsigned int i = 0 ; i < m_vRequestHeader.size() ; i++)
    {
        if (!::HttpAddRequestHeaders(m_hHTTPRequest,
                                       m_vRequestHeader[i].first,
                                       m_vRequestHeader[i].first.GetLength(),
                                       m_vRequestHeader[i].second))
        {
            return FALSE;
        }
    }

    DWORD dwFlags;
    DWORD dwBuffLen = sizeof(dwFlags);
    InternetQueryOption(m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
    dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
    dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION;
    InternetSetOption(m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));

    do
    {
        SetLastError(0);
        if (!::HttpSendRequest(		m_hHTTPRequest,	// handle by returned HttpOpenRequest
                                  NULL,					// additional HTTP header
                                  0,					// additional HTTP header length
                                  NULL,					// additional data in HTTP Post or HTTP Put
                                  0						// additional data length
                              )
          )
        {
            DWORD dwError = GetLastError();
            if (ERROR_INTERNET_INVALID_CA == dwError)
            {
                DWORD dwFlags = 0;
                DWORD dwBuffLen = sizeof(dwFlags);
                InternetQueryOption (m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                      (LPVOID)&dwFlags, &dwBuffLen);
                dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ; 
				dwFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ;
                dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION ;
				InternetSetOption (m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                    &dwFlags, sizeof (dwFlags));
                continue;
            }
            return FALSE;
        }
		break;
    }while (true);
    return TRUE;
}

BOOL CHttpClientSession::RequestPost(const CStringATL& strURI)
{
    LPCTSTR ppszAcceptTypes[2];
    ppszAcceptTypes[0] = _T("*/*");  //We support accepting any mime file type since this is a simple download of a file
    ppszAcceptTypes[1] = NULL;
    if(!OpenRequest(TRUE, strURI, ppszAcceptTypes))
    {
        return FALSE;
    }


    CStringATL strContentType = m_strContentType;//_T("Content-Type: application/oct-stream\r\n");
    std::string cache;
    GetPostArgumentsBuffer(cache);
    //if (cache.empty())
    //{
    //    return FALSE;
    //}

	CStringATL strContentLength;
	strContentLength.Format(_T("Content-Length: %u\r\n") , cache.size()); 
    AddOneRequestHeader(strContentLength );

    if (!::HttpAddRequestHeaders(m_hHTTPRequest, m_strAcceptType, m_strAcceptType.GetLength(), HTTP_ADDREQ_FLAG_REPLACE))
    {
        return FALSE;
    }

   // add user header
    for (unsigned int i = 0 ; i < m_vRequestHeader.size() ; i++)
    {
        if (!::HttpAddRequestHeaders(m_hHTTPRequest,
                                       m_vRequestHeader[i].first,
                                       m_vRequestHeader[i].first.GetLength(),
                                       m_vRequestHeader[i].second))
        {
            return FALSE;
        }
    }

    DWORD dwFlags;
    DWORD dwBuffLen = sizeof(dwFlags);
    InternetQueryOption(m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
    dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
    dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION;
    InternetSetOption(m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));

    do
    {
        SetLastError(0);
		if (!::HttpSendRequest(
					m_hHTTPRequest,				// handle by returned HttpOpenRequest
					strContentType,				// additional HTTP header
					strContentType.GetLength(),	// additional HTTP header length
					(void*)cache.c_str(),					// additional data in HTTP Post or HTTP Put
					cache.size()		// additional data length
				)
		  )
		{
			DWORD dwError = GetLastError();
			if (ERROR_INTERNET_INVALID_CA == dwError)
			{
                DWORD dwFlags = 0;
                DWORD dwBuffLen = sizeof(dwFlags);
                InternetQueryOption (m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                      (LPVOID)&dwFlags, &dwBuffLen);
                dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ; 
				dwFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ;
                dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION ;
                InternetSetOption (m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                    &dwFlags, sizeof (dwFlags));
                continue;
			
			}
			return FALSE;
		}
		break;
	}while(true);
    return TRUE;
}

BOOL CHttpClientSession::RequestPostMultiParts(const CStringATL& szURI)
{
    CStringATL strContentType;
    strContentType.Format(_T("Content-Type: multipart/form-data; boundary=%s\r\n") , m_strBoundaryName) ;

    //ALLOCATE POST MULTI-PARTS FORM DATA ARGUMENTS
    std::string cache;
    AllocMultiPartsFormData(cache , m_strBoundaryName);

    LPCTSTR ppszAcceptTypes[2];
    ppszAcceptTypes[0] = _T("*/*");  //We support accepting any mime file type since this is a simple download of a file
    ppszAcceptTypes[1] = NULL;
    if(!OpenRequest(TRUE, szURI, ppszAcceptTypes))
    {
        return FALSE;
    }

    // REPLACE HEADER
    if (!::HttpAddRequestHeaders(m_hHTTPRequest, m_strAcceptType, m_strAcceptType.GetLength(), HTTP_ADDREQ_FLAG_REPLACE))
    {
        return FALSE;
    }

    if (!::HttpAddRequestHeaders(m_hHTTPRequest, strContentType, strContentType.GetLength(), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
    {
        return FALSE;
    }

    CStringATL strCL;
    strCL.Format(_T("Content-Length: %u\r\n") , cache.size());
    if (!::HttpAddRequestHeaders(m_hHTTPRequest, strCL, strCL.GetLength(), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
    {
        return FALSE;
    }

    //add user header
    for (unsigned int i = 0 ; i < m_vRequestHeader.size() ; i++)
    {
        if (!::HttpAddRequestHeaders(m_hHTTPRequest,
                                       m_vRequestHeader[i].first,
                                       m_vRequestHeader[i].first.GetLength(),
                                       m_vRequestHeader[i].second))
        {
            return FALSE;
        }
    }

    // SEND REQUEST WITH HttpSendRequestEx and InternetWriteFile
    INTERNET_BUFFERS InternetBufferIn = {0};
    InternetBufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
    InternetBufferIn.Next = NULL;
    do
    {
		if (!::HttpSendRequestEx(m_hHTTPRequest, &InternetBufferIn, NULL, HSR_INITIATE, 0))
		{
			DWORD dwError = GetLastError();
			if (ERROR_INTERNET_INVALID_CA == dwError)
			{
	            DWORD dwFlags = 0;
                DWORD dwBuffLen = sizeof(dwFlags);
                InternetQueryOption (m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                      (LPVOID)&dwFlags, &dwBuffLen);
                dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ; 
				dwFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID ;
                dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION ;
                InternetSetOption (m_hHTTPRequest, INTERNET_OPTION_SECURITY_FLAGS,
                                    &dwFlags, sizeof (dwFlags));
                continue;
			}
			return FALSE;
		}

		break;
	}while(false);

    DWORD dwOutPostBufferLength = 0;
    if (!::InternetWriteFile(m_hHTTPRequest, cache.data(), cache.size(), &dwOutPostBufferLength))
    {
        return FALSE;
    }

    if (!::HttpEndRequest(m_hHTTPRequest, NULL, HSR_INITIATE, 0))
    {
        return FALSE;
    }

    return TRUE;
}

int CHttpClientSession::ReadResponse(std::string &data)				// >0
{
    ATLASSERT(m_bTwoStepRequest);
    std::string &abuf = m_cachePartResponse;
    unsigned int abufsize = 1024 * 128;
	if(abuf.length() <= 0){
		abuf.resize(abufsize, '\0');
	}
    DWORD dwNumOfBytesToRead = 0;
    BOOL bReadOK = ::InternetReadFile(	m_hHTTPRequest,
                                       (char*)abuf.data(),
                                       abuf.length(),
                                       &dwNumOfBytesToRead
                                    );
    if (bReadOK)
    {
		data.append(abuf.data(), dwNumOfBytesToRead);
    }
    else
    {
        return -1;
    }

    if (m_strResponseHeaders.IsEmpty())
    {
		abuf.resize(abufsize, '\0');
        DWORD dwHeaderBufferLength = abufsize - 1;
		if (!::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (char*)abuf.data(), &dwHeaderBufferLength, NULL))
        {
            return FALSE;
        }
		m_strResponseHeaders = (TCHAR*)abuf.data();
		m_ptrResponseHeaders->Parse(m_hHTTPRequest , m_strResponseHeaders);
    }
    return dwNumOfBytesToRead;
}

BOOL CHttpClientSession::Response(DWORD &dwResultSize) {
    if (m_bTwoStepRequest) {
		return TRUE;
	}

    std::string abuf;
    unsigned int abufsize = 1024 * 100;
	abuf.resize(abufsize, '\0');

    DWORD dwNumOfBytesToRead = 0;
    m_bufResponseContent.clear();
    BOOL bReadOK = FALSE;
    do {
		dwNumOfBytesToRead = 0;
        bReadOK = ::InternetReadFile(m_hHTTPRequest, (char*)abuf.data(),
                                      abufsize, &dwNumOfBytesToRead);
        if (bReadOK) {
			m_bufResponseContent.append(abuf.data(), dwNumOfBytesToRead);
        }
		abuf.resize(abufsize, '\0');
    } while (bReadOK && dwNumOfBytesToRead > 0);
    if (!bReadOK) {
        return FALSE;
    }

	abuf.resize(abufsize, '\0');
    DWORD dwHeaderBufferLength = abufsize - 1;
	if (!::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (char*)abuf.data(), &dwHeaderBufferLength, NULL))
    {
        return FALSE;
    }
	m_strResponseHeaders = (TCHAR*)abuf.data();
	m_ptrResponseHeaders->Parse(m_hHTTPRequest , m_strResponseHeaders);
    return TRUE;
}

void CHttpClientSession::GetPostArgumentsBuffer(std::string &cache)
{
    cache.clear();
    std::vector<CHttpPostArgument>::iterator itArg;
    for (itArg = m_vArguments.begin(); itArg < m_vArguments.end(); ++itArg)
    {
        if ((*itArg).GetType() == CHttpPostArgument::TypeNormal)
        {
            cache = cache + (*itArg).GetName() + std::string("=") + (*itArg).GetValue();
            if (itArg + 1 != m_vArguments.end())
            {
                cache +=  "&" ;
            }
        }
        else if ((*itArg).GetType() == CHttpPostArgument::TypeRawText)
        {
            cache = cache + (*itArg).GetValue();
            if (m_vArguments.size() != 1)
            {
                ATLASSERT(FALSE);
            }
        }
    }
}


void CHttpClientSession::AllocMultiPartsFormData(std::string &cache, LPCTSTR szBoundary)
{
    cache.clear();
    std::vector<CHttpPostArgument>::iterator itArgv;
    for (itArgv = m_vArguments.begin(); itArgv < m_vArguments.end(); ++itArgv)
    {
        // SET MULTI_PRATS FORM DATA BUFFER
        char strFormItem[4096] = {'\0'};

        switch ((*itArgv).GetType())
        {
            case CHttpPostArgument::TypeRawText:
                ATLASSERT(FALSE);
                break;

            case CHttpPostArgument::TypeNormal:
                sprintf(strFormItem,
                         "--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n"
                         , CT2A(szBoundary).m_psz , (*itArgv).GetName().c_str() , (*itArgv).GetValue().c_str()
                      );
                cache.append( strFormItem);
				break;
				//////////////////////////////////////////////////////////////////////////
				// Post 一个磁盘文件的方法是没有用的，不允许干这种事情！
				// 另实现为直接 Post 二进制流参数
				// 唐春林 080923
				// 
				//case CHttpPostArgument::TypeBinary:
				//HANDLE hFile =::CreateFile(CA2T((*itArgv).GetValue().c_str()).m_psz,	//File Name
				//                            GENERIC_READ,
				//                            FILE_SHARE_READ,
				//                            NULL,
				//                            OPEN_EXISTING,
				//                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, // flag and attributes
				//                            NULL
				//                         );
				//if (hFile == NULL) return ;
				//LONGLONG dwSize =::GetFileSize(hFile, NULL);
				//BOOL bNotFirstpackage = TRUE;
				//BYTE	pBytes[4096] = {'\0'};
				//DWORD	dwNumOfBytesToRead = 0;
				//while (::ReadFile(hFile, pBytes, 4096, &dwNumOfBytesToRead, NULL) && dwNumOfBytesToRead > 0)
				//{
				//    if (bNotFirstpackage)
				//    {
				//        std::string strContextType = "application/octet-stream";
				//        LPWSTR	lMime = NULL;
				//        HRESULT hr = ::FindMimeFromData(NULL, NULL, pBytes, dwNumOfBytesToRead, NULL, 0, &lMime, 0);
				//        if (NOERROR == hr)
				//        {
				//            strContextType = CW2A(lMime).m_psz;
				//        }
				//        sprintf(strFormItem, "--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n" ,
				//                 CT2A(szBoundary).m_psz , (*itArgv).GetName().c_str() ,
				//                 (*itArgv).GetValue().c_str() , strContextType.c_str());

				//        cache.append( strFormItem );
				//        bNotFirstpackage = FALSE;
				//    }
				//    cache.append((const char*)pBytes , dwNumOfBytesToRead);
				//}
				//::CloseHandle(hFile);
				//cache.append("\r\n");
				//break;
		}
	}
	std::vector<CHttpPostBinaryRefArgument>::iterator it;
	for (it = m_vBinaryRefArguments.begin(); it < m_vBinaryRefArguments.end(); ++it)
	{
		CStringATL header;
		header.Format(_T("--%s\r\nContent-Disposition: form-data; name=%s;filename=\"%s\"\r\nContent-Type: %s\r\n\r\n"),
			szBoundary, it->ArgName, it->FileName, it->ContentType);
		cache.append(Global::toUTF8((LPCSTR)header));
		cache.append(it->PBuffer, it->Length);
		cache.append("\r\n");
	}
    cache.append("--");
    cache.append(CT2A(szBoundary).m_psz);
    cache.append("\r\n");
    return ;
}


BOOL CHttpClientSession::OpenRequest(BOOL bPost, const CStringATL &strURI,  LPCTSTR* szAcceptType)
{
    DWORD dwFlag = INTERNET_FLAG_KEEP_CONNECTION |  INTERNET_FLAG_NO_CACHE_WRITE |INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI;
    CStringATL strPut = _T("GET");
    if (bPost)
    {
        dwFlag |= INTERNET_FLAG_FORMS_SUBMIT;
        strPut = _T("POST");
    }

    if (m_bHttps)
    {
        dwFlag |= INTERNET_FLAG_SECURE;
        dwFlag |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
        dwFlag |= INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlag |= INTERNET_FLAG_EXISTING_CONNECT;
		//dwFlag |= ~INTERNET_FLAG_NO_COOKIES;
	}

	if(m_hHTTPRequest != NULL)
	{
		::InternetCloseHandle(m_hHTTPRequest);
		m_hHTTPRequest = NULL;
	}

    m_hHTTPRequest = HttpOpenRequest(m_hHTTPConnect,
                            strPut, // HTTP Verb
                            strURI, // Object Name
                            _T("HTTP/1.1"), // Version
                            m_strReferrerName.IsEmpty() ? NULL : (LPCTSTR)m_strReferrerName, // Reference
                            szAcceptType, // Accept Type
                            dwFlag,
                            0); // context call-back point
	
	return m_hHTTPRequest != NULL ;
	
}


BOOL CHttpClientSession::RequestPostMultiPartsFormData(LPCTSTR szURI)
{

    return TRUE;
}


}
//CHttpClientSession httpClient;
//httpClient.AddPostArguments("UserID", "liuweigang");
//httpClient.AddPostArguments("Sign", "liuweigang");
//httpClient.AddPostArguments("File", "c:\\test.gif", true);
////Multipart
//BOOL bReturn = httpClient.Request("http://q.sohu.com/test.jsp", GenericHTTPClient::RequestPostMethodMultiPartsFormData);
////Post