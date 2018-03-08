#include "stdafx.h"
#include "DanLueStat.h"
#include "Global.h"
#include "DanLueDialog.h"


DanLueStat::DanLueStat(const std::shared_ptr<SQLite::Database>& db) : 
	m_lstResult(this, 100),
	m_buSearch(this, 100),
	m_coIDs(this, 100),
	m_chkIDs(this, 100),
	m_edCat(this, 100),
	m_chkCat(this, 100)
{
	m_pDatabase = db;
}

LRESULT DanLueStat::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DoDataExchange(FALSE);
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	int gapX = rcDesktop.Width() / 40;
	int gapY = rcDesktop.Height() / 50;
	rcDesktop.DeflateRect(gapX, gapY, gapX, gapY);
	SetWindowPos(NULL, &rcDesktop, SWP_NOZORDER);
	InitControls();
	InitData();
	return TRUE;
}

LRESULT DanLueStat::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = CAxDialogImpl<DanLueStat>::OnDestroy(uMsg, wParam, lParam, bHandled);
	return lRet;
}

LRESULT DanLueStat::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
	pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left - 10;
	pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top - 10;
	return 1L;
}

LRESULT DanLueStat::OnQuery(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	DoQuery();
	return 1L;
}

LRESULT DanLueStat::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	EndDialog(IDCANCEL);
	return 1L;
}

void DanLueStat::InitControls() {
	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconSmall, FALSE);
	DWORD dwStyleEx = LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL;
	ListView_SetExtendedListViewStyle(m_lstResult.m_hWnd, dwStyleEx);

	CRect rcItem, rcc;
	GetClientRect(rcc);
	m_lstResult.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcc.right - 10;
	rcItem.bottom = rcc.bottom - 10;
	m_lstResult.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER | SWP_NOMOVE);

	int colIndex = 0;
	m_lstResult.InsertColumn(colIndex, "期号", LVCFMT_CENTER, 100);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstResult.InsertColumn(colIndex, "赛事", LVCFMT_CENTER, 100);    //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstResult.InsertColumn(colIndex, "对阵", LVCFMT_CENTER, 180);    //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstResult.InsertColumn(colIndex, "比赛时间", LVCFMT_CENTER, 110);    //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstResult.InsertColumn(colIndex, "胜平负", LVCFMT_CENTER, 120);    //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstResult.InsertColumn(colIndex, "让胜平负", LVCFMT_CENTER, 120);  //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstResult.InsertColumn(colIndex, "比分结果", LVCFMT_CENTER, 100);  //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstResult.InsertColumn(colIndex, "胜平负结果", LVCFMT_CENTER, 80);    //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstResult.InsertColumn(colIndex, "让胜平负结果", LVCFMT_CENTER, 80);    //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstResult.InsertColumn(colIndex, "盘路", LVCFMT_CENTER, 80);  //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstResult.InsertColumn(colIndex, "进球总数", LVCFMT_CENTER, 80);  //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstResult.InsertColumn(colIndex, "半全场", LVCFMT_CENTER, 80);  //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstResult.InsertColumn(colIndex, "比分", LVCFMT_CENTER, 80);  //90
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);

	m_chkIDs.SetCheck(BST_CHECKED);
	m_chkCat.SetCheck(BST_UNCHECKED);
}

void DanLueStat::InitData() {
	CStringATL strSQL = (_T("SELECT ID FROM JCZQ WHERE ID LIKE ? ORDER BY ID DESC"));
	std::set<CStringATL> ids;
	if (TRUE) {
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		CStringATL strParam, id;
		strParam.Format("%04u%02u_____", st.wYear, st.wMonth);
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bind(1, strParam);
		while (sm.executeStep()) {
			id = sm.getColumn(0).getString().c_str();
			id = id.Left(id.GetLength() - 3);
			ids.insert(id);
		}
	}
	for (const auto& id : ids) {
		m_coIDs.AddString(id);
	}
	if (m_coIDs.GetCount() > 0) {
		m_coIDs.SetCurSel(0);
	}
	DoQuery();
}

