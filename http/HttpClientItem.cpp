#include "stdafx.h"
#include "HttpClientItem.h"
#include "HttpClientMgr.h"
#include "ZlibStream.h"
#include "MiscHelper.h"


const char* CHttpClientItem::s_form_boundary_ = "-----xz4BBcV-E38578A6-8C9C-4EDA-A4DE-034A865844DB---";
using namespace talk_base;
CHttpClientItem::CHttpClientItem( CHttpClientMgr *httpclientmgr ) {
	httpclientmgr_ = httpclientmgr;
	responsecategory_ = RESPONSETEXT;
	usessl_ = false;
	proxy_.type = PROXY_NONE; 
	http_response_code_ = 0;

	//agent_ = "Cleandroid/1.0";
	//fixed_requestheaders_.push_back( std::make_pair( "User-Agent" , agent_ ) );
	fixed_requestheaders_.push_back( std::make_pair( "Accept" , "*/*" ) );
	//fixed_requestheaders_.push_back( std::make_pair( "TE" , "chunked" ) );
	//fixed_requestheaders_.push_back( std::make_pair( "Referer" , "www.360.cn" ) );
	request_type_ = HTTP_REQUEST_GET;

	is_closed_ = false;
	is_abort_check_arrived = false;
}

CHttpClientItem::~CHttpClientItem() {
	Close();
}

void CHttpClientItem::Request( const THttpRequestData &request_param ) {
	request_id_ = request_param.request_id;
	url_ = request_param.request_url;
	request_data_ = request_param.request_post_data;
	request_type_ = request_param.request_type;
	usessl_ = request_param.use_ssl;
	responsecategory_ = request_param.response_type;
	proxy_ = request_param.proxy;
	if (!request_param.agent.empty()) {
		set_agent(request_param.agent);
	}
	if( request_param.request_type == HTTP_REQUEST_GET || request_param.request_type == HTTP_REQUEST_POST ) {
		fixed_requestheaders_.push_back( std::make_pair( "Content-Type" , "application/oct-stream") );
	}
	else if( request_param.request_type == HTTP_REQUEST_POSTFORM ) {
		fixed_requestheaders_.push_back( std::make_pair( "Content-Type" , "application/x-www-form-urlencoded") );
	}
	else if( request_param.request_type == HTTP_REQUEST_POSTMULTIFORM ) {
		std::string content_type = std::string( "multipart/form-data; boundary=" ) + s_form_boundary_;
		fixed_requestheaders_.push_back( std::make_pair( "Content-Type" , content_type ) );
	}
	else {
		ATLASSERT( FALSE ); 
	}

	talk_base::HttpData::HeaderMap::const_iterator citer = request_param.request_headers.begin();
	for( ; citer != request_param.request_headers.end() ; ++citer ) {
		fixed_requestheaders_.push_back( std::make_pair( citer->first , citer->second ) );
	}
	if( request_param.response_type == RESPONSEFILE ) {
		//fixed_requestheaders_.push_back( std::make_pair( "Accept-Encoding" , "identity" ) );
		fixed_requestheaders_.push_back( std::make_pair( "Accept-Encoding" , "gzip,identity" ) );
	} else {
		//fixed_requestheaders_.push_back( std::make_pair( "Accept-Encoding" , "identity" ) );
		//fixed_requestheaders_.push_back( std::make_pair( "Accept-Encoding" , "gzip,deflate" ) );
		fixed_requestheaders_.push_back( std::make_pair( "Accept-Encoding" , "gzip,deflate,identity" ) );
	}
	Url<char> purl(url_);
	fixed_requestheaders_.push_back(std::make_pair("Host", purl.address()));


	StartAsyncRequest(purl);
}



void CHttpClientItem::AddFixedRequestHeader( HttpClient *http )
{
	if( http == NULL ) return;
	for( size_t i = 0 ; i < fixed_requestheaders_.size(); i++ )
	{
		http->request().setHeader( fixed_requestheaders_[i].first , fixed_requestheaders_[i].second );
	}
}

