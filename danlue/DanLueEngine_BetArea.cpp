#include "stdafx.h"
#include "DanLueDialog.h"
#include "Global.h"


LRESULT DanLueDialog::OnBetAreaLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_stBetArea.DefWindowProc(uMsg, wParam, lParam);
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	if (m_CurrentMatchItem.get() != NULL) {
		DrawBetItem *pItem = NULL;
		for (auto& item : m_SPFDrawBetItems) {
			if (item.rect.PtInRect(pt)) {
				pItem = &item;
				break;
			}
		}
		if (pItem == NULL) {
			for (auto& item : m_JQZSDrawBetItems) {
				if (item.rect.PtInRect(pt)) {
					pItem = &item;
					break;
				}
			}
		}

		if (pItem == NULL) {
			for (auto& item : m_BQCDrawBetItems) {
				if (item.rect.PtInRect(pt)) {
					pItem = &item;
					break;
				}
			}
		}

		if (pItem == NULL) {
			for (auto& item : m_BFDrawBetItems) {
				if (item.rect.PtInRect(pt)) {
					pItem = &item;
					break;
				}
			}
		}

		if (pItem != NULL) {
			DanLueDialog::JCMatchItem::Subject* sub = m_CurrentMatchItem->get_subject(pItem->tid, pItem->betCode);
			if (sub != NULL) {
				sub->checked = !sub->checked;
				m_stBetArea.Invalidate();
				DoRefreshMatchListResults();
			}
		}
	}
	return lRet;
}


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
	memDC.SelectFont(GetFont());
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
		for (int j = 0; j < 3; j++) {
			int l = left + j * (itemWidth + 1);
			int r = l + itemWidth;
			for (int i = 0; i < 2; i++) {
				int t = top + i * (itemHeight + 1);
				int b = t + itemHeight;
				int index = i * 3 + j;
				m_SPFDrawBetItems[index].rect.SetRect(l, t, r, b);
				m_SPFDrawBetItems[index].rect.OffsetRect(1, 1);
				if (index == 0) {
					m_SPFDrawBetItems[index].tid = 6;
					m_SPFDrawBetItems[index].betCode = 3;
				}
				else if (index == 1) {
					m_SPFDrawBetItems[index].tid = 6;
					m_SPFDrawBetItems[index].betCode = 1;
				}
				else if (index == 2) {
					m_SPFDrawBetItems[index].tid = 6;
					m_SPFDrawBetItems[index].betCode = 0;
				}
				else if (index == 3) {
					m_SPFDrawBetItems[index].tid = 1;
					m_SPFDrawBetItems[index].betCode = 3;
				}
				else if (index == 4) {
					m_SPFDrawBetItems[index].tid = 1;
					m_SPFDrawBetItems[index].betCode = 1;
				}
				else if (index == 5) {
					m_SPFDrawBetItems[index].tid = 1;
					m_SPFDrawBetItems[index].betCode = 0;
				}
			}
		}
	}
	
	if (m_CurrentMatchItem.get() != NULL) {
		JCMatchItem::Subject* subject = NULL;
		for (int i = 0; i < 6; i++) {
			CRect rcItem = m_SPFDrawBetItems[i].rect;
			subject = m_CurrentMatchItem->get_subject(m_SPFDrawBetItems[i].tid, m_SPFDrawBetItems[i].betCode);
			if (subject != NULL) {
				if (subject->checked) {
					dc.FillSolidRect(rcItem, RGB(0x7F, 0x00, 0x00));
					dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
				}
				else {
					dc.SetTextColor(RGB(0x00, 0x00, 0x00));
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
		for (int j = 0; j < 4; j++) {
			int l = left + j * (itemWidth + 1);
			int r = l + itemWidth;
			for (int i = 0; i < 2; i++) {
				int t = top + i * (itemHeight + 1);
				int b = t + itemHeight;
				int index = i * 4 + j;
				m_JQZSDrawBetItems[index].rect.SetRect(l, t, r, b);
				m_JQZSDrawBetItems[index].rect.OffsetRect(1, 1);
				if (index == 0) {
					m_JQZSDrawBetItems[index].tid = 2;
					m_JQZSDrawBetItems[index].betCode = 0;
				}
				else if (index == 1) {
					m_JQZSDrawBetItems[index].tid = 2;
					m_JQZSDrawBetItems[index].betCode = 1;
				}
				else if (index == 2) {
					m_JQZSDrawBetItems[index].tid = 2;
					m_JQZSDrawBetItems[index].betCode = 2;
				}
				else if (index == 3) {
					m_JQZSDrawBetItems[index].tid = 2;
					m_JQZSDrawBetItems[index].betCode = 3;
				}
				else if (index == 4) {
					m_JQZSDrawBetItems[index].tid = 2;
					m_JQZSDrawBetItems[index].betCode = 4;
				}
				else if (index == 5) {
					m_JQZSDrawBetItems[index].tid = 2;
					m_JQZSDrawBetItems[index].betCode = 5;
				}
				else if (index == 6) {
					m_JQZSDrawBetItems[index].tid = 2;
					m_JQZSDrawBetItems[index].betCode = 6;
				}
				else if (index == 7) {
					m_JQZSDrawBetItems[index].tid = 2;
					m_JQZSDrawBetItems[index].betCode = 7;
				}

			}
		}
	}

	if (m_CurrentMatchItem.get() != NULL) {
		for (int i = 0; i < 8; i++) {
			CRect rcItem = m_JQZSDrawBetItems[i].rect;
			JCMatchItem::Subject* subject = m_CurrentMatchItem->get_subject(m_JQZSDrawBetItems[i].tid, 
				m_JQZSDrawBetItems[i].betCode);
			if (subject != NULL) {
				if (subject->checked) {
					dc.FillSolidRect(rcItem, RGB(0x7F, 0x00, 0x00));
					dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
				} else {
					dc.SetTextColor(RGB(0x00, 0x00, 0x00));
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
		for (int j = 0; j < 3; j++) {
			int l = left + j * (itemWidth + 1);
			int r = l + itemWidth;
			for (int i = 0; i < 3; i++) {
				int t = top + i * (itemHeight + 1);
				int b = t + itemHeight;
				int index = i * 3 + j;
				m_BQCDrawBetItems[index].rect.SetRect(l, t, r, b);
				m_BQCDrawBetItems[index].rect.OffsetRect(1, 1);
				if (index == 0) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 0;
				}
				else if (index == 1) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 1;
				}
				else if (index == 2) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 2;
				}
				else if (index == 3) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 3;
				}
				else if (index == 4) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 4;
				}
				else if (index == 5) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 5;
				}
				else if (index == 6) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 6;
				}
				else if (index == 7) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 7;
				}
				else if (index == 8) {
					m_BQCDrawBetItems[index].tid = 4;
					m_BQCDrawBetItems[index].betCode = 8;
				}

			}
		}
	}

	if (m_CurrentMatchItem.get() != NULL) {
		for (int i = 0; i < 9; i++) {
			CRect rcItem = m_BQCDrawBetItems[i].rect;
			JCMatchItem::Subject* subject = m_CurrentMatchItem->get_subject(m_BQCDrawBetItems[i].tid,
				m_BQCDrawBetItems[i].betCode);
			if (subject != NULL) {
				if (subject->checked) {
					dc.FillSolidRect(rcItem, RGB(0x7F, 0x00, 0x00));
					dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
				}
				else {
					dc.SetTextColor(RGB(0x00, 0x00, 0x00));
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
		for (int j = 0; j < 7; j++) {
			int l = left + j * (itemWidth + 1);
			int r = l + itemWidth;
			for (int i = 0; i < 5; i++) {
				int t = top + i * (itemHeight + 1);
				int b = t + itemHeight;
				int index = i * 7 + j;
				if (index < 31) {
					m_BFDrawBetItems[index].rect.SetRect(l, t, r, b);
					m_BFDrawBetItems[index].rect.OffsetRect(1, 1);
					m_BFDrawBetItems[index].tid = 3;
					m_BFDrawBetItems[index].betCode = index;
				}
				else {
					break;
				}
			}
		}
	}
	
	if (m_CurrentMatchItem.get() != NULL) {
		JCMatchItem::Subject* subject = NULL;
		for (int i = 0; i < 32; i++) {
			CRect rcItem = m_BFDrawBetItems[i].rect;
			subject = m_CurrentMatchItem->get_subject(3, i);
			if (subject != NULL) {
				if (subject->checked) {
					dc.FillSolidRect(rcItem, RGB(0x7F, 0x00, 0x00));
					dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
				}
				else {
					dc.SetTextColor(RGB(0x00, 0x00, 0x00));
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