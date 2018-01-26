#pragma once
#include "AsyncFuncCall.h"
#include <deque>
class IAsyncFuncsDispatcher
{
protected:
	IAsyncFuncsDispatcher(){}
public:
	virtual ~IAsyncFuncsDispatcher(){}

public:
    virtual bool AddOneAsyncFunc( talk_base::IAsyncFuncCall *pAsyncFunc ) = 0;
    virtual void DispathOneAsyncFunc() = 0;
    virtual void ClearAllAsyncFuncs() = 0;
    virtual int GetAsyncFuncCount() = 0;

public:
     template <typename PointerToObj , typename PointerToMemFunc > void RemoveAsyncFuncs( PointerToMemFunc pMemFunc , PointerToObj pObj )
    {
        void *pObjTmp = reinterpret_cast<void*>( pObj );
        void *pMemFuncTmp = reinterpret_cast<void*>( &pMemFunc );
		RemoveAsyncFuncsImpl( pMemFuncTmp , pObjTmp );
    }

    template <typename PointerToObj > void RemoveAsyncFuncs( PointerToObj pObj )
    {
        void *pObjTmp = reinterpret_cast<void*>( pObj );
		RemoveAsyncFuncsImpl( pObjTmp );
    }

protected:
    virtual void RemoveAsyncFuncsImpl( void *pMemFunc , void* pObj ) = 0;
    virtual void RemoveAsyncFuncsImpl( void* pObj ) = 0;


};



template< class MUTEX >
class CAsyncFuncDispatcherT:
	public IAsyncFuncsDispatcher
{
public:
	typedef MUTEX MUTEXTYPE;
public:
    CAsyncFuncDispatcherT()
    {

    }

    virtual ~CAsyncFuncDispatcherT()
    {
        ClearAllAsyncFuncs();
    }

    virtual bool AddOneAsyncFunc( talk_base::IAsyncFuncCall *pAsyncFunc )
    {
        try
        {
            ATLASSERT( pAsyncFunc != NULL );
            CComCritSecLock<MUTEXTYPE> guard( m_csObj );
            m_lstAsyncFunc.push_back( std::tr1::shared_ptr<talk_base::IAsyncFuncCall>( pAsyncFunc ) );

//			ATLTRACE( _T("\r\nAddOneAsyncFunc Excute!! ptr = %08x , timestamp = %d , threadid = %d" ) , (DWORD)pAsyncFunc , GetTickCount() , GetCurrentThreadId() );
       
		}
        catch ( ... )
        {

        }
        return true;
    }

    virtual void DispathOneAsyncFunc()
    {
        try
        {
            std::tr1::shared_ptr<talk_base::IAsyncFuncCall> func;
            if ( true )
            {
                CComCritSecLock<MUTEXTYPE> guard( m_csObj );
                if ( !m_lstAsyncFunc.empty() )
                {
                    func = m_lstAsyncFunc.front();
                    m_lstAsyncFunc.pop_front();
                }
            }

            if ( func.get() != NULL )
            {
//				ATLTRACE( _T("\r\nDispathOneAsyncFunc Excute!! ptr = %08x , timestamp = %d , threadid = %d") , (DWORD)func.get() , GetTickCount() , GetCurrentThreadId() );
				func->Execute();
            }
        }
        catch ( ... )
        {
        }
    }

    virtual void ClearAllAsyncFuncs()
    {
        try
        {
            CComCritSecLock<MUTEXTYPE> guard( m_csObj );
            m_lstAsyncFunc.clear();
        }
        catch ( ... )
        {

        }
    }

    virtual int GetAsyncFuncCount()
    {
        int nRet = 0;
        try
        {
            CComCritSecLock<MUTEXTYPE> guard( m_csObj );
            nRet = m_lstAsyncFunc.size();
        }
        catch ( ... )
        {

        }
        return nRet;
    }

	MUTEX& GetMutex()
	{
		return m_csObj;
	}
//can't copy
private:
    CAsyncFuncDispatcherT( const CAsyncFuncDispatcherT &other );
    CAsyncFuncDispatcherT& operator=( const CAsyncFuncDispatcherT &other );

protected:
    class CAsyncDelFunctor
    {
    private:
        CAsyncDelFunctor();
        CAsyncDelFunctor( void *pObj , void *pMemFunc )
                : _pObj( pObj ),
                _pMemFunc( pMemFunc )
        {
            ATLASSERT( pMemFunc != NULL );
        }

        CAsyncDelFunctor( void *pObj  )
                : _pObj( pObj ),
                _pMemFunc( NULL )
        {

        }
    public:
		bool operator()( const std::tr1::shared_ptr<talk_base::IAsyncFuncCall> &asyncItem ) const
        {
            if ( _pMemFunc != NULL )
            {
                if ( ( *( char** )_pMemFunc ) == ( *( char** )asyncItem->GetMemberFunctionPointer() ) && _pObj == asyncItem->GetObjectPointer() )
                    return true;
            }
            else
            {
                if ( _pObj == asyncItem->GetObjectPointer() )
                    return true;
            }
            return false;
        }

    public:
        ~CAsyncDelFunctor() {}
        friend class CAsyncFuncDispatcherT;
    private:
        void *_pObj;
        void *_pMemFunc;

    };


protected:
    virtual void RemoveAsyncFuncsImpl( void *pMemFuncTmp , void* pObjTmp )
    {
        try
        {
            CComCritSecLock<MUTEXTYPE> guard( m_csObj );
            std::deque< std::tr1::shared_ptr<talk_base::IAsyncFuncCall> >::iterator endnew = m_lstAsyncFunc.end();
            endnew = std::remove_if( m_lstAsyncFunc.begin() , m_lstAsyncFunc.end() , CAsyncDelFunctor( pObjTmp , pMemFuncTmp ) );
            m_lstAsyncFunc.erase( endnew , m_lstAsyncFunc.end() );
        }
        catch ( ... )
        {

        }
    }

    virtual void RemoveAsyncFuncsImpl( void* pObjTmp )
    {
        try
        {
            CComCritSecLock<MUTEXTYPE> guard( m_csObj );
            std::deque< std::tr1::shared_ptr<talk_base::IAsyncFuncCall> >::iterator endnew = m_lstAsyncFunc.end();
            endnew = std::remove_if( m_lstAsyncFunc.begin() , m_lstAsyncFunc.end() , CAsyncDelFunctor( pObjTmp ) );
            m_lstAsyncFunc.erase( endnew , m_lstAsyncFunc.end() );
        }
        catch ( ... )
        {

        }
    }

protected:
    std::deque< std::tr1::shared_ptr<talk_base::IAsyncFuncCall> > m_lstAsyncFunc;
    MUTEX m_csObj;


};


typedef CAsyncFuncDispatcherT<CComAutoCriticalSection> CAsyncFuncDispatcher;
typedef CAsyncFuncDispatcherT<CComFakeCriticalSection> CAsyncFuncDispatcherNotThreadSafe;