void CHttpClientItem::OnHeaderAvailable( HttpClient* http , bool last , size_t hsize ) {
	if( last ) {
		response_headers_ = http->response().Headers();
		http_response_code_ = http->response().scode;
	}
}

void CHttpClientItem::OnHttpClientComplete( HttpClient* http, HttpErrorType type ) {
	CHttpResponseDataPtr res_data(new (std::nothrow) THttpResponseData());
	if (res_data.get() != NULL) {
		res_data->request_id = request_id_;
		res_data->httperror = type;
	}
	if( http != NULL && res_data.get() != NULL) {
		res_data->http_response_code = 0;
		res_data->response_type = responsecategory_ ;
		res_data->response_headers = response_headers_;
		res_data->response_content = response_data_;
		res_data->http_response_code = http_response_code_;
		if( type == HE_NONE && !response_data_.empty() ) {
			std::string ce_val;
			talk_base::HttpData::HeaderMap::const_iterator iter = response_headers_.find( "Content-Encoding" );
			if( iter != response_headers_.end() ) {
				ce_val = iter->second;
			}
			if (ce_val == "deflate") {//response_type为RESPONSEFILE,请求header中accept-encoding不要指定此值
				CZlibStream zlib;
				res_data->response_content.clear();
				res_data->response_content.reserve( response_data_.size() * 2 );
				if( !zlib.Decompress( response_data_.data() , response_data_.size() , res_data->response_content ) ) {
					res_data->httperror = HE_CONTENT_ENCODE;
				}
			} else if( ce_val == "gzip" ) {
				if( responsecategory_ == RESPONSEFILE ) {
					/*
					res_data->response_content += ".raw";
					std::wstring srcfile , dstfile;
					talk_base::Utf8ToWindowsFilename( response_data_, &srcfile );
					talk_base::Utf8ToWindowsFilename( res_data->response_content , &dstfile );
					CGZipFile gzip;
					if (!gzip.DecompressFile(CW2A(srcfile.c_str()), CW2A(dstfile.c_str()))) {
						res_data->httperror = HE_CONTENT_ENCODE;
						res_data->response_content = "";
					}
					DeleteFileW( srcfile.c_str() );
					*/
				} else {
					CZlibStream zlib;
					res_data->response_content.clear();
					res_data->response_content.reserve( response_data_.size() * 2 );
					if( !zlib.DecompressGZip( response_data_.data() , response_data_.size() , res_data->response_content ) ) {
						res_data->httperror = HE_CONTENT_ENCODE;
					}
				}
			}
		}
	}
	if (http_.get() != NULL) {
		http_->response().clear(true);
	}
	//httpclientmgr_->DoDelayAsyncHttpWork( Thread::Current() , 1 , BindAsyncFunc( &CHttpClientItem::DeleteThis , this ) );
	httpclientmgr_->OnAsyncHttpRequestComplete( res_data );
	Close();

}

void CHttpClientItem::OnSignalCheckAbort( HttpClient *http , bool *abort ) {
	BOOL aborted = httpclientmgr_->IsHttpRequestAborted( request_id_ );
	if( abort ) {
		*abort = ( (*abort) || ( aborted == TRUE ) );
	}
}

