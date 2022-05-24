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
	postdata_content_type_ = request_param.postdata_content_type;
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



void CHttpClientItem::AddFixedRequestHeader( pcutil::CHttpClientSession *http )
{
	if( http == NULL ) return;
	for( size_t i = 0 ; i < fixed_requestheaders_.size(); i++ )
	{
		CStringATL header = CA2T((fixed_requestheaders_[i].first + ":" + fixed_requestheaders_[i].second).c_str()).m_psz;
		http->AddOneRequestHeader(header, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	}
}

void CHttpClientItem::OnHeaderAvailable( pcutil::CHttpClientSession* http , bool last , size_t hsize ) {
	if( last ) {
		response_headers_ = http->GetResponseHeaderPtr()->GetRawHeaders();
		http_response_code_ = http->GetResponseHeaderPtr()->GetStatusCode();
	}
}

void CHttpClientItem::OnHttpClientComplete( pcutil::CHttpClientSession* http, HttpErrorType type ) {
	CHttpResponseDataPtr res_data(new (std::nothrow) THttpResponseData());
	if (res_data.get() != NULL) {
		res_data->request_id = request_id_;
		res_data->httperror = type;
	}
	if( http != NULL && res_data.get() != NULL) {
		res_data->http_response_code = 0;
		res_data->response_type = responsecategory_ ;
		res_data->response_headers = response_headers_;
		res_data->response_content = http->GetResponseContent();
		res_data->http_response_code = http_response_code_;
		//if( type == HE_NONE && !res_data->response_content.empty() ) {

		//}
	}

	//httpclientmgr_->DoDelayAsyncHttpWork( Thread::Current() , 1 , BindAsyncFunc( &CHttpClientItem::DeleteThis , this ) );
	httpclientmgr_->OnAsyncHttpRequestComplete( res_data );
	Close();

}

void CHttpClientItem::OnSignalCheckAbort( pcutil::CHttpClientSession *http , bool *abort ) {
	BOOL aborted = httpclientmgr_->IsHttpRequestAborted( request_id_ );
	if( abort ) {
		*abort = ( (*abort) || ( aborted == TRUE ) );
	}
}

void CHttpClientItem::StartAsyncRequest(Url<char>& purl) {
	try {
		http_.reset( new (std::nothrow) pcutil::CHttpClientSession() ) ;
		if( http_.get() == NULL ) {
			throw 0;
		}
		if (!postdata_content_type_.empty()) {
			http_->SetContentType(postdata_content_type_.c_str());
		}
		if (!request_data_.empty()) {
			http_->AddPostRawText(request_data_);
		}
		AddFixedRequestHeader(http_.get());
		CStringATL url = CA2T(purl.url().c_str()).m_psz;
		BOOL result = FALSE;
		if( request_type_ == HTTP_REQUEST_GET ) {
			result = http_->Request(url, pcutil::CHttpClientSession::RequestGetMethod);
		} else {
			result = http_->Request(url, pcutil::CHttpClientSession::RequestPostMethod);
		}
		if (result) {
			OnHeaderAvailable(http_.get(), true, 0);
		}
		OnHttpClientComplete(http_.get(), result ? HE_NONE : HE_DEFAULT);
	} catch(...) {
		OnHttpClientComplete(  NULL , HE_DEFAULT );
	}
}

void CHttpClientItem::Close() {
	is_closed_ = true;
}

