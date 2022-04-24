#include "stdafx.h"
#include "WebBrowser.h"

static const char* const PAD_KEY_1 = "VIEN5418814524NA.AT";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CComPtr<IHTMLElement> FindHTMLElement(CComPtr<IHTMLDocument2> doc, const std::wstring& tagName,
	const std::map<std::wstring, CComVariant>& attrs) {
	CComPtr<IHTMLElement> result;
	CComPtr<IHTMLElementCollection> pCollec;
	doc->get_all(&pCollec);
	if (NULL == pCollec.p) {
		return result;
	}
	long len = 0;
	pCollec->get_length(&len);
	for (int i = 0; i < len; i++)
	{
		VARIANT varName;
		VariantInit(&varName);
		varName.vt = VT_I4;
		varName.llVal = i;
		CComPtr<IDispatch> pDisp;
		pCollec->item(varName, varName, &pDisp);
		VariantClear(&varName);
		if (NULL == pDisp.p) {
			continue;
		}
		CComPtr<IHTMLElement> element;
		pDisp->QueryInterface(IID_IHTMLElement, (LPVOID*)&element);
		if (NULL != element.p) {
			bool tagOK = false, attrsOK = false;;
			CComBSTR tagText;
			if (SUCCEEDED(element->get_tagName(&tagText))) {
				if (_wcsicmp(tagText.m_str, tagName.c_str()) == 0) {
					tagOK = true;
				}
			}
			if (tagOK) {
				attrsOK = true;
				for (auto& attr : attrs) {
					CComBSTR attrName(attr.first.c_str());
					CComVariant a;
					element->getAttribute(attrName, 0, &a);
					if (a.vt != attr.second.vt) {
						attrsOK = false;
					}
					else {
						if (a.vt == VT_BSTR) {
							if (a.bstrVal == NULL || _wcsicmp(a.bstrVal, attr.second.bstrVal) != 0) {
								attrsOK = false;
							}
						}
						else if (a.vt == VT_I1 || a.vt == VT_I2 || a.vt == VT_I4 || a.vt == VT_I8) {
							if (a != attr.second) {
								attrsOK = false;
							}
						}
						else if (a.vt == VT_BOOL) {
							if (a.boolVal != attr.second.boolVal) {
								attrsOK = false;
							}
						}
						else {
							attrsOK = false;
						}
					}
					//attrsOK == (attrsOK) && (a == attr.second);
					if (!attrsOK) {
						break;
					}
				}
			}
			if (attrsOK && tagOK) {
				result = element;
				break;
			}
		}
	}
	return result;
}

CComPtr<IHTMLElement> FindHTMLElement(CComPtr<IHTMLDocument2> doc, LPCWSTR prefix) {
	CComPtr<IHTMLElement> result;
	CComPtr<IHTMLElementCollection> pCollec;
	doc->get_all(&pCollec);
	if (NULL == pCollec.p) {
		return result;
	}
	long len = 0;
	pCollec->get_length(&len);
	for (int i = 0; i < len; i++) {
		VARIANT varName;
		VariantInit(&varName);
		varName.vt = VT_I4;
		varName.llVal = i;
		CComPtr<IDispatch> pDisp;
		pCollec->item(varName, varName, &pDisp);
		VariantClear(&varName);
		if (NULL == pDisp.p) {
			continue;
		}
		CComPtr<IHTMLElement> element;
		pDisp->QueryInterface(IID_IHTMLElement, (LPVOID*)&element);
		if (NULL != element.p) {
			CComBSTR text;
			element->get_innerText(&text);
			if (text.Length() > 0 && wcsstr(text.m_str, prefix) == text.m_str) {
				result = element;
			}
		}
	}
	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WebBrowser::WebBrowser() {
	m_wndCreated = FALSE;
	m_wndDestroying = FALSE;
}

std::shared_ptr<WebBrowser> WebBrowser::CreateWebBrowser(
		HWND parent, const std::string& title, 
			const std::string& url, IWebBrowserCallback* cb) {
	std::shared_ptr<WebBrowser> result(new WebBrowser());
	CWindow parentWnd = parent;
	CRect rcWnd;
	parentWnd.GetWindowRect(rcWnd);
	int width = 460;
	CPoint cp = rcWnd.CenterPoint();
	rcWnd.left = cp.x - width / 2;
	rcWnd.right = cp.x + width / 2;
	CWindow wnd = result->Create(parent, rcWnd);
	if (!wnd.IsWindow()) {
		result.reset();
	} else {
		CStringA t = title.c_str();
		t.Trim();
		wnd.SetWindowText(t);
		result->Navigate2(url);
		result->m_cb = cb;
		result->m_url = url;
	}
	return result;
}

LRESULT WebBrowser::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	CRect rc;
	GetClientRect(&rc);
	CComPtr<IUnknown> spUnkWebBrowser;
	HRESULT hr = spUnkWebBrowser.CoCreateInstance(CLSID_WebBrowser);
	HWND hWndWB = m_ax.Create(
		m_hWnd,
		rc,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0,
		ID_WebBrowser);

	ATLASSERT(hWndWB);

	CComPtr<IUnknown> spUnkContainer;
	m_ax.AttachControl(spUnkWebBrowser, &spUnkContainer);
	AtlAdviseSinkMap(this, true);
	hr = m_ax.QueryControl(&m_spIWebBrowser);
	CenterWindow();
	m_wndCreated = TRUE;
	return 0;
}

LRESULT WebBrowser::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rc;
	GetClientRect(&rc);
	m_ax.SetWindowPos(m_hWnd, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
	return 1L;
}

LRESULT WebBrowser::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	m_wndDestroying = TRUE;
	DestroyWindow();
	if (m_cb != nullptr) {
		m_cb->onWebBrowserClose(m_url);
	}
	return 1L;
}

LRESULT WebBrowser::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = FALSE;
	m_wndDestroying = TRUE;
	AtlAdviseSinkMap(this, false);
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	return 1L;
}

LRESULT WebBrowser::OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = FALSE;
	m_wndCreated = TRUE;
	return 1L;
}

