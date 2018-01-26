#include "stdafx.h"
#include "HttpClientMgr.h"
#include "CRCUtility.h"
#include "libjingle/base/base64.h"




using namespace talk_base;

void CHttpClientMgr::AsyncInit() {
	for( UINT i = 0; i < CHttpClientMgr::THREADCOUNT ; i++ ) {
		if( !thread_pool_[i].Start() )
			return ;
	}
	return ;
}

void CHttpClientMgr::AsyncTerm() {
	for( UINT i = 0; i < CHttpClientMgr::THREADCOUNT ; i++ ) {
		thread_pool_[i].Stop();
	}
	
	if( TRUE ) {
		CComCritSecLock< CComCriticalSection > guard( csObj_ );
		TEMP_THREAD::iterator iter = temp_threads_.begin();
		for( ; iter != temp_threads_.end() ; ++iter ) {
			iter->second->Stop();
		}
		SetEvent( eventThreadExited_.m_h );
	}
}

void CHttpClientMgr::DispatchHttpRequest(BOOL bNewThread, const CHttpRequestPtr &request_param) {
	talk_base::Thread *disp_thread = NULL;
	if( !bNewThread ) {
		int iIndex = ( ( pool_task_count_++ ) % CHttpClientMgr::THREADCOUNT );
		disp_thread = &thread_pool_[ iIndex ];	
	}
	else {
		CComCritSecLock< CComCriticalSection > guard( csObj_ );
		std::tr1::shared_ptr< talk_base::Thread > tmpthread( new (std::nothrow) talk_base::Thread() );
		if( tmpthread.get() != NULL ) {
			if( tmpthread->Start() ) {
				disp_thread = tmpthread.get();
				temp_threads_[ request_param->request_id ] = tmpthread;
			}
		}
	}
	if( TRUE ) {
		CComCritSecLock< CComCriticalSection > guard( csObj_ );
		running_requests_[ request_param->request_id ] = FALSE;
	}
	if( disp_thread != NULL ) {
		DoAsyncHttpWork(disp_thread, BindAsyncFunc(&CHttpClientMgr::DispatchHttpRequest_Http, this, request_param));
	}
	if (request_param->request_time > -1) {
		DoDelayAsyncSessionWork(request_param->request_time, BindAsyncFunc(&CHttpClientMgr::AsyncCheckHttpRequestTimeout, 
			this, request_param->request_id));
	}
}

void CHttpClientMgr::AbortHttpRequest( const std::string &request_id ) {
	CComCritSecLock< CComCriticalSection > guard( csObj_ );
	running_requests_[ request_id ] = TRUE;
}

BOOL CHttpClientMgr::IsHttpRequestAborted( const std::string &request_id ) {
	CComCritSecLock< CComCriticalSection > guard( csObj_ );
	if( abortall_ ) return TRUE;
	stdext::hash_map< std::string , BOOL >::iterator iter = running_requests_.find( request_id );
	if( iter != running_requests_.end() ) {
		if( iter->second )
			return TRUE;
	}
	return FALSE;
}

void CHttpClientMgr::DispatchHttpRequest_Http(const CHttpRequestPtr &request_param) {
	try {
		CHttpClientItemPtr pItem(new (std::nothrow) CHttpClientItem(this));
		if( pItem.get() != NULL ) {
			pItem->Request( *request_param );
			DoDelayAsyncHttpWork(Thread::Current(), 500, BindAsyncFunc(&CHttpClientMgr::AsyncCheckHttpClientStatus, this, pItem));
		}
	}
	catch(...) {
		CHttpResponseDataPtr data(new (std::nothrow)THttpResponseData());
		data->request_id = request_param->request_id;
		data->httperror = HE_DEFAULT;
		OnAsyncHttpRequestComplete( data );	
	}
}

void CHttpClientMgr::AsyncCheckHttpClientStatus(CHttpClientItemPtr ptr) {
	if (ptr.get() == NULL) {
		return;
	}
	if (abortall_ || IsHttpRequestAborted(ptr->request_id())) {
		ptr->Close();
		return;
	}
	if (ptr->is_abort_check_arrive() || ptr->is_closed()) {
		return;
	}
	DoDelayAsyncHttpWork(Thread::Current(), 500, BindAsyncFunc(&CHttpClientMgr::AsyncCheckHttpClientStatus, this, ptr));
}



void CHttpClientMgr::AsyncCheckHttpRequestTimeout( const std::string &request_id ) {
	CHttpResponseDataPtr data(new (std::nothrow)THttpResponseData());
	data->request_id = request_id;
	data->httperror = HE_DEFAULT;
	AbortHttpRequest( request_id );
	OnAsyncHttpRequestComplete( data );
}


