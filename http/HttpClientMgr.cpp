#include "stdafx.h"
#include "HttpClientMgr.h"
//#include "utils/atlcrypt.h"
#include "libjingle/base/base64.h"
#include "MiscHelper.h"

#define HTTPWORK_ID 1
#define SESSIONWORK_ID 2

using namespace talk_base;

CHttpClientMgr::CHttpClientMgr()
:pool_task_count_( 0 )
{
	abortall_ = false;
}

CHttpClientMgr::~CHttpClientMgr()
{
	
}

void CHttpClientMgr::OnMessage( talk_base::Message* msg )
{
	if( msg->message_id == HTTPWORK_ID )
	{
		IAsyncFuncCallDataPtr *pMsgData = static_cast< IAsyncFuncCallDataPtr * >( msg->pdata );
		if( pMsgData != NULL && pMsgData->Ptr() != NULL )
		{
			pMsgData->Ptr()->Execute();
			msg->pdata = NULL;
			delete pMsgData;
		}	
	}
	else if( msg->message_id == SESSIONWORK_ID )
	{
		IAsyncFuncCallDataPtr *pMsgData = static_cast< IAsyncFuncCallDataPtr * >( msg->pdata );
		if( pMsgData != NULL && pMsgData->Ptr() != NULL )
		{
			pMsgData->Ptr()->Execute();
			msg->pdata = NULL;
			delete pMsgData;
		}
	}
}

BOOL CHttpClientMgr::DoAsyncSessionWork( talk_base::IAsyncFuncCall *pWork )
{
	if( !session_thread_.started() || session_thread_.IsQuitting() || pWork == NULL )
		return FALSE;
	session_thread_.Post( this , SESSIONWORK_ID , new (std::nothrow) IAsyncFuncCallDataPtr( pWork ) , false );
	return TRUE;
}

BOOL CHttpClientMgr::DoDelayAsyncSessionWork( int timeout , talk_base::IAsyncFuncCall *pWork )
{
	if( !session_thread_.started() || session_thread_.IsQuitting() || pWork == NULL )
		return FALSE;
	session_thread_.PostDelayed( timeout , this , SESSIONWORK_ID , new (std::nothrow) IAsyncFuncCallDataPtr( pWork ) );
	return TRUE;
}


BOOL CHttpClientMgr::DoAsyncHttpWork(  Thread *thread , talk_base::IAsyncFuncCall *pWork )
{
	if( thread == NULL || pWork == NULL )
		return FALSE;
	if( !thread->started() || thread->IsQuitting() )
		return FALSE;
	thread->Post( this , HTTPWORK_ID , new (std::nothrow) IAsyncFuncCallDataPtr( pWork ) , false );
	return TRUE;
}

BOOL CHttpClientMgr::DoDelayAsyncHttpWork( Thread *thread , int timeout , talk_base::IAsyncFuncCall *pWork )
{
	if( thread == NULL || pWork == NULL )
		return FALSE;
	if( !thread->started() || thread->IsQuitting() )
		return FALSE;
	thread->PostDelayed( timeout , this , HTTPWORK_ID , new (std::nothrow) IAsyncFuncCallDataPtr( pWork ) );
	return TRUE;
}


CHttpRequestPtr CHttpClientMgr::FindRequestCommand(const std::string &identity, BOOL bRemove)
{
	CHttpRequestPtr ptr;
	if( TRUE )
	{
		CComCritSecLock< CComCriticalSection > guard( csObj_ );
		stdext::hash_map< std::string, CHttpRequestPtr >::iterator iter = pend_requests_.find(identity);
		if( iter != pend_requests_.end() )
		{
			ptr = iter->second;
			if( bRemove )
				pend_requests_.erase( iter );
		}
	}
	return ptr;
}

void CHttpClientMgr::RemoveHttpCommandRequest( const std::string &identity )
{
	std::tr1::shared_ptr< talk_base::Thread > ptrThread;
	if( TRUE )
	{
		CComCritSecLock< CComCriticalSection > guard( csObj_ );
		TEMP_THREAD::iterator iter = temp_threads_.find( identity );
		if( iter != temp_threads_.end() )
		{
			ptrThread = iter->second;
			temp_threads_.erase( iter );
		}
	}
	if( ptrThread.get() != NULL )
	{
		Sleep( 20 );
		ptrThread->Stop();
	}
	CComCritSecLock< CComCriticalSection > guard( csObj_ );
	pend_requests_.erase( identity );
	running_requests_.erase( identity );
}


BOOL CHttpClientMgr::IsInThreadPool()
{
	for( UINT i = 0 ; i < THREADCOUNT ; i++ )
	{
		if( talk_base::ThreadManager::Instance()->CurrentThread() == &thread_pool_[i] )
			return TRUE;
	}
	return FALSE;
}

BOOL CHttpClientMgr::Init()
{
	if( !eventThreadExited_.Create( NULL , TRUE , FALSE , NULL ) ) 
		return FALSE;	
	if( !session_thread_.started() )
	{
		if( !session_thread_.Start() )
			return FALSE;
	}
	return DoAsyncSessionWork( BindAsyncFunc( &CHttpClientMgr::AsyncInit , this ) );
}

void CHttpClientMgr::Term()
{
	if( TRUE )
	{
		CComCritSecLock< CComCriticalSection > guard( csObj_ );
		abortall_ = TRUE;
		Sleep( 10 );
	}
	DoAsyncSessionWork( BindAsyncFunc( &CHttpClientMgr::AsyncTerm , this ) );
	Sleep( 0 );
	WaitForSingleObject( eventThreadExited_.m_h , 10000 ) ;
	session_thread_.Stop();	
}

BOOL CHttpClientMgr::DoHttpCommandRequest(const CHttpRequestPtr &cmd, std::string *request_id)
{
	if( cmd.get() == NULL ) return FALSE;
	std::string identity;
	identity.reserve( 64 );
	CMiscHelper::generateUUID( &identity );
	cmd->request_id = identity;
	if( request_id != NULL ) *request_id = identity;
	DoAsyncSessionWork( BindAsyncFunc( &CHttpClientMgr::AsyncDoHttpCommandRequest , this , cmd , identity ) );
	return TRUE;
}

BOOL CHttpClientMgr::DoHttpCommandRequest(const CHttpRequestPtr &cmd) {
	if (cmd.get() == NULL) return FALSE;
	DoAsyncSessionWork(BindAsyncFunc(&CHttpClientMgr::AsyncDoHttpCommandRequest, this, cmd, cmd->request_id));
	return TRUE;
}

std::string CHttpClientMgr::generateRequestId() {
	std::string identity;
	identity.reserve(64);
	CMiscHelper::generateUUID(&identity);
	return identity;
}


