#pragma once


#include "HttpPostArgument.h"
#include "HttpResponseHeaders.h"

namespace pcutil
{


class CHttpClientSession
{
public:
	CHttpClientSession();
	virtual ~CHttpClientSession();

public:
	static CStringATL GetContentType( const CStringATL &strExtName );

public:
    enum RequestMethod		// REQUEST METHOD
    {
        RequestUnknown = 0,
        RequestGetMethod = 1,
        RequestPostMethod = 2,
        RequestPostMethodMultiPartsFormData = 3
    };

public:
	virtual void InitilizePostArguments();
	virtual void AddPostArguments( LPCTSTR lpName, DWORD nValue );
	virtual void AddPostArguments( LPCTSTR lpName, LPCTSTR lpValue );
	virtual void AddPostBinaryRefArgument( LPCTSTR argName, const CStringATL& fileName, LPCSTR pBuffer, DWORD len, LPCTSTR contentType = _T( "application/octet-stream" )  );

	virtual void AddPostRawText(  const std::string& szValue );
	virtual void AddPostCStringATLBody( const CStringATL& strBody );
	virtual BOOL AddOneRequestHeader( const CStringATL &strHeader, DWORD dwFlag = HTTP_ADDREQ_FLAG_ADD_IF_NEW );
	virtual void RemoveAllRequestHeader();

	virtual BOOL Request( LPCTSTR szURL, CHttpClientSession::RequestMethod Method = CHttpClientSession::RequestGetMethod , BOOL bAutoClose = TRUE );
    virtual BOOL Response( DWORD &dwResultSize );
	virtual int ReadResponse( std::string &data );				// >0

	BOOL Close();

protected:
	void ParseURL(	const CStringATL &strURL, 
					CStringATL &strProtocol, 
					CStringATL &strAddress, 
					unsigned short &nPort, 
					CStringATL &strURI 
				); 

	BOOL RequestOfURI( const CStringATL &strURI , CHttpClientSession::RequestMethod Method );
	BOOL Connect( const CStringATL &strAddress, unsigned short nPort = INTERNET_DEFAULT_HTTP_PORT, 
				  LPCTSTR szUserAccount = NULL, 
				  LPCTSTR szPassword = NULL 
				 );


//property
public:
	void SetMaxResponseContentSize( DWORD dwMaxResponseContentSize );
	DWORD GetMaxResponseContentSize();
	void SetAgentName( const CStringATL &strAgentName );
	CStringATL GetAgentName();
	void SetReferrerName( const CStringATL &strReferrerName );
	CStringATL GetReferrerName();
	void SetBoundaryName( const CStringATL &strBoundaryName );
	CStringATL GetBoundaryName();

	void SetProxy( DWORD dwType ,const CStringATL &strProxyNames , 
		const CStringATL &strProxyUser , const CStringATL &strProxyPwd );
	//void SetProxyType( DWORD dwType  ){}

	void SetContentType( const CStringATL &strContentType );

	void SetConnectionKeepAlive( BOOL bConnectionKeepAlive );

	virtual std::string& GetResponseContent();
    virtual CStringATL& GetResponseHeader();
	virtual CStringATL GetResponseTextContent();

	void SetTwoStepRequest( BOOL bTwoStepRequest );

	CHttpResponseHeadersPtr GetResponseHeaderPtr();

protected:
    virtual BOOL OpenRequest( BOOL bPost, const CStringATL& strURI,  LPCTSTR* szAcceptType );
	virtual BOOL RequestPost( const CStringATL& strURI );
	virtual BOOL RequestPostMultiParts( const CStringATL& szURI );
	virtual BOOL RequestGet( const CStringATL& strURI );
	virtual BOOL RequestPostMultiPartsFormData( LPCTSTR szURI );

	virtual void GetPostArgumentsBuffer( std::string &cache );
	virtual void AllocMultiPartsFormData( std::string &cache, LPCTSTR szBoundary );

	

//WinInet Handles
protected:
	volatile HINTERNET m_hHTTPOpen;
	volatile HINTERNET m_hHTTPConnect;
	volatile HINTERNET m_hHTTPRequest;

//Property
protected:
	//CStringATL m_strServerAddress;
	//unsigned short m_nServerPort; 
	CStringATL m_strAgentName;
	CStringATL m_strReferrerName;
	CStringATL m_strBoundaryName;
	CStringATL m_strContentType;

	DWORD m_dwMaxResponseContentSize;
//	DWORD m_dwMaxResponseHeaderSize;

	std::string m_bufResponseContent;
	CStringATL m_strResponseHeaders;

	std::vector< CHttpPostArgument > m_vArguments;
	std::vector<CHttpPostBinaryRefArgument> m_vBinaryRefArguments;

	volatile BOOL m_bHttps;

	DWORD m_dwProxyType;
	CStringATL m_strProxyNames;
	CStringATL m_strProxyUser;
	CStringATL m_strProxyPwd;


	BOOL m_bTwoStepRequest;
	BOOL m_bConnectionKeepAlive;

	CHttpResponseHeadersPtr m_ptrResponseHeaders;
	std::string m_cachePartResponse;

 
private:
	const CStringATL m_strAcceptType;
	std::vector< std::pair< CStringATL , DWORD > > m_vRequestHeader;

#ifdef _DEBUG
	CStringATL m_strRequestURL;
	RequestMethod m_emRequestMethod;
#endif

};

}