void CHttpClientMgr::AsyncDoHttpCommandRequest(const CHttpRequestPtr &cmd, const std::string &request_id) {
	if( TRUE ) {
		CComCritSecLock< CComCriticalSection > guard( csObj_ );
		pend_requests_[ request_id ] = cmd;
	}
	try {
		DispatchHttpRequest(FALSE, cmd);
	} catch(...) {
	}
}

void CHttpClientMgr::OnAsyncHttpRequestComplete(const CHttpResponseDataPtr &data) {
	if( Thread::Current() != &session_thread_ ) {
		DoAsyncSessionWork( BindAsyncFunc( &CHttpClientMgr::OnAsyncHttpRequestComplete , this , data ) );
		return;
	}
	try {
		CHttpRequestPtr cmd = FindRequestCommand(data->request_id, TRUE);
		if( cmd.get() == NULL ) 
			return;
		if (cmd->callback != NULL) {
			cmd->callback(cmd, data);
		}
	} catch(...) {
		OutputDebugStringA("OnAsyncHttpRequestComplete exception!");
	}
	RemoveHttpCommandRequest( data->request_id );
}

/*
void CHttpClientMgr::UploadPortrait( const CXepModuleCommandConvPtr& cmd , const std::string &request_id )
{
	const CUploadPortrait &portrait = cmd->GetExtension( xepmod::UploadPortrait );
	if( portrait.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( (int)portrait.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = portrait.url();
	request_param.request_type = HTTP_REQUEST_POSTMULTIFORM;
	request_param.response_type = RESPONSETEXT;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( portrait.has_proxy() )
		request_param.proxy = ConvertToJingleProxy( portrait.proxy() );
	std::ostringstream oss;
	ReadFileDataToStdString( portrait.filename().c_str() , &request_param.request_post_data );
	oss << "--" << CHttpClientItem::s_form_boundary_ << "\r\n"
		<< "Content-Disposition: form-data; name=\"face_file\"; filename=\"face_file.png\"\r\n"
		<<"Content-Type: application/oct-stream\r\n\r\n"
		<< request_param.request_post_data << "\r\n"
		<<"--" << CHttpClientItem::s_form_boundary_ << "\r\n"; 
	request_param.request_post_data  = oss.str();
	DispatchHttpRequest( FALSE , request_param );
}

void CHttpClientMgr::UploadPortraitRet( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CUploadPortraitRet *pRet = ret.MutableExtension( xepmod::UploadPortraitRet );
	pRet->mutable_request()->CopyFrom( cmd->GetExtension( xepmod::UploadPortrait ) );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		Json::Reader jreader;
		Json::Value rootValue;
		bool bOK = jreader.parse( res_data.response_content , rootValue , true );
		if( bOK )
		{
			Json::Value v;
			GetValueFromJsonObject( rootValue , "result" , &v );
			pRet->set_result( v.asInt() );
			GetValueFromJsonObject( rootValue , "msg" , &v );
			std::wstring sss = CXEPUtils::SysUTF8ToWide( v.asCString() );
			pRet->set_msg( v.asCString() );
			GetValueFromJsonObject( rootValue , "data" , &v );
			if( v.isString() ) pRet->set_data( v.asCString() );
			else pRet->set_result( pRet->result() == 1 ? -1 : pRet->result() );
		}
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 256 ) );
}


void CHttpClientMgr::DownloadPortrait( const CXepModuleCommandConvPtr& cmd , const std::string &request_id )
{
	const CDownloadPortrait &portrait = cmd->GetExtension( xepmod::DownloadPortrait );
	if( portrait.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( (int)portrait.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = portrait.url();
	request_param.request_type = HTTP_REQUEST_GET;
	request_param.response_type = RESPONSEFILE;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( portrait.has_proxy() )
		request_param.proxy = ConvertToJingleProxy( portrait.proxy() );
	DispatchHttpRequest( TRUE , request_param );
}

void CHttpClientMgr::DownloadPortraitRet( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CDownloadPortraitRet *pRet = ret.MutableExtension( xepmod::DownloadPortraitRet );
	pRet->mutable_request()->CopyFrom( cmd->GetExtension( xepmod::DownloadPortrait ) );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		pRet->set_result( 1 );
		pRet->set_tempfilename( res_data.response_content );
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 256 ) );
}

void CHttpClientMgr::GetNearbyPerson( const CXepModuleCommandConvPtr& cmd , const std::string &request_id )
{
	const CGetNearbyPerson &np = cmd->GetExtension( xepmod::GetNearbyPerson );
	if( np.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( (int)np.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = np.url();
	request_param.request_type = HTTP_REQUEST_GET;
	request_param.response_type = RESPONSETEXT;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( np.has_proxy() )
		request_param.proxy = ConvertToJingleProxy( np.proxy() );
	DispatchHttpRequest( FALSE , request_param );
}

void CHttpClientMgr::GetNearbyPersonRet( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CGetNearbyPersonRet *pRet = ret.MutableExtension( xepmod::GetNearbyPersonRet );
	pRet->mutable_request()->CopyFrom( cmd->GetExtension( xepmod::GetNearbyPerson ) );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		Json::Reader jreader;
		Json::Value rootValue;
		bool bOK = jreader.parse( res_data.response_content , rootValue , true );
		if( bOK )
		{
			pRet->set_result( 1 );
			Json::Value v;
			if( GetValueFromJsonObject( rootValue , "total" , &v ) )
			{
				pRet->set_count( v.asUInt() );
				if( pRet->count() > 0 )
				{
					GetValueFromJsonObject( rootValue , "localer" , &v );
					if( v.isArray() )
					{
						for( Json::Value::ArrayIndex i = 0; i < v.size(); ++i )
						{
							CGetNearbyPersonRet_CNearbyPersonInfo *pInfo = pRet->add_persons();
							Json::Value v1;
							GetValueFromJsonObject( v[i] , "qid" , &v1 );
							if( v1.isString() )
							{
								pInfo->set_qid( v1.asCString() );
							}
							GetValueFromJsonObject( v[i] , "nn" , &v1 );
							if( v1.isString() )
							{
								pInfo->set_nickname( v1.asCString() );
							}
							GetValueFromJsonObject( v[i] , "imid" , &v1 );
							if( v1.isString() )
							{
								pInfo->set_imid( v1.asCString() );
							}
							GetValueFromJsonObject( v[i] , "sex" , &v1 );
							if( v1.isString() )
							{
								pInfo->set_sex( v1.asCString() );
							}
							GetValueFromJsonObject( v[i] , "uf" , &v1 );
							if( v1.isString() )
							{
								pInfo->set_faceurl( v1.asCString() );
							}
						}
					}
				}
			}
			else if( GetValueFromJsonObject( rootValue , "im_error" , &v ) )
			{
				if( v.isUInt() )
				{
					pRet->mutable_errinfo()->set_im_errno( v.asUInt() );
				}
				if( GetValueFromJsonObject( rootValue , "errmsg" , &v ) )
				{
					if( v.isString() )
					{
						pRet->mutable_errinfo()->set_errmsg( v.asCString() );
					}
				}
				if( GetValueFromJsonObject( rootValue , "consume" , &v ) )
				{
					if( v.isUInt() )
					{
						pRet->mutable_errinfo()->set_consume( v.asUInt() );
					}
				}
			}

		}		
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 512 ) );
}

static void AddFCRequestCookie( const std::string &data , THttpRequestData &request_param )
{
	DWORD dwCRC = BufCrc32( (const BYTE *)data.data() , data.length() );
	dwCRC = _lrotl( dwCRC , 13 );
	char buf[20] = {'\0'};
//	_ultoa( dwCRC , buf , 10 );
	sprintf( buf , "cpv=%u" , dwCRC );
	request_param.request_headers.insert( std::pair< std::string , std::string >( "Cookie" , buf ) ) ;
}

void CHttpClientMgr::DoFCAddRequest( const CXepModuleCommandConvPtr& cmd , const std::string &request_id )
{
	const CFCAddRequest &reqdata = cmd->GetExtension( xepmod::FCAddRequest );
	if( reqdata.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( reqdata.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = reqdata.url();
	request_param.request_type = HTTP_REQUEST_POSTFORM;
	request_param.response_type = RESPONSETEXT;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( reqdata.has_proxy() )
	{
		request_param.proxy = ConvertToJingleProxy( reqdata.proxy() );
	}
	Json::Value root( Json::objectValue );
	root["cmd"] = "add";	
	root["uin"] = reqdata.jid();
	root["seq"] = (Json::UInt)GetTickCount();
	const CFileCollectItem& reqdata_item = reqdata.item();
	if( reqdata_item.has_task_seed() )
	{
		Json::Value file_info( Json::objectValue );
		file_info["file_sig"] = reqdata_item.file_sig();
		file_info["file_count"] = reqdata_item.file_count();
		file_info["path_count"] = reqdata_item.path_count();
		file_info["total_size"] = reqdata_item.total_size();
		file_info["task_seed_type"] = reqdata_item.task_seed_type();
		file_info["task_seed"] = reqdata_item.task_seed();
		file_info["file_type"] = reqdata_item.file_type();
		root["file_info"] = file_info;
	}
	Json::Value favorite_item( Json::objectValue );
	favorite_item["file_sig"] = reqdata_item.file_sig();
	favorite_item["collect_name"] = reqdata_item.collect_name();
	favorite_item["collect_desc"] = reqdata_item.collect_desc();
	favorite_item["collect_tags"] = reqdata_item.collect_tags();
	favorite_item["collect_rights"] = reqdata_item.collect_rights();
	favorite_item["collect_flag"] = reqdata_item.collect_flag();
	favorite_item["collect_id"] = reqdata_item.collect_id();
	favorite_item["create_time"] = 0;
	root["favorite_item"] = favorite_item;
	std::string strJson = JsonValueToString( root );
	std::string strEncJson;
	EncryptFCData( reqdata.enckey() , strJson , strEncJson );
	request_param.request_post_data = std::string("user=") + UrlEncodeStringWithoutEncodingSpaceAsPlus( reqdata.jid() ) 
									  + std::string("&req=") + strEncJson;
	AddFCRequestCookie( strEncJson , request_param );
	DispatchHttpRequest( FALSE , request_param );
}

void CHttpClientMgr::OnFCAddResponse( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CFCAddResponse *pRet = ret.MutableExtension( xepmod::FCAddResponse );
	pRet->mutable_request()->CopyFrom( cmd->GetExtension( xepmod::FCAddRequest ) );
	pRet->set_http_response_code( res_data.http_response_code );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		std::string response_content = res_data.response_content;
		if( DecryptFCData( pRet->request().enckey() , response_content ) )
		{
			Json::Reader jreader;
			Json::Value rootValue , val;
			bool bOK = jreader.parse( response_content , rootValue , true );
			if( bOK )
			{
				pRet->set_success( true );
				if( GetValueFromJsonObject( rootValue , "result" , &val ) )
				{
					pRet->set_result( val.asUInt() );
				}
				if( GetValueFromJsonObject( rootValue , "create_time" , &val ) )
				{
					if( val.isUInt64() )
					{
						pRet->mutable_request()->mutable_item()->set_create_time( val.asUInt64() );
					}
				}
			}
		}
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 512 ) );
}

void CHttpClientMgr::DoFCRemoveRequest( const CXepModuleCommandConvPtr& cmd , const std::string &request_id )
{
	const CFCRemoveRequest &reqdata = cmd->GetExtension( xepmod::FCRemoveRequest );
	if( reqdata.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( reqdata.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = reqdata.url();
	request_param.request_type = HTTP_REQUEST_POSTFORM;
	request_param.response_type = RESPONSETEXT;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( reqdata.has_proxy() )
		request_param.proxy = ConvertToJingleProxy( reqdata.proxy() );
	Json::Value root( Json::objectValue );
	root["cmd"] = "remove";	
	root["uin"] = reqdata.jid();
	root["seq"] = (Json::UInt)GetTickCount();
	root["collect_id"] = reqdata.collect_id();
	std::string strJson = JsonValueToString( root );
	std::string strEncJson;
	EncryptFCData( reqdata.enckey() , strJson , strEncJson );
	request_param.request_post_data = std::string("user=") + UrlEncodeStringWithoutEncodingSpaceAsPlus( reqdata.jid() ) 
		+ std::string("&req=") + strEncJson;
	DispatchHttpRequest( FALSE , request_param );
}

void CHttpClientMgr::OnFCRemoveResponse( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CFCRemoveResponse *pRet = ret.MutableExtension( xepmod::FCRemoveResponse );
	pRet->mutable_request()->CopyFrom( cmd->GetExtension( xepmod::FCRemoveRequest ) );
	pRet->set_http_response_code( res_data.http_response_code );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		std::string response_content = res_data.response_content;
		if( DecryptFCData( pRet->request().enckey() , response_content ) )
		{
			Json::Reader jreader;
			Json::Value rootValue , val;
			bool bOK = jreader.parse( res_data.response_content  , rootValue , true );
			if( bOK )
			{
				pRet->set_success( true );
				if( GetValueFromJsonObject( rootValue , "result" , &val ) )
				{
					pRet->set_result( val.asUInt() );
				}
			}
		}
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 512 ) );
}

void CHttpClientMgr::DoFCModifyInfoRequest( const CXepModuleCommandConvPtr& cmd , const std::string &request_id )
{
	const CFCModifyInfoRequest &reqdata = cmd->GetExtension( xepmod::FCModifyInfoRequest );
	if( reqdata.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( reqdata.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = reqdata.url();
	request_param.request_type = HTTP_REQUEST_POSTFORM;
	request_param.response_type = RESPONSETEXT;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( reqdata.has_proxy() )
		request_param.proxy = ConvertToJingleProxy( reqdata.proxy() );
	Json::Value root( Json::objectValue );
	root["cmd"] = "modify";	
	root["uin"] = reqdata.jid();
	root["seq"] = (Json::UInt)GetTickCount();
	root["collect_id"] = reqdata.collect_id();
	root["file_sig"] = reqdata.file_sig();
	
	Json::Value modify_items( Json::objectValue );
	for( int i = 0 ; i < reqdata.items_size(); i++ )
	{
		const CFCModifyInfoRequest_pairdata &pair_item = reqdata.items(i);
		if( pair_item.key() == "collect_tags" )
		{
			modify_items["collect_tags"] = pair_item.val();
			continue;
		}
		if( pair_item.key() == "collect_desc" )
		{
			modify_items["collect_desc"] = pair_item.val();
			continue;
		}
		if( pair_item.key() == "collect_rights" )
		{
			modify_items["collect_rights"] = (Json::UInt)strtoul( pair_item.val().c_str() , NULL , 10 );
			continue;
		}
		if( pair_item.key() == "good_score" )
		{
			modify_items["good_score"] = (Json::UInt)strtoul( pair_item.val().c_str() , NULL , 10 );
			continue;
		}
		if( pair_item.key() == "bad_score" )
		{
			modify_items["bad_score"] = (Json::UInt)strtoul( pair_item.val().c_str() , NULL , 10 );
			continue;
		}
		if( pair_item.key() == "collect_flag" )
		{
			modify_items["collect_flag"] = (Json::UInt64)_strtoui64( pair_item.val().c_str() , NULL , 10 );
			continue;
		}
	}
	root["modify_item"] = modify_items;
	std::string strJson = JsonValueToString( root );
	std::string strEncJson;
	EncryptFCData( reqdata.enckey() , strJson , strEncJson );
	request_param.request_post_data = std::string("user=") + UrlEncodeStringWithoutEncodingSpaceAsPlus( reqdata.jid() ) 
		+ std::string("&req=") + strEncJson;
	DispatchHttpRequest( FALSE , request_param );
}

void CHttpClientMgr::OnFCModifyInfoResponse( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CFCModifyInfoResponse *pRet = ret.MutableExtension( xepmod::FCModifyInfoResponse );
	pRet->mutable_request()->CopyFrom( cmd->GetExtension( xepmod::FCModifyInfoRequest ) );
	pRet->set_http_response_code( res_data.http_response_code );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		std::string response_content = res_data.response_content;
		if( DecryptFCData( pRet->request().enckey() , response_content ) )
		{
			Json::Reader jreader;
			Json::Value rootValue , val;
			bool bOK = jreader.parse( res_data.response_content  , rootValue , true );
			if( bOK )
			{
				pRet->set_success( true );
				if( GetValueFromJsonObject( rootValue , "result" , &val ) )
				{
					pRet->set_result( val.asUInt() );
				}
			}
		}
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 512 ) );
}

void CHttpClientMgr::DoFCGetListRequest( const CXepModuleCommandConvPtr& cmd , const std::string &request_id )
{
	const CFCGetListRequest &reqdata = cmd->GetExtension( xepmod::FCGetListRequest );
	if( reqdata.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( reqdata.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = reqdata.url();
	request_param.request_type = HTTP_REQUEST_POSTFORM;
	request_param.response_type = RESPONSETEXT;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( reqdata.has_proxy() )
		request_param.proxy = ConvertToJingleProxy( reqdata.proxy() );
	Json::Value root( Json::objectValue );
	root["cmd"] = "getlist";	
	root["uin"] = reqdata.jid();
	root["seq"] = (Json::UInt)GetTickCount();
	Json::Value search_items( Json::arrayValue );
	for( int i = 0 ; i < reqdata.params_size() ; i++ )
	{
		const CFCGetListRequest_search_param &param = reqdata.params( i );
		Json::Value param_json( Json::objectValue );
		param_json["uin"] = param.jid();
		param_json["pagesize"] = (Json::UInt)param.pagesize();
		param_json["pageindex"] = (Json::UInt)param.pageindex();
		param_json["total_count"] = (Json::UInt64)param.total_count();
		search_items.append( param_json );
	}
	root["search_items"] = search_items;
	std::string strJson = JsonValueToString( root );
	std::string strEncJson;
	EncryptFCData( reqdata.enckey() , strJson , strEncJson );
	request_param.request_post_data = std::string("user=") + UrlEncodeStringWithoutEncodingSpaceAsPlus( reqdata.jid() ) 
		+ std::string("&req=") + strEncJson;
	DispatchHttpRequest( FALSE , request_param );
}

void CHttpClientMgr::OnFCGetListResponse( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CFCGetListResponse *pRet = ret.MutableExtension( xepmod::FCGetListResponse );
	CFCGetListRequest *request = pRet->mutable_request();
	request->CopyFrom( cmd->GetExtension( xepmod::FCGetListRequest ) );
	pRet->set_http_response_code( res_data.http_response_code );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		std::string response_content = res_data.response_content;
		if( DecryptFCData( pRet->request().enckey() , response_content ) )
		{
			Json::Reader jreader;
			Json::Value rootValue , val;
			bool bOK = jreader.parse( res_data.response_content  , rootValue , true );
			if( bOK )
			{
				pRet->set_success( true );
				if( GetValueFromJsonObject( rootValue , "result" , &val ) )
				{
					pRet->set_result( val.asUInt() );
				}
				if( GetValueFromJsonObject( rootValue , "list_items" , &val ) && val.isArray() )
				{
					for( Json::Value::ArrayIndex i = 0; i < val.size(); ++i )	
					{
						const Json::Value &vItem = val[i];
						if( !vItem.isObject() ) continue;
						const Json::Value &search_item = vItem["search_item"];
						if( !search_item.isObject() ) continue;
						const Json::Value& v_uin = search_item["uin"];
						const Json::Value& v_pagesize= search_item["pagesize"];
						const Json::Value& v_pageindex = search_item["pageindex"];
						const Json::Value& v_total_count = search_item["total_count"];
						if( !( v_uin.isString() && v_pagesize.isUInt() && v_pageindex.isUInt() ) ) 
							continue;
						CFCGetListRequest_search_param* curparam = NULL;
						for( int j = 0 ; j < request->params_size() ; j++ )
						{
							curparam = request->mutable_params( j );
							if( curparam->jid() == v_uin.asCString() 
								&& curparam->pagesize() == v_pagesize.asUInt() 
								&& curparam->pageindex() == v_pageindex.asUInt() )							
							{
								if( v_total_count.isUInt64() )
									curparam->set_total_count( v_total_count.asUInt64() );
								break;
							}
						}
						if( curparam == NULL ) continue;
						const Json::Value &favorite_items = vItem["favorite_items"];
						if( !favorite_items.isArray() ) continue;
						for( Json::Value::ArrayIndex k = 0; k < favorite_items.size(); ++k )
						{
							const Json::Value &file_info = favorite_items[k]["file_info"];
							const Json::Value &favorite_item = favorite_items[k]["favorite_item"];
							if( !file_info.isObject() || !favorite_item.isObject() ) continue;
							CFileCollectItem *fcItem = curparam->add_items();
							if( fcItem == NULL ) continue;
							const Json::Value& v_file_sig = file_info["file_sig"];
							const Json::Value& v_collect_file_count = file_info["file_count"];
							const Json::Value& v_collect_path_count = file_info["path_count"];
							const Json::Value& v_collect_contentsize = file_info["total_size"];
							const Json::Value& v_task_seed_type = file_info["task_seed_type"];
							const Json::Value& v_task_seed = file_info["task_seed"];
							const Json::Value& v_collect_file_type = favorite_item["file_type"];

							const Json::Value& v_collect_id = favorite_item["collect_id"];
							const Json::Value& v_collect_name = favorite_item["collect_name"];
							const Json::Value& v_collect_desc = favorite_item["collect_desc"];
							const Json::Value& v_collect_tags = favorite_item["collect_tags"];
							const Json::Value& v_collect_rights = favorite_item["collect_rights"];
							const Json::Value& v_collect_flag = favorite_item["collect_flag"];
							const Json::Value& v_create_time = favorite_item["create_time"];
							if( v_file_sig.isString() )
								fcItem->set_file_sig( v_file_sig.asString() );
							if( v_collect_file_type.isUInt() )
								fcItem->set_file_type( v_collect_file_type.asUInt() );
							if( v_collect_file_count.isUInt() )
								fcItem->set_file_count( v_collect_file_count.asUInt() );
							if( v_collect_path_count.isUInt() )
								fcItem->set_path_count( v_collect_path_count.asUInt() );
							if( v_collect_contentsize.isUInt64() )
								fcItem->set_total_size( v_collect_contentsize.asUInt64() );
							if( v_task_seed_type.isUInt() )
								fcItem->set_task_seed_type( v_task_seed_type.asUInt() );
							if( v_task_seed.isString() )
								fcItem->set_task_seed( v_task_seed.asString() );


							if( v_collect_id.isString() )
								fcItem->set_collect_id( v_collect_id.asString() );
							if( v_collect_name.isString() )
								fcItem->set_collect_name( v_collect_name.asString() );
							if( v_collect_desc.isString() )
								fcItem->set_collect_desc( v_collect_desc.asString() );
							if( v_collect_tags.isString() )
								fcItem->set_collect_tags( v_collect_tags.asString() );
							if( v_collect_rights.isUInt() )
								fcItem->set_collect_rights( v_collect_rights.asUInt() );
							if( v_collect_flag.isUInt64() )
								fcItem->set_collect_flag( v_collect_flag.asUInt64() );
							if( v_create_time.isUInt64() )
								fcItem->set_create_time( v_create_time.asUInt64() );
						}
					}
				}
			}
		}
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 1024 ) );}

void CHttpClientMgr::DoFCGetSeedRequest( const CXepModuleCommandConvPtr& cmd , const std::string &request_id )
{
	const CFCGetSeedRequest &reqdata = cmd->GetExtension( xepmod::FCGetSeedRequest );
	if( reqdata.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( reqdata.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = reqdata.url();
	request_param.request_type = HTTP_REQUEST_POSTFORM;
	request_param.response_type = RESPONSETEXT;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( reqdata.has_proxy() )
		request_param.proxy = ConvertToJingleProxy( reqdata.proxy() );
	Json::Value root( Json::objectValue );
	root["cmd"] = "getseed";	
	root["uin"] = reqdata.jid();
	root["seq"] = (Json::UInt)GetTickCount();
	root["file_sig"] = reqdata.file_sig();
	std::string strJson = JsonValueToString( root );
	std::string strEncJson;
	EncryptFCData( reqdata.enckey() , strJson , strEncJson );
	request_param.request_post_data = std::string("user=") + UrlEncodeStringWithoutEncodingSpaceAsPlus( reqdata.jid() ) 
		+ std::string("&req=") + strEncJson;
	DispatchHttpRequest( FALSE , request_param );
}

void CHttpClientMgr::OnFCGetSeedResponse( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CFCGetSeedResponse *pRet = ret.MutableExtension( xepmod::FCGetSeedResponse );
	pRet->mutable_request()->CopyFrom( cmd->GetExtension( xepmod::FCGetSeedRequest ) );
	pRet->set_http_response_code( res_data.http_response_code );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		std::string response_content = res_data.response_content;
		if( DecryptFCData( pRet->request().enckey() , response_content ) )
		{
			Json::Reader jreader;
			Json::Value rootValue , val;
			bool bOK = jreader.parse( res_data.response_content  , rootValue , true );
			if( bOK )
			{
				pRet->set_success( true );
				if( GetValueFromJsonObject( rootValue , "result" , &val ) )
				{
					pRet->set_result( val.asUInt() );
				}
				if( GetValueFromJsonObject( rootValue , "task_seed_type" , &val ) )
				{
					pRet->set_task_seed_type( val.asUInt() );
				}
				if( GetValueFromJsonObject( rootValue , "task_seed" , &val ) )
				{
					pRet->set_task_seed( val.asCString() );
				}
			}
		}
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 512 ) );
}

void CHttpClientMgr::DoFCGetBillBoardRequest( const CXepModuleCommandConvPtr& cmd , const std::string &request_id ) 
{
	const CFCGetBillBoardRequest &reqdata = cmd->GetExtension( xepmod::FCGetBillBoardRequest );
	if( reqdata.timeout() != 0x7FFFFFFF )
	{
		DoDelayAsyncSessionWork( reqdata.timeout() , BindAsyncFunc( &CHttpClientMgr::AsyncCheckHttpRequestTimeout , this , request_id )  );
	}
	THttpRequestData request_param;
	request_param.request_id = request_id;
	request_param.request_url = reqdata.url();
	request_param.request_type = HTTP_REQUEST_POSTFORM;
	request_param.response_type = RESPONSETEXT;
	request_param.use_ssl = ( cmd->cmdtype() == xepmod::HTTPS_COMMAND );
	if( reqdata.has_proxy() )
		request_param.proxy = ConvertToJingleProxy( reqdata.proxy() );
	Json::Value root( Json::objectValue );
	root["cmd"] = "getbillboard";	
	root["uin"] = reqdata.jid();
	root["seq"] = (Json::UInt)GetTickCount();
	root["pagesize"] = (Json::UInt)reqdata.pagesize();
	root["pageindex"] = (Json::UInt)reqdata.pageindex();
	std::string strJson = JsonValueToString( root );
	std::string strEncJson;
	EncryptFCData( reqdata.enckey() , strJson , strEncJson );
	request_param.request_post_data = std::string("user=") + UrlEncodeStringWithoutEncodingSpaceAsPlus( reqdata.jid() ) 
		+ std::string("&req=") + strEncJson;
	DispatchHttpRequest( FALSE , request_param );
}

void CHttpClientMgr::OnFCGetBillBoardResponse( const CXepModuleCommandConvPtr& cmd , const THttpResponseData &res_data )
{
	CXepModuleCommandConv ret;
	ret.set_sessid( cmd->sessid() );
	if( cmd->has_context() ) ret.set_context( cmd->context() );	
	CFCGetBillBoardResponse *pRet = ret.MutableExtension( xepmod::FCGetBillBoardResponse );
	CFCGetBillBoardRequest *request = pRet->mutable_request();
	request->CopyFrom( cmd->GetExtension( xepmod::FCGetBillBoardRequest ) );
	pRet->set_http_response_code( res_data.http_response_code );
	if( talk_base::HE_NONE == res_data.httperror )
	{
		std::string response_content = res_data.response_content;
		if( DecryptFCData( pRet->request().enckey() , response_content ) )
		{
			Json::Reader jreader;
			Json::Value rootValue , val;
			bool bOK = jreader.parse( res_data.response_content  , rootValue , true );
			if( bOK )
			{
				pRet->set_success( true );
				if( GetValueFromJsonObject( rootValue , "result" , &val ) )
				{
					pRet->set_result( val.asUInt() );
				}
				if( GetValueFromJsonObject( rootValue , "total_count" , &val ) )
				{
					pRet->set_total_count( val.asUInt64() );
				}
				if( GetValueFromJsonObject( rootValue , "billboard_items" , &val ) && val.isArray() )
				{
					for( Json::Value::ArrayIndex i = 0; i < val.size(); ++i )
					{
						const Json::Value &vItem = val[i];
						CFCGetBillBoardResponse_billboard_item *bitem = pRet->add_items();
						const Json::Value &vField = vItem["group_id"];	
						if( vField.isUInt64() )
							bitem->set_group_id( vField.asUInt64() );
						const Json::Value &vField1 = vItem["group_name"];	
						if( vField1.isString() )
							bitem->set_group_name( vField1.asString() );
						const Json::Value &vField2 = vItem["task_name"];	
						if( vField2.isString() )
							bitem->set_task_name( vField2.asString() );
						const Json::Value &vField3 = vItem["task_attr"];	
						if( vField3.isString() )
							bitem->set_task_attr( vField3.asString() );
						const Json::Value &vField4 = vItem["task_desc"];	
						if( vField4.isString() )
							bitem->set_task_desc( vField4.asString() );
						const Json::Value &vField5 = vItem["task_tags"];	
						if( vField5.isString() )
							bitem->set_task_tags( vField5.asString() );
						const Json::Value &vField6 = vItem["class_id"];	
						if( vField6.isUInt() )
							bitem->set_class_id( vField6.asUInt() );
						const Json::Value &vField7 = vItem["access_right"];	
						if( vField7.isUInt() )
							bitem->set_access_right( vField7.asUInt() );
						const Json::Value &vField8 = vItem["create_time"];	
						if( vField8.isUInt64() )
							bitem->set_create_time( vField8.asUInt64() );
 						const Json::Value &vField9 = vItem["file_sig"];	
						if( vField9.isString() )
							bitem->set_file_sig( vField9.asString() );
 						const Json::Value &vFielda = vItem["good_score"];	
						if( vFielda.isUInt() )
							bitem->set_good_score( vFielda.asUInt() );
 						const Json::Value &vFieldb = vItem["bad_score"];	
						if( vFieldb.isUInt() )
							bitem->set_bad_score( vFieldb.asUInt() );
						const Json::Value &vFieldc = vItem["down_count"];	
						if( vFieldc.isUInt64() )
							bitem->set_down_count( vFieldc.asUInt64() );
						const Json::Value &vFieldd = vItem["total_size"];	
						if( vFieldd.isUInt64() )
							bitem->set_total_size( vFieldd.asUInt64() );
 						const Json::Value &vFielde = vItem["file_count"];	
						if( vFielde.isUInt() )
							bitem->set_file_count( vFielde.asUInt() );
 						const Json::Value &vFieldf = vItem["path_count"];	
						if( vFieldf.isUInt() )
							bitem->set_path_count( vFieldf.asUInt() );
   						const Json::Value &vFieldg = vItem["file_type"];	
						if( vFieldg.isUInt() )
							bitem->set_file_type( vFieldg.asUInt() );
					}
				}
			}
		}
	}
	theCallback.OnRecvXEPCommand( ret.ToBuffer( 1024 ) );
}
*/