void CHttpClientItem::StartAsyncRequest(Url<char>& purl) {
	try {
		if (url_.empty()) {
			OnHttpClientComplete(NULL, HE_DEFAULT);
			return;
		}
		sslsocketfactory_.reset( new (std::nothrow) SslSocketFactory( Thread::Current()->socketserver() , agent_ ) );
		if (sslsocketfactory_.get() == NULL) {
			OnHttpClientComplete(NULL, HE_DEFAULT);
			return;
		}
		sslsocketfactory_->SetProxy( proxy_ );
		if( usessl_ ) {
			sslsocketfactory_->UseSSL( purl.host().c_str() );
		}
		SocketAddress server;
		if (!DoResolverUrl(purl, server)) {
			OnHttpClientComplete(NULL, HE_DEFAULT);
			return;
		}
		http_.reset( new (std::nothrow) HttpClientDefault( sslsocketfactory_.get() , agent_ , NULL ) ) ;
		if( http_.get() == NULL ) {
			throw 0;
		}
		http_->SignalHeaderAvailable.connect( this , &CHttpClientItem::OnHeaderAvailable );
		http_->SignalHttpClientComplete.connect( this , &CHttpClientItem::OnHttpClientComplete );
		http_->SignalCheckAbort.connect( this , &CHttpClientItem::OnSignalCheckAbort );
		if( request_type_ == HTTP_REQUEST_GET ) {
			http_->prepare_get( url_ );
		}
		else {
			talk_base::scoped_ptr< talk_base::StreamInterface > request_document( new (std::nothrow) StringStream( request_data_ ) );
			if( request_document.get() == NULL ) throw 0;
			http_->prepare_post( url_ , "application/oct-stream" , request_document.release() );
		}
		AddFixedRequestHeader( http_.get() );
		talk_base::scoped_ptr<StreamInterface> pStm;
		if( responsecategory_ == RESPONSEFILE ) {
			pStm.reset( new (std::nothrow) FileStream() );
			if( pStm.get() == NULL ) throw 0;
			wchar_t path[ MAX_PATH + 1 ] = { L'\0' };
			wchar_t temp_file[ MAX_PATH + 1 ] = { L'\0' };
			CMiscHelper::PathProviderWin( CMiscHelper::DIR_TEMP , NULL , path , MAX_PATH );
			GetTempFileNameW( path , L"360im" , 0 , temp_file );
			response_data_ = CMiscHelper::SysWideToUTF8( temp_file );
			if( !( (FileStream*)( pStm.get() ) )->Open( response_data_.c_str() , "wb" , NULL ) ) throw 0;
		} else {
			response_data_.clear();
			response_data_.reserve( 32 * 1024 );
			pStm.reset( new (std::nothrow) StringStream( response_data_ ) );
			if( pStm.get() == NULL ) throw 0;
		}
		http_->response().setDocumentAndLength( pStm.release() );
		http_->set_server(server);
		http_->set_request_retries(0);
		http_->start();
	} catch(...) {
		http_.reset();
		sslsocketfactory_.reset();
		OnHttpClientComplete(  NULL , HE_DEFAULT );
	}
}

void CHttpClientItem::Close() {
	is_closed_ = true;
	if(  http_.get() != NULL  ) {
		http_->SignalHeaderAvailable.disconnect(this);
		http_->SignalHttpClientComplete.disconnect(this);
		http_->SignalCheckAbort.disconnect(this);
		http_->reset();
	}
	sslsocketfactory_.reset();
}

BOOL CHttpClientItem::DoResolverUrl(const Url<char>& purl, SocketAddress& server) {
	server.Clear();
	server.SetIP(purl.host());
	server.SetPort(purl.port());
	DWORD dwThreadID = 0;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, (UINT(WINAPI*)(void*))ResolverThreadRun, &server, 0, (UINT*)&dwThreadID);
	if (hThread == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	int wait = 0, maxWait = 50;
	while (WaitForSingleObject(hThread, 200) == WAIT_TIMEOUT) {
		if (++wait >= maxWait) {
			TerminateThread(hThread, 0);
			break;
		} else if (httpclientmgr_->IsHttpRequestAborted(request_id_)) {
			TerminateThread(hThread, 0);
			break;
		}
	}
	CloseHandle(hThread);
	if (server.IsAnyIP() || httpclientmgr_->IsHttpRequestAborted(request_id_)) {
		return FALSE;
	}
#ifdef DEBUG
	std::string ipstr = server.ipaddr().ToString();
	ipstr.clear();
#endif
	return TRUE;
}

void* CHttpClientItem::ResolverThreadRun(void* pv) {
	SocketAddress* sa = (SocketAddress*)pv;
	if (sa->IsAnyIP()) {
		int err = 0;
		if (!sa->ResolveIP(false, &err)) {
			return NULL;
		}
	}
	return NULL;
}

