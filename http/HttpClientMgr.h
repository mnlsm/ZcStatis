#ifndef XMPPXEP_HTTPCLIENTMGR_H__
#define XMPPXEP_HTTPCLIENTMGR_H__

#include "HttpClientItem.h"

class CHttpClientMgr:
	public talk_base::MessageHandler
{
public:
	friend class CHttpClientItem;
	CHttpClientMgr();
	virtual ~CHttpClientMgr();

public:
	BOOL Init();
	void Term();
	BOOL IsAbortAll();
	void AbortAll();
	BOOL DoHttpCommandRequest(const CHttpRequestPtr &cmd, std::string *request_id);

	BOOL DoHttpCommandRequest(const CHttpRequestPtr &cmd);

	static std::string generateRequestId();

protected:
	void AsyncInit();
	void AsyncTerm();
	void AsyncDoHttpCommandRequest(const CHttpRequestPtr &cmd, const std::string &request_id);
	void AsyncCheckHttpRequestTimeout( const std::string &request_id );
	void OnAsyncHttpRequestComplete(const CHttpResponseDataPtr &data);

public:
	BOOL IsInThreadPool();
	virtual void OnMessage( talk_base::Message* msg );


protected:
	enum
	{
		THREADCOUNT = 3
	};

private:
	BOOL DoAsyncHttpWork( talk_base::Thread *thread , talk_base::IAsyncFuncCall *pWork );
	BOOL DoDelayAsyncHttpWork( talk_base::Thread *thread , int timeout , talk_base::IAsyncFuncCall *pWork );
	BOOL DoAsyncSessionWork( talk_base::IAsyncFuncCall *pWork );
	BOOL DoDelayAsyncSessionWork( int timeout , talk_base::IAsyncFuncCall *pWork );

	void RemoveHttpCommandRequest( const std::string &identity );
	CHttpRequestPtr FindRequestCommand(const std::string &identity, BOOL bRemove);
	void DispatchHttpRequest(BOOL bNewThread, const CHttpRequestPtr &request_param);
	void DispatchHttpRequest_Http(const CHttpRequestPtr &request_param);

	void AbortHttpRequest( const std::string &request_id );
	BOOL IsHttpRequestAborted( const std::string &request_id );

	void AsyncCheckHttpClientStatus(CHttpClientItemPtr ptr);

private:
	talk_base::Thread session_thread_; 
	stdext::hash_map< std::string, CHttpRequestPtr > pend_requests_;
	stdext::hash_map< std::string , BOOL > running_requests_;

	talk_base::Thread thread_pool_[THREADCOUNT];
	DWORD pool_task_count_;

	CComAutoCriticalSection csObj_;
	ATL::CEvent eventThreadExited_;
	volatile BOOL abortall_;

	typedef std::map< std::string , std::tr1::shared_ptr< talk_base::Thread > > TEMP_THREAD; 
	TEMP_THREAD temp_threads_;

};

inline BOOL CHttpClientMgr::IsAbortAll()
{
	CComCritSecLock< CComCriticalSection > guard( csObj_ );
	return abortall_;
}

inline void CHttpClientMgr::AbortAll()
{
	CComCritSecLock< CComCriticalSection > guard( csObj_ );
	abortall_ = true;
}


#endif