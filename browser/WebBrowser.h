#include "stdafx.h"
#include <comdef.h>
#include <Exdisp.h>
#include <string>
#include <tchar.h>
#include <Windows.h>

#include <exdisp.h>
#include <exdispid.h>
#include <mshtml.h>
#include <mshtmdid.h>

using namespace std;

class WebBrowserEvent;
#include <mshtmhst.h>

class CNoScriptErrorHelper
    : public IDocHostUIHandler
    , public IOleCommandTarget
{

public:
    HRESULT STDMETHODCALLTYPE SetHandler(CComPtr<IWebBrowser2> spWebBrowser)
    {
        HRESULT result = E_NOINTERFACE;
        CComPtr<IDispatch> spDisp;
        HRESULT hr = spWebBrowser->get_Document(&spDisp);
        if (SUCCEEDED(hr) && spDisp)
        {
            // If this is not an HTML document (e.g., it's a Word doc or a PDF), don't sink.
            CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> spHTML(spDisp);
            if (spHTML)
            {
                // Get pointers to default interfaces
                CComQIPtr<IOleObject, &IID_IOleObject> spOleObject(spDisp);
                if (spOleObject)
                {
                    CComPtr<IOleClientSite> spClientSite;
                    hr = spOleObject->GetClientSite(&spClientSite);
                    if (SUCCEEDED(hr) && spClientSite)
                    {
                        m_spDefaultDocHostUIHandler = spClientSite;
                        m_spDefaultOleCommandTarget = spClientSite;
                    }
                }

                // Set this class to be the IDocHostUIHandler
                CComQIPtr<ICustomDoc, &IID_ICustomDoc> spCustomDoc(spDisp);
                if (spCustomDoc)
                {
                    spCustomDoc->SetUIHandler(this);
                    result = S_OK;
                }
            }
        }
        return result;
    }
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
    {
        HRESULT result = S_OK;

        if (IsBadWritePtr(ppvObject, sizeof(LPVOID)))
            result = E_INVALIDARG;

        if (result == S_OK)
        {
            *ppvObject = NULL;

            if (IsEqualIID(riid, IID_IUnknown))
                *ppvObject = this;
            else if (IsEqualIID(riid, IID_IDispatch))
                *ppvObject = (IDispatch*)this;
            else if (IsEqualIID(riid, IID_IDocHostUIHandler))
                *ppvObject = (IDocHostUIHandler*)this;
            else if (IsEqualIID(riid, IID_IOleCommandTarget))
                *ppvObject = (IOleCommandTarget*)this;
            else
                result = E_NOINTERFACE;
        }

        if (result == S_OK)
            this->AddRef();

        return result;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        InterlockedIncrement(&m_cRef);
        return m_cRef;
    }

    ULONG STDMETHODCALLTYPE Release() {
        ULONG ulRefCount = InterlockedDecrement(&m_cRef);
        if (m_cRef == 0)
        {
            delete this;
        }
        return ulRefCount;
    }


    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdTarget, IDispatch FAR* pdispReserved)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->ShowContextMenu(dwID, ppt, pcmdTarget, pdispReserved);
        return S_FALSE;
    }

    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject FAR* pActiveObject,
        IOleCommandTarget FAR* pCommandTarget,
        IOleInPlaceFrame  FAR* pFrame,
        IOleInPlaceUIWindow FAR* pDoc)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
        return S_FALSE;
    }

    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO FAR* pInfo)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->GetHostInfo(pInfo);
        return S_OK;
    }

    STDMETHOD(HideUI)(void)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->HideUI();
        return S_OK;
    }

    STDMETHOD(UpdateUI)(void)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->UpdateUI();
        return S_OK;
    }

    STDMETHOD(EnableModeless)(BOOL fEnable)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->EnableModeless(fEnable);
        return S_OK;
    }

    STDMETHOD(OnDocWindowActivate)(BOOL fActivate)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->OnDocWindowActivate(fActivate);
        return S_OK;
    }

    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->OnFrameWindowActivate(fActivate);
        return S_OK;
    }

    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow, BOOL fFrameWindow)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->ResizeBorder(prcBorder, pUIWindow, fFrameWindow);
        return S_OK;
    }

    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID FAR* pguidCmdGroup, DWORD nCmdID)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
        return E_NOTIMPL;
    }

    STDMETHOD(GetOptionKeyPath)(LPOLESTR FAR* pchKey, DWORD dw)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->GetOptionKeyPath(pchKey, dw);
        return E_FAIL;
    }

    STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->GetDropTarget(pDropTarget, ppDropTarget);
        return S_OK;
    }

    STDMETHOD(GetExternal)(IDispatch** ppDispatch)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->GetExternal(ppDispatch);
        return S_FALSE;
    }

    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
        return S_FALSE;
    }

    STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)
    {
        if (m_spDefaultDocHostUIHandler)
            return m_spDefaultDocHostUIHandler->FilterDataObject(pDO, ppDORet);
        return S_FALSE;
    }

    //
    // IOleCommandTarget
    //
    STDMETHOD(QueryStatus)(
        /*[in]*/ const GUID* pguidCmdGroup,
        /*[in]*/ ULONG cCmds,
        /*[in,out][size_is(cCmds)]*/ OLECMD* prgCmds,
        /*[in,out]*/ OLECMDTEXT* pCmdText)
    {
        return m_spDefaultOleCommandTarget->QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);
    }
    STDMETHOD(Exec)(
        /*[in]*/ const GUID* pguidCmdGroup,
        /*[in]*/ DWORD nCmdID,
        /*[in]*/ DWORD nCmdExecOpt,
        /*[in]*/ VARIANTARG* pvaIn,
        /*[in,out]*/ VARIANTARG* pvaOut)
    {
        if (nCmdID == OLECMDID_SHOWSCRIPTERROR)
        {
            // Don't show the error dialog, but
            // continue running scripts on the page.
            (*pvaOut).vt = VT_BOOL;
            (*pvaOut).boolVal = VARIANT_TRUE;
            return S_OK;
        }
        return m_spDefaultOleCommandTarget->Exec(pguidCmdGroup, nCmdID, nCmdExecOpt, pvaIn, pvaOut);
    }
