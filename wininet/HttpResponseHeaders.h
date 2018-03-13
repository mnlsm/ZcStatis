#pragma once
#include "../auxtools.h"
//#include "UsableValue.h"

namespace pcutil
{

struct THttpResponseCookie
{
	CStringATL name;
	CStringATL value;
	CStringATL path;
	CStringATL domain;
	CStringATL expires;
	CStringATL maxage;
	CStringATL secure;
	CStringATL httponly;
};
	
	
class CHttpResponseHeaders
{

public:
	friend class CHttpClientSession;
	virtual ~CHttpResponseHeaders();


private:
	CHttpResponseHeaders();
	void Parse( HINTERNET hRequest , const CStringATL &rawheader );
	BOOL ParseCookie( std::string &cookie_line , THttpResponseCookie &ret );



public:
	int GetStatusCode();
	
	BOOL GetCookie(const CStringATL &name, THttpResponseCookie& cookies);
	__int64 GetContentLength();
	CStringATL GetRawHeaders();

private:
	int m_statusCode;
	__int64 m_contentLength;
	std::vector< THttpResponseCookie > m_vCookies;
	CStringATL m_rawHeaders;

	DISALLOW_COPY_AND_ASSIGN( CHttpResponseHeaders );
};

typedef std::shared_ptr< CHttpResponseHeaders > CHttpResponseHeadersPtr;

}