void DanLueStat::DoQuery() {
	m_lstResult.DeleteAllItems();
	CStringATL strDate, strCat;
	if (m_chkIDs.GetCheck() == BST_CHECKED && m_coIDs.GetCurSel() >= 0) {
		m_coIDs.GetLBText(m_coIDs.GetCurSel(), strDate);
		strDate += "___";
	}
	CStringATL strSQL;
	if (!strDate.IsEmpty()) {
		strSQL.Format(_T("SELECT ID, CATEGORY, DESCRIPTION, HAND, START_TIME, BUY_TIME, SUBJECTS, RESULT FROM JCZQ WHERE ID LIKE '%s'"), strDate);
	} else {
		strSQL = _T("SELECT ID, CATEGORY, DESCRIPTION, HAND, START_TIME, BUY_TIME, SUBJECTS, RESULT FROM JCZQ WHERE");
	}
	if (m_chkCat.GetCheck() == BST_CHECKED) {
		m_edCat.GetWindowText(strCat);
	}
	if (!strCat.IsEmpty()) {
		strSQL += _T(" CATEGORY=?");
	}
	strSQL += _T(" ORDER BY ID ASC");
	if (strDate.IsEmpty() && strCat.IsEmpty()) {
		return;
	}
	SQLite::Statement sm(*m_pDatabase, strSQL);
	int iIndex = 0;
	while (sm.executeStep()) {
		int colIndex = 0;
		DanLueDialog::JCMatchItem item;
		item.id = sm.getColumn(0).getString().c_str();
		item.match_category = sm.getColumn(1).getString();
		item.descrition = sm.getColumn(2).getString();
		item.hand = sm.getColumn(3).getInt64();
		item.start_time = sm.getColumn(4).getString();
		item.last_buy_time = sm.getColumn(5).getString();
		std::string data = sm.getColumn(6).getString();
		item.result = sm.getColumn(7).getString();
		CStlStrArray arrBetItems, arrPart;
		Global::DepartString(data, "|", arrBetItems);
		for (auto& bet : arrBetItems) {
			CStlStrArray arrBetInfos;
			Global::DepartString(bet, ";", arrBetInfos);
			if (arrBetInfos.size() != 3) return ;
			DanLueDialog::JCMatchItem::Subject sub;
			sub.tid = atoi(arrBetInfos[0].c_str());
			sub.betCode = atoi(arrBetInfos[1].c_str());
			sub.odds = atof(arrBetInfos[2].c_str());
			sub.calcTip(item.hand);
			sub.checked = false;
			item.subjects.push_back(sub);
		}
		iIndex = m_lstResult.InsertItem(iIndex, item.id.c_str());
		m_lstResult.SetItemText(iIndex, ++colIndex, item.match_category.c_str());
		m_lstResult.SetItemText(iIndex, ++colIndex, item.descrition.c_str());
		m_lstResult.SetItemText(iIndex, ++colIndex, item.start_time.c_str());

		CStringATL temp;
		double a = 0.00, b = 0.00, c = 0.00;
		DanLueDialog::JCMatchItem::Subject* sub = item.get_subject(6, 3);
		if (sub == NULL) {
			temp = "未 开 售";
		}
		else {
			a = sub->odds;
			sub = item.get_subject(6, 1);
			b = sub->odds;
			sub = item.get_subject(6, 0);
			c = sub->odds;
			temp.Format("%.2f  %.2f  %.2f", a, b, c);
		}
		m_lstResult.SetItemText(iIndex, ++colIndex, temp);

		sub = item.get_subject(1, 3);
		a = sub->odds;
		sub = item.get_subject(1, 1);
		b = sub->odds;
		sub = item.get_subject(1, 0);
		c = sub->odds;
		if (item.hand < 0)
			temp.Format("%.2f(%d)  %.2f  %.2f", a, (int)item.hand, b, c);
		else
			temp.Format("%.2f(+%d)  %.2f  %.2f", a, (int)item.hand, b, c);
		m_lstResult.SetItemText(iIndex, ++colIndex, temp);

		if (!item.result.empty()) {
			m_lstResult.SetItemText(iIndex, ++colIndex, item.result.c_str());
			Global::DepartString(item.result, "|", arrBetItems);
			if (arrBetItems.size() == 2) {
				CStringATL bqcTip, bfTip;
				Global::DepartString(arrBetItems[0], ":", arrPart);
				int half_home = atoi(arrPart[0].c_str());
				int half_away = atoi(arrPart[1].c_str());
				bfTip = arrBetItems[1].c_str();
				Global::DepartString(arrBetItems[1], ":", arrPart);
				int full_home = atoi(arrPart[0].c_str());
				int full_away = atoi(arrPart[1].c_str());
				if (half_home > half_away) {
					bqcTip = "胜";
				} else if (half_home == half_away) {
					bqcTip = "平";
				} else {
					bqcTip = "负";
				}
				if (full_home > full_away) {
					bqcTip += "胜";
					if (full_away > 2) {
						bfTip = "胜其它";
					}
				}
				else if (full_home == full_away) {
					bqcTip += "平";
					if (full_away > 2) {
						bfTip = "平其它";
					}
				}
				else {
					if (full_away > 2) {
						bfTip = "负其它";
					}
					bqcTip += "负";
				}
				sub = item.get_subject(6, 3);
				int pan = -100;
				if (sub != NULL) {
					if (full_home == full_away) {
						sub = item.get_subject(6, 1);
					} else if (full_home < full_away) {
						sub = item.get_subject(6, 0);
					}
					temp.Format("%d [%.2f]", (int)sub->betCode, sub->odds);
					m_lstResult.SetItemText(iIndex, ++colIndex, temp);
					pan = sub->getPan(item.hand);
				} else {
					m_lstResult.SetItemText(iIndex, ++colIndex, "");
				}
				sub = item.get_subject(1, 3);
				if (sub != NULL) {
					if (full_home + item.hand == full_away) {
						sub = item.get_subject(1, 1);
					} else if (full_home + item.hand < full_away) {
						sub = item.get_subject(1, 0);
					}
					temp.Format("%d [%.2f]", (int)sub->betCode, sub->odds);
					m_lstResult.SetItemText(iIndex, ++colIndex, temp);
					if (pan < 0) {
						pan = sub->getPan(item.hand);
					}
				} else {
					m_lstResult.SetItemText(iIndex, ++colIndex, "");
				}
				temp.Format("%d", pan);
				m_lstResult.SetItemText(iIndex, ++colIndex, temp); //盘路
				int jqzs = full_home + full_away;
				if (jqzs > 7) {
					jqzs = 7;
				}
				sub = item.get_subject(2, jqzs);  //jqzs
				if (sub != NULL) {
					temp.Format("%d [%.2f]", (int)sub->betCode, sub->odds);
					m_lstResult.SetItemText(iIndex, ++colIndex, temp);
				}
				else {
					m_lstResult.SetItemText(iIndex, ++colIndex, "");
				}
				sub = item.get_subject(4, (LPCSTR)bqcTip);
				if (sub != NULL) {
					temp.Format("%s [%.2f]", (LPCSTR)bqcTip, sub->odds);
					m_lstResult.SetItemText(iIndex, ++colIndex, temp);
				}
				else {
					m_lstResult.SetItemText(iIndex, ++colIndex, "");
				}
				sub = item.get_subject(3, (LPCSTR)bfTip);
				if (sub != NULL) {
					temp.Format("%s [%.2f]", (LPCSTR)bfTip, sub->odds);
					m_lstResult.SetItemText(iIndex, ++colIndex, temp);
				}
				else {
					m_lstResult.SetItemText(iIndex, ++colIndex, "");
				}

			}
		}

	}


}