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
	std::unordered_map< std::string , BOOL >::iterator iter = running_requests_.find( request_id );
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

