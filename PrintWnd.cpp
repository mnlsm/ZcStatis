#include "stdafx.h"
#include "PrintWnd.h"
//-A2 -U -p -c -n $(ItemFileName)$(ItemExt)
CPrintWnd::CPrintWnd() {

}

CPrintWnd::~CPrintWnd() {

}

void CPrintWnd::Hide() {
	if (this->IsWindow()) {
		this->ShowWindow(SW_HIDE);
	} else {
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);
		this->Create(GetDesktopWindow(), CRect(0, 0, 10, 10),
			_T("Print"), 0, 0, (UINT)(HMENU)menu);
		if (this->IsWindow()) {
			this->ShowWindow(SW_HIDE);
		}
	}
}


LRESULT CPrintWnd::OnNext(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    NextPage();
    return 1L;
}

LRESULT CPrintWnd::OnPrev(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    PrevPage();
    return 1L;
}

