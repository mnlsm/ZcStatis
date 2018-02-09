#include "stdafx.h"
#include "DanLueDialog.h"
#include "Global.h"


LRESULT DanLueDialog::OnBetAreaEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return TRUE;
}

LRESULT DanLueDialog::OnBetAreaPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CPaintDC dc(m_stBetArea.m_hWnd);
	CRect rcc;
	m_stBetArea.GetClientRect(rcc);
	CMemoryDC memDC(dc, rcc);
	memDC.FillSolidRect(rcc, RGB(0xFF, 0xFF, 0xFF));
	CBrush frameBrush;
	frameBrush.CreateSolidBrush(RGB(10, 10, 10));
	memDC.FrameRect(rcc, frameBrush);
	return 0;
}
