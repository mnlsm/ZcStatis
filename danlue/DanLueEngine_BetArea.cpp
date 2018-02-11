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
	rcc.DeflateRect(1, 1, 1, 1);
	int yPos = 1;
	DrawBetAreaTitle(memDC.m_hDC, rcc, yPos);
	DrawSPF(memDC.m_hDC, rcc, yPos);
	DrawJQZS(memDC.m_hDC, rcc, yPos);
	DrawBQC(memDC.m_hDC, rcc, yPos);
	DrawBF(memDC.m_hDC, rcc, yPos);
	m_FirstDrawBetArea = false;
	return 0;
}

void DanLueDialog::DrawBetAreaTitle(CDCHandle dc, const CRect& rcc, int& yTop) {
	int saveDC = dc.SaveDC();
	if (m_FirstDrawBetArea) {
		m_TitleDrawBetItem.rect = rcc;
		m_TitleDrawBetItem.rect.top = yTop;
		m_TitleDrawBetItem.rect.bottom = yTop + 30;
	}
	dc.FillSolidRect(m_TitleDrawBetItem.rect, RGB(0x66, 0x66, 0xEE));
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0x11, 0xFF, 0x11));
	if (m_CurrentMatchItem.get() != NULL) {
		dc.DrawText(m_CurrentMatchItem.get()->descrition.c_str(), m_CurrentMatchItem.get()->descrition.size(),
			m_TitleDrawBetItem.rect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
	}
	else {
		CStringATL text = "Î´ Ñ¡ Ôñ";
		dc.DrawText(text, text.GetLength(),
			m_TitleDrawBetItem.rect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
	}
	yTop = yTop + m_TitleDrawBetItem.rect.Height() + 16;
	dc.RestoreDC(saveDC);
}

void DanLueDialog::DrawSPF(CDCHandle dc, const CRect& rcc, int& yTop) {
	int saveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	int itemWidth = 80;
	int itemHeight = 40;
	CPen penLine;
	penLine.CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	dc.SelectPen(penLine);
	int left = rcc.CenterPoint().x - (itemWidth * 3 / 2 + 2);
	int top = yTop;
	dc.MoveTo(left, top);
	dc.LineTo(left + 3 * (itemWidth + 1) , top);
	dc.MoveTo(left, top + (itemHeight + 1));
	dc.LineTo(left + 3 * (itemWidth + 1), top + (itemHeight + 1));
	dc.MoveTo(left, top + 2 * (itemHeight + 1));
	dc.LineTo(left + 3 * (itemWidth + 1), top + 2 * (itemHeight + 1));

	dc.MoveTo(left, top);
	dc.LineTo(left, top + 2 * (itemHeight + 1));
	dc.MoveTo(left + 1 * (itemWidth + 1), top);
	dc.LineTo(left + 1 * (itemWidth + 1), top + 2 * (itemHeight + 1));
	dc.MoveTo(left + 2 * (itemWidth + 1), top);
	dc.LineTo(left + 2 * (itemWidth + 1), top + 2 * (itemHeight + 1));
	dc.MoveTo(left + 3 * (itemWidth + 1), top);
	dc.LineTo(left + 3 * (itemWidth + 1), top + 2 * (itemHeight + 1));

	if (m_FirstDrawBetArea) {
		for (int i = 0; i < 2; i++) {
			int l = left + i * (itemWidth + 1);
			int r = l + itemWidth;
			for (int j = 0; j < 3; j++) {
				int t = top + j * (itemHeight + 1);
				int b = t + itemHeight;
				m_SPFDrawBetItems[i * 3 + j].rect.SetRect(l, t, r, b);
			}
		}
	}
	
	if (m_CurrentMatchItem.get() != NULL) {
		JCMatchItem::Subject* subject = NULL;
		for (int i = 0; i < 6; i++) {
			CRect rcItem = m_SPFDrawBetItems[i].rect;
			if (i == 0) {
				subject = m_CurrentMatchItem->get_subject(6, 3);
			}
			else if (i == 1) {
				subject = m_CurrentMatchItem->get_subject(6, 1);
			}
			else if (i == 2) {
				subject = m_CurrentMatchItem->get_subject(6, 0);
			}
			else if (i == 3) {
				subject = m_CurrentMatchItem->get_subject(1, 3);
			}
			else if (i == 4) {
				subject = m_CurrentMatchItem->get_subject(1, 1);
			}
			else if (i == 5) {
				subject = m_CurrentMatchItem->get_subject(1, 0);
			}
			if (subject != NULL) {
				if (subject->checked) {
					dc.FillSolidRect(rcc, RGB(0x7F, 0x00, 0x00));
					dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
				}
				CRect tipRect = rcItem;
				tipRect.bottom = rcItem.CenterPoint().y;
				dc.DrawText(subject->tip.c_str(), subject->tip.size(),
					tipRect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
				CStringATL strOdds;
				strOdds.Format("%.2f", subject->odds);
				CRect oddsRect = rcItem;
				oddsRect.top = rcItem.CenterPoint().y;
				dc.DrawText(strOdds, strOdds.GetLength(),
					oddsRect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
			}
		}
	}
	dc.RestoreDC(saveDC);

	yTop = yTop + 2 * (itemHeight + 1) + 20;
}

void DanLueDialog::DrawJQZS(CDCHandle dc, const CRect& rcc, int& yTop) {
	int saveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	int itemWidth = 80;
	int itemHeight = 40;
	CPen penLine;
	penLine.CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	dc.SelectPen(penLine);
	int left = rcc.CenterPoint().x - (itemWidth + 1) * 2;
	int top = yTop;
	dc.MoveTo(left, top);
	dc.LineTo(left + 4 * (itemWidth + 1), top);
	dc.MoveTo(left, top + (itemHeight + 1));
	dc.LineTo(left + 4 * (itemWidth + 1), top + (itemHeight + 1));
	dc.MoveTo(left, top + 2 * (itemHeight + 1));
	dc.LineTo(left + 4 * (itemWidth + 1), top + 2 * (itemHeight + 1));

	dc.MoveTo(left, top);
	dc.LineTo(left, top + 2 * (itemHeight + 1));
	dc.MoveTo(left + 1 * (itemWidth + 1), top);
	dc.LineTo(left + 1 * (itemWidth + 1), top + 2 * (itemHeight + 1));
	dc.MoveTo(left + 2 * (itemWidth + 1), top);
	dc.LineTo(left + 2 * (itemWidth + 1), top + 2 * (itemHeight + 1));
	dc.MoveTo(left + 3 * (itemWidth + 1), top);
	dc.LineTo(left + 3 * (itemWidth + 1), top + 2 * (itemHeight + 1));
	dc.MoveTo(left + 4 * (itemWidth + 1), top);
	dc.LineTo(left + 4 * (itemWidth + 1), top + 2 * (itemHeight + 1));

	if (m_FirstDrawBetArea) {
		for (int i = 0; i < 2; i++) {
			int l = left + i * (itemWidth + 1);
			int r = l + itemWidth;
			for (int j = 0; j < 4; j++) {
				int t = top + j * (itemHeight + 1);
				int b = t + itemHeight;
				m_JQZSDrawBetItems[i * 4 + j].rect.SetRect(l, t, r, b);
			}
		}
	}

	if (m_CurrentMatchItem.get() != NULL) {
		JCMatchItem::Subject* subject = NULL;
		for (int i = 0; i < 8; i++) {
			CRect rcItem = m_JQZSDrawBetItems[i].rect;
			if (i == 0) {
				subject = m_CurrentMatchItem->get_subject(2, 0);
			}
			else if (i == 1) {
				subject = m_CurrentMatchItem->get_subject(2, 1);
			}
			else if (i == 2) {
				subject = m_CurrentMatchItem->get_subject(2, 2);
			}
			else if (i == 3) {
				subject = m_CurrentMatchItem->get_subject(2, 3);
			}
			else if (i == 4) {
				subject = m_CurrentMatchItem->get_subject(2, 4);
			}
			else if (i == 5) {
				subject = m_CurrentMatchItem->get_subject(2, 5);
			}
			else if (i == 6) {
				subject = m_CurrentMatchItem->get_subject(2, 6);
			}
			else if (i == 7) {
				subject = m_CurrentMatchItem->get_subject(2, 7);
			}
			if (subject != NULL) {
				if (subject->checked) {
					dc.FillSolidRect(rcc, RGB(0x7F, 0x00, 0x00));
					dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
				}
				CRect tipRect = rcItem;
				tipRect.bottom = rcItem.CenterPoint().y;
				dc.DrawText(subject->tip.c_str(), subject->tip.size(),
					tipRect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
				CStringATL strOdds;
				strOdds.Format("%.2f", subject->odds);
				CRect oddsRect = rcItem;
				oddsRect.top = rcItem.CenterPoint().y;
				dc.DrawText(strOdds, strOdds.GetLength(),
					oddsRect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
			}
		}
	}
	dc.RestoreDC(saveDC);
	yTop = yTop + 2 * (itemHeight + 1) + 20;

}

void DanLueDialog::DrawBQC(CDCHandle dc, const CRect& rcc, int& yTop) {
	int saveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	int itemWidth = 80;
	int itemHeight = 40;
	CPen penLine;
	penLine.CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	dc.SelectPen(penLine);
	int left = rcc.CenterPoint().x - (itemWidth * 3 / 2 + 2);
	int top = yTop;
	dc.MoveTo(left, top);
	dc.LineTo(left + 3 * (itemWidth + 1), top);
	dc.MoveTo(left, top + (itemHeight + 1));
	dc.LineTo(left + 3 * (itemWidth + 1), top + (itemHeight + 1));
	dc.MoveTo(left, top + 2 * (itemHeight + 1));
	dc.LineTo(left + 3 * (itemWidth + 1), top + 2 * (itemHeight + 1));
	dc.MoveTo(left, top + 3 * (itemHeight + 1));
	dc.LineTo(left + 3 * (itemWidth + 1), top + 3 * (itemHeight + 1));

	dc.MoveTo(left, top);
	dc.LineTo(left, top + 3 * (itemHeight + 1));
	dc.MoveTo(left + 1 * (itemWidth + 1), top);
	dc.LineTo(left + 1 * (itemWidth + 1), top + 3 * (itemHeight + 1));
	dc.MoveTo(left + 2 * (itemWidth + 1), top);
	dc.LineTo(left + 2 * (itemWidth + 1), top + 3 * (itemHeight + 1));
	dc.MoveTo(left + 3 * (itemWidth + 1), top);
	dc.LineTo(left + 3 * (itemWidth + 1), top + 3 * (itemHeight + 1));

	if (m_FirstDrawBetArea) {
		for (int i = 0; i < 3; i++) {
			int l = left + i * (itemWidth + 1);
			int r = l + itemWidth;
			for (int j = 0; j < 3; j++) {
				int t = top + j * (itemHeight + 1);
				int b = t + itemHeight;
				m_BQCDrawBetItems[i * 3 + j].rect.SetRect(l, t, r, b);
			}
		}
	}

	if (m_CurrentMatchItem.get() != NULL) {
		JCMatchItem::Subject* subject = NULL;
		for (int i = 0; i < 9; i++) {
			CRect rcItem = m_BQCDrawBetItems[i].rect;
			if (i == 0) {
				subject = m_CurrentMatchItem->get_subject(4, 0);
			}
			else if (i == 1) {
				subject = m_CurrentMatchItem->get_subject(4, 1);
			}
			else if (i == 2) {
				subject = m_CurrentMatchItem->get_subject(4, 2);
			}
			else if (i == 3) {
				subject = m_CurrentMatchItem->get_subject(4, 3);
			}
			else if (i == 4) {
				subject = m_CurrentMatchItem->get_subject(4, 4);
			}
			else if (i == 5) {
				subject = m_CurrentMatchItem->get_subject(4, 5);
			}
			else if (i == 6) {
				subject = m_CurrentMatchItem->get_subject(4, 6);
			}
			else if (i == 7) {
				subject = m_CurrentMatchItem->get_subject(4, 7);
			}
			else if (i == 8) {
				subject = m_CurrentMatchItem->get_subject(4, 8);
			}
			if (subject != NULL) {
				if (subject->checked) {
					dc.FillSolidRect(rcc, RGB(0x7F, 0x00, 0x00));
					dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
				}
				CRect tipRect = rcItem;
				tipRect.bottom = rcItem.CenterPoint().y;
				dc.DrawText(subject->tip.c_str(), subject->tip.size(),
					tipRect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
				CStringATL strOdds;
				strOdds.Format("%.2f", subject->odds);
				CRect oddsRect = rcItem;
				oddsRect.top = rcItem.CenterPoint().y;
				dc.DrawText(strOdds, strOdds.GetLength(),
					oddsRect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
			}
		}
	}
	dc.RestoreDC(saveDC);

	yTop = yTop + 3 * (itemHeight + 1) + 20;
}

void DanLueDialog::DrawBF(CDCHandle dc, const CRect& rcc, int& yTop) {
	int saveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	int itemWidth = 60;
	int itemHeight = 40;
	CPen penLine;
	penLine.CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	dc.SelectPen(penLine);
	int left = rcc.CenterPoint().x - (itemWidth * 7 / 2 + 3);
	int top = yTop;
	dc.MoveTo(left, top);
	dc.LineTo(left + 7 * (itemWidth + 1), top);
	dc.MoveTo(left, top + (itemHeight + 1));
	dc.LineTo(left + 7 * (itemWidth + 1), top + (itemHeight + 1));
	dc.MoveTo(left, top + 2 * (itemHeight + 1));
	dc.LineTo(left + 7 * (itemWidth + 1), top + 2 * (itemHeight + 1));
	dc.MoveTo(left, top + 3 * (itemHeight + 1));
	dc.LineTo(left + 7 * (itemWidth + 1), top + 3 * (itemHeight + 1));
	dc.MoveTo(left, top + 4 * (itemHeight + 1));
	dc.LineTo(left + 7 * (itemWidth + 1), top + 4 * (itemHeight + 1));
	dc.MoveTo(left, top + 5 * (itemHeight + 1));
	dc.LineTo(left + 7 * (itemWidth + 1), top + 5 * (itemHeight + 1));



	dc.MoveTo(left, top);
	dc.LineTo(left, top + 5 * (itemHeight + 1));
	dc.MoveTo(left + 1 * (itemWidth + 1), top);
	dc.LineTo(left + 1 * (itemWidth + 1), top + 5 * (itemHeight + 1));
	dc.MoveTo(left + 2 * (itemWidth + 1), top);
	dc.LineTo(left + 2 * (itemWidth + 1), top + 5 * (itemHeight + 1));
	dc.MoveTo(left + 3 * (itemWidth + 1), top);
	dc.LineTo(left + 3 * (itemWidth + 1), top + 5 * (itemHeight + 1));
	dc.MoveTo(left + 4 * (itemWidth + 1), top);
	dc.LineTo(left + 4 * (itemWidth + 1), top + 5 * (itemHeight + 1));
	dc.MoveTo(left + 5 * (itemWidth + 1), top);
	dc.LineTo(left + 5 * (itemWidth + 1), top + 5 * (itemHeight + 1));
	dc.MoveTo(left + 6 * (itemWidth + 1), top);
	dc.LineTo(left + 6 * (itemWidth + 1), top + 5 * (itemHeight + 1));
	dc.MoveTo(left + 7 * (itemWidth + 1), top);
	dc.LineTo(left + 7 * (itemWidth + 1), top + 5 * (itemHeight + 1));

	
	if (m_FirstDrawBetArea) {
		for (int i = 0; i < 5; i++) {
			int l = left + i * (itemWidth + 1);
			int r = l + itemWidth;
			for (int j = 0; j < 7; j++) {
				int t = top + j * (itemHeight + 1);
				int b = t + itemHeight;
				if (i * 7 + j < 32) {
					m_BFDrawBetItems[i * 7 + j].rect.SetRect(l, t, r, b);
				}
			}
		}
	}
	
	if (m_CurrentMatchItem.get() != NULL) {
		JCMatchItem::Subject* subject = NULL;
		for (int i = 0; i < 32; i++) {
			CRect rcItem = m_BQCDrawBetItems[i].rect;
			subject = m_CurrentMatchItem->get_subject(4, i);
			if (subject != NULL) {
				if (subject->checked) {
					dc.FillSolidRect(rcc, RGB(0x7F, 0x00, 0x00));
					dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
				}
				CRect tipRect = rcItem;
				tipRect.bottom = rcItem.CenterPoint().y;
				dc.DrawText(subject->tip.c_str(), subject->tip.size(),
					tipRect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
				CStringATL strOdds;
				strOdds.Format("%.2f", subject->odds);
				CRect oddsRect = rcItem;
				oddsRect.top = rcItem.CenterPoint().y;
				dc.DrawText(strOdds, strOdds.GetLength(),
					oddsRect, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX);
			}
		}
	}
	
	dc.RestoreDC(saveDC);
	yTop = yTop + 2 * (itemHeight + 1) + 20;

}