private:
    // Default interface pointers
    CComQIPtr<IDocHostUIHandler, &IID_IDocHostUIHandler> m_spDefaultDocHostUIHandler;
    CComQIPtr<IOleCommandTarget, &IID_IOleCommandTarget> m_spDefaultOleCommandTarget;
    LONG m_cRef;
};




const UINT ID_WebBrowser = 1;

typedef CWinTraits < WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN > CWebWinTraits;

class IWebBrowserCallback {
public:
    virtual void onWebBrowserClose(const std::string& url) = 0;
};

class WebBrowser :
    public CWindowImpl<WebBrowser, CWindow, CWebWinTraits>,
	public CMessageFilter,
    public IDispEventImpl<ID_WebBrowser, WebBrowser> {

protected:
	WebBrowser();

public:
	static std::shared_ptr<WebBrowser> CreateWebBrowser(HWND parent,
		const std::string& title, const std::string& url, IWebBrowserCallback* cb);

	BOOL IsWindowDestroyed() {
		return !m_wndCreated;
	}

	BOOL IsWindowDestroying() {
		if (!IsWindow()) {
			return TRUE;
		}
		return m_wndDestroying;
	}

protected:


	CComPtr<IWebBrowser2> GetWebBrowser2() {
		return m_spIWebBrowser;
	}

	void Navigate2(const std::string& szUrl) {
		bstr_t url(szUrl.c_str());
		_variant_t flags((long)(navNoHistory | navNoReadFromCache), VT_I4); //navNoHistory
		this->m_spIWebBrowser->Navigate(url, &flags, 0, 0, 0);
	}

public:
    DECLARE_WND_CLASS(_T("WTL ActiveX host window"))
    //Windows message handler
    BEGIN_MSG_MAP(WebBrowser)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
    END_MSG_MAP()

    //WebBrowser control event message handler
    BEGIN_SINK_MAP(WebBrowser)
		SINK_ENTRY(ID_WebBrowser, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
		SINK_ENTRY(ID_WebBrowser, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
    END_SINK_MAP()



	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


	virtual void __stdcall OnDocumentComplete(IDispatch *pDisp, VARIANT* url) {
        m_helper.SetHandler(m_spIWebBrowser);
    }
	virtual void __stdcall OnBeforeNavigate2(
		IDispatch* pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel) {}

	virtual BOOL PreTranslateMessage(MSG* pMsg){ return CWindow::IsDialogMessage(pMsg); }

protected:
    CAxWindow m_ax;
    CComPtr<IWebBrowser2> m_spIWebBrowser;
	BOOL m_wndCreated;
	BOOL m_wndDestroying;

    CNoScriptErrorHelper m_helper;
    IWebBrowserCallback* m_cb;
    std::string m_url;
};


class IHtmlElementEventHandler {
public:
	virtual void OnHtmlElementEvent(const CComPtr<IHTMLElement>& element) = 0;
};

class HtmlElementEvent
	: public IDispatch {
	
public:
	HtmlElementEvent() { dwCookie_ = 0; }

public:
	bool RegisterEvents(CComPtr<IHTMLElement> element, IHtmlElementEventHandler* callback);
	void UnRegisterEvents();
	bool IsRegistered();
	CComPtr<IHTMLElement> getHTMLElement();

public:
	virtual ULONG STDMETHODCALLTYPE AddRef(void) {
		return 1;
	}  
	virtual ULONG STDMETHODCALLTYPE Release(void) {
		return 1;
	} 
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(const struct _GUID &iid, void ** ppv) {
		if (iid == IID_IUnknown)
			*ppv = static_cast <IUnknown *>(this);
		else if (iid == IID_IDispatch)
			*ppv = static_cast <IDispatch *>(this);
		else
			return E_NOINTERFACE;
		AddRef();
		return S_OK;
	}

public:
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo) {
		return E_NOTIMPL;
	}
	virtual HRESULT  STDMETHODCALLTYPE GetTypeInfo(
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo **ppTInfo) {
		return E_NOTIMPL;
	}
	virtual HRESULT  STDMETHODCALLTYPE GetIDsOfNames(
		/* [in] */ REFIID riid,
		/* [size_is][in] */ LPOLESTR *rgszNames,
		/* [in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ DISPID *rgDispId) {
		return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE Invoke(
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr);

private:
	CComPtr<IHTMLElement> spElement_;
	CComPtr <IConnectionPoint> spConnectionPoint_;  //Á¬½Óµã 
	DWORD dwCookie_;
	IHtmlElementEventHandler* callback_;
};



