#ifndef XMPPXEP_HTTPCLIENTITEM_H__
#define XMPPXEP_HTTPCLIENTITEM_H__

//#include "libjingle/base/sslsocketfactory.h"
//#include "libjingle/base/httpcommon.h"
#include "wininet/HttpClientSession.h"

class CHttpClientMgr;

enum EWebRequestCategory
{
	HTTP_REQUEST_GET,
	HTTP_REQUEST_POST,
	HTTP_REQUEST_POSTFORM,
	HTTP_REQUEST_POSTMULTIFORM
};

enum EWebResponseCategory
{
	RESPONSETEXT,
	RESPONSEMEMORY,
	RESPONSEFILE
};

struct THttpResponseData
{
	std::string request_id;
	talk_base::HttpErrorType httperror;
	EWebResponseCategory response_type;
	CStringATL response_headers;
	uint32 http_response_code;
	std::string response_content; //response_type=RESPONSEFILE时 ， 为临时文件名
};
typedef std::shared_ptr<THttpResponseData> CHttpResponseDataPtr;

class THttpRequestData;
typedef std::shared_ptr<THttpRequestData> CHttpRequestPtr;

class THttpRequestData
{
public:
	THttpRequestData() {
		proxy.type = talk_base::PROXY_NONE;
		request_type = HTTP_REQUEST_GET;
		response_type = RESPONSETEXT;
		use_ssl = FALSE;
		callback = NULL;
		request_time = -1;
	}
	virtual ~THttpRequestData(){}
public:
	std::string agent;
	std::string cmd;
	std::string request_id;
	std::string request_url;
	talk_base::HttpData::HeaderMap request_headers;
	EWebRequestCategory request_type;
	EWebResponseCategory response_type;
	BOOL use_ssl;
	talk_base::ProxyInfo proxy;
	int request_time;
	std::string request_post_data;

public:
	void(*callback)(const CHttpRequestPtr& pThis, const CHttpResponseDataPtr& response);

};



class CHttpClientItem
	:public sigslot::has_slots<>
{
public:
	virtual ~CHttpClientItem();
	friend class CHttpClientMgr;



	static const char* s_form_boundary_;

public:
	void Request( const THttpRequestData &request_param );
	void Close();

	inline BOOL is_closed() const { return is_closed_; }
	inline BOOL is_abort_check_arrive() const { return is_abort_check_arrived; }
	inline const std::string& request_id() { return request_id_; }
	inline void set_agent(const std::string& agent) { agent_ = agent; }

protected:
	CHttpClientItem( CHttpClientMgr *httpclientmgr );
	void StartAsyncRequest(talk_base::Url<char>& purl);
	void AddFixedRequestHeader( pcutil::CHttpClientSession *http );
	void OnHeaderAvailable( pcutil::CHttpClientSession *http , bool last , size_t hsize );
    void OnHttpClientComplete( pcutil::CHttpClientSession *http, talk_base::HttpErrorType type );
	void OnSignalCheckAbort( pcutil::CHttpClientSession *http , bool *abort );


private:
	std::string request_id_;
	CHttpClientMgr *httpclientmgr_;
	EWebResponseCategory responsecategory_;
	BOOL usessl_;
	BOOL is_closed_;
	BOOL is_abort_check_arrived;

	//talk_base::scoped_ptr< talk_base::SslSocketFactory > sslsocketfactory_;
	talk_base::scoped_ptr< pcutil::CHttpClientSession > http_;
	std::vector< std::pair< std::string , std::string > > fixed_requestheaders_;
	std::string url_;
	std::string agent_;
	talk_base::ProxyInfo proxy_;
	EWebRequestCategory request_type_;
	CStringATL response_headers_;
	uint32 http_response_code_;
	std::string response_data_;
	std::string request_data_;

};

typedef std::shared_ptr<CHttpClientItem> CHttpClientItemPtr;

#endif