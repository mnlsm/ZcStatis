#include "stdafx.h"
#include "DanLueDialog.h"
#include "Global.h"
//"http://appserver.87.cn/jc/match"

static const CStlString LUA_FILTER_NAME = _T("脚本文件(*.lua)");
static const CStlString LUA_FILTER = _T("*.lua");

DanLueDialog DanLueDialog::sInst;
void DanLueDialog::PopUp() {
	if (!sInst.IsWindow()) {
		sInst.Create(::GetDesktopWindow());
	}
	if (sInst.IsWindow()) {
		sInst.ShowWindow(SW_SHOW);
		::SetForegroundWindow(sInst.m_hWnd);
	}
}

void DanLueDialog::Destroy() {
	if (sInst.IsWindow()) {
		sInst.DestroyWindow();
	}
}


DanLueDialog::DanLueDialog() : 
	m_Engine((DanLueEngine*)NULL),
	m_lstMatch(this, 1),
	m_lstResult(this, 2),
	m_stYZM(this, 3),
	m_stBetArea(this, 4),
	m_buLogin(this, 100),
	m_buLogoff(this, 100),
	m_buRefresh(this, 100),
	m_buClear(this, 100),
	m_stBetAreaTitle(this, 100),
	m_buCalc(this, 100),
	m_stSep1(this, 100),
	m_stSep2(this, 100),
	m_buCopy(this, 100),
	m_stResult(this, 100),
	m_buUpload(this, 100)  {
	m_FirstDrawBetArea = true;
	SYSTEMTIME tm = { 0 };
	GetLocalTime(&tm);
	m_strQH.Format("%04d-%02d-%02d", (int)tm.wYear, (int)tm.wMonth, (int)tm.wDay);
	CreateWorkDir();
	httpMgr_.reset(new (std::nothrow) CHttpClientMgr());
	if (httpMgr_.get() != nullptr) {
		httpMgr_->Init();
	}
	m_Engine.reset();
}

LRESULT DanLueDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DoDataExchange(FALSE);

	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	int gapX = rcDesktop.Width() / 50;
	int gapY = rcDesktop.Height() / 50;
	rcDesktop.DeflateRect(gapX, gapY, gapX, gapY);
	SetWindowPos(NULL, &rcDesktop, SWP_NOZORDER);

	//CenterWindow();
	InitControls();

	//ReloadStatisData();
	return TRUE;
}

LRESULT DanLueDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = CAxDialogImpl<DanLueDialog>::OnDestroy(uMsg, wParam, lParam, bHandled);
	//doLogOff();
	return lRet;
}

LRESULT DanLueDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
	pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left - 10;
	pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top - 10;
	return 1L;
}

LRESULT DanLueDialog::OnListLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstMatch.DefWindowProc(uMsg, wParam, lParam);
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LVHITTESTINFO lvh = { 0 };
	lvh.pt = pt;
	UINT index = m_lstMatch.HitTest(&lvh);
	if (index != -1) {
		CStringATL strID;
		m_lstMatch.GetItemText(index, 0, strID);
		auto& iter = m_JCMatchItems.begin();
		for (; iter != m_JCMatchItems.end(); ++iter) {
			if (iter->second->id.compare(strID) == 0) {
				m_CurrentMatchItem = iter->second;
				if (m_CurrentMatchItem.get() != NULL) {
					m_stBetArea.Invalidate();
				}
			}
		}
		//DoListMenuCommand(IDM_EDIT_FANGANS, index);
	}

	return lRet;
}

LRESULT DanLueDialog::OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstMatch.DefWindowProc(uMsg, wParam, lParam);
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LVHITTESTINFO lvh = { 0 };
	lvh.pt = pt;
	UINT index = m_lstMatch.HitTest(&lvh);
	if (index != -1) {
		CMenu menu;
		if (menu.CreatePopupMenu()) {
			menu.AppendMenu(MF_STRING, 100, _T("清空选择"));
			menu.AppendMenu(MF_STRING, 101, _T("高赔选择"));

			m_lstMatch.ClientToScreen(&pt);
			UINT cmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
			DoMatchListMenuCommand(cmd, index);
			menu.DestroyMenu();
		}
	}
	return lRet;
}

void DanLueDialog::DoMatchListMenuCommand(UINT cmd, UINT index) {
	if (cmd == 100 || cmd == 101) {
		CStringATL strID;
		m_lstMatch.GetItemText(index, 0, strID);
		auto& iter = m_JCMatchItems.begin();
		for (; iter != m_JCMatchItems.end(); ++iter) {
			if (iter->second->id.compare(strID) == 0) {
				m_CurrentMatchItem = iter->second;
				for (auto& sub : m_CurrentMatchItem->subjects) {
					sub.checked = false;
				}
				if (cmd == 101) {
					int hand = iter->second->hand;
					for (auto& sub : m_CurrentMatchItem->subjects) {
						if (sub.getPan(hand) > 0) {
							sub.checked = true;
						}
					}
					DoRefreshMatchListResults();
				} else if (cmd == 100) {
					m_lstMatch.SetItemText(index, 6, "");
				}
				m_stBetArea.Invalidate();
			}
		}
	}
}

void DanLueDialog::DoRefreshMatchListResults() {
	for (int i = 0; i < m_lstMatch.GetItemCount(); i++) {
		CStringATL strID;
		m_lstMatch.GetItemText(i, 0, strID);
		CStringATL result;
		for (auto& item : m_JCMatchItems) {
			if (item.second->id.compare(strID) == 0) {
				for (auto& sub : item.second->subjects) {
					if (sub.checked) {
						result += sub.betStr().c_str();
						result += ";";
					}
				}
			}
		}
		m_lstMatch.SetItemText(i, 6, result);
	}
}

LRESULT DanLueDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ShowWindow(SW_HIDE);
	return 1L;
}
/*
LRESULT DanLueDialog::OnAddRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CStringATL strLuaFile;
	CStlString strScript, reason;
	strLuaFile.Format(_T("%s\\%s.lua"), m_strWorkDir, m_strQH);
	if (!PathFileExists(strLuaFile)) {
		MessageBox("Lua脚本文件读取失败 0！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	if (!Global::ReadFileData((LPCTSTR)strLuaFile, strScript) || strScript.empty()) {
		MessageBox("Lua脚本文件读取失败 1！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	DanLueEngine engine(strScript);
	if (engine.CalculateAllResult(reason)) {
		strLuaFile.Format(_T("【%s】计算完成， 共有【%u】注结果， 是否保存？"), m_strQH, engine.GetResult().size());
		if (MessageBox(strLuaFile, "完成", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			DoSaveResult(engine);
		}
	} else {
		strLuaFile.Format(_T("【%s】计算失败！"), m_strQH);
		MessageBox(strLuaFile, "错误", MB_ICONERROR | MB_OK);
	}
	return 1L;
}
*/


LRESULT DanLueDialog::OnLoginIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	doLogin();
	return 1L;
}

LRESULT DanLueDialog::OnLoginOff(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	doLogOff();
	return 1L;
}

LRESULT DanLueDialog::OnCalc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	std::string filedata;
	TCHAR szFilterName[30] = { _T('\0') };
	_tcscpy(szFilterName, LUA_FILTER_NAME.c_str());
	_tcscat(szFilterName + LUA_FILTER_NAME.length() + 1, LUA_FILTER.c_str());
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilterName, m_hWnd);
	dlg.m_ofn.lpstrInitialDir = m_strWorkDir;
	CStringATL strInitFileName = _T("1.lua");;
	TCHAR szFileName[MAX_PATH + 1] = { _T('\0') };
	_tcscpy(szFileName, strInitFileName);
	dlg.m_ofn.lpstrFile = szFileName;
	if(dlg.DoModal() != IDOK) {
		return 1;
	}
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	if (!Global::ReadFileData(strLoadPath, filedata) || filedata.size() == 0) {
		MessageBox("Script文件读取失败！", "错误", MB_ICONERROR | MB_OK);
		return 1;
	}

	std::shared_ptr<DanLueEngine> engine(new (std::nothrow) DanLueEngine(filedata, m_strWorkDir));
	if (engine.get() != NULL) {
		engine->setScriptFile(strLoadPath.c_str());
		CStlString reason = "";
		if (!engine->CalculateAllResult(reason)) {
			CStringATL errorStr;
			errorStr.Format("计算错误： %s", reason.c_str());
			MessageBox(errorStr, "错误", MB_ICONERROR | MB_OK);
			return 1L;
		}
		engine.swap(m_Engine);
	}

	//更新显示
	for (auto& item : m_JCMatchItems) {
		for (auto& sub : item.second->subjects) {
			sub.checked = false;
		}
	}
	for (const auto& r : m_Engine->getSource()) {
		for (const auto& item : r.bets) {
			JCMatchItem::Subject* sub = get_subjects(r.id, item.tid, item.code);
			if (sub != NULL) {
				sub->checked = true;
			}
		}
	}
	DoRefreshMatchListResults();
	m_stBetArea.Invalidate();

	m_lstResult.DeleteAllItems();
	int index = 0;
	for (const auto& r : m_Engine->getResult()) {
		int colIndex = 0;
		double bouns = 2.0;
		CStringATL strResult, strCodes;
		strResult.Format("%d", index + 1);
		int lIndex = m_lstResult.InsertItem(index++, strResult);
		for (const auto& item : r) {
			bouns = bouns * item.bet.odds;
			char sz[32] = { '\0' };
			sprintf(sz, "%d-%d(%.2f)", item.bet.tid, item.bet.code, item.bet.odds);
			if (!strCodes.IsEmpty()) {
				strCodes += " ,";
			}
			strCodes += sz;
		}
		strResult.Format("%.2f", bouns);
		m_lstResult.SetItemText(lIndex, ++colIndex, strResult);
		m_lstResult.SetItemText(lIndex, ++colIndex, strCodes);
	}
	CStringATL temp;
	temp.Format("结果列表(%d):", m_Engine->getResult().size());
	m_stResult.SetWindowText(temp);

	return 1L;
}

LRESULT DanLueDialog::OnUpload(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	if (MessageBox("是否上传投注结果？", "提示", MB_ICONQUESTION | MB_YESNO) == IDYES) {
		if (doHeMai() == 0) {
			m_buUpload.EnableWindow(FALSE);
		}
	}
	return 1L;
}

LRESULT DanLueDialog::OnClearAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	for (auto& item : m_JCMatchItems) {
		for (auto& sub : item.second->subjects) {
			sub.checked = false;
		}
	}
	DoRefreshMatchListResults();
	m_stBetArea.Invalidate();
	m_lstResult.DeleteAllItems();
	m_stResult.SetWindowText("结果列表:");

	return 1L;
}

LRESULT DanLueDialog::OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	doJcMatchList();
	return 1L;
}

LRESULT DanLueDialog::OnCopyChoices(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CStringATL strMatchBets = "kMatchBets = {\r\n";
	for (auto& item : m_JCMatchItems) {
		CStringATL strItem, strBets;
		item.second->id;
		for (auto& sub : item.second->subjects) {
			if (sub.checked) {
				char szTemp[128] = { '\0' };
				sprintf(szTemp, "%d,%d,%.2f", (int)sub.tid, (int)sub.betCode, sub.odds);
				if (!strBets.IsEmpty()) {
					strBets += ";";
				}
				strBets += szTemp;
			}
		}
		if (!strBets.IsEmpty()) {
			strItem.Format("    \"%s;%d;%s\",         --%s\r\n", item.second->id.c_str(),
				(int)item.second->hand, strBets, item.second->descrition.c_str());
			strMatchBets += strItem;
		}
	}
	strMatchBets += "};\r\n";
	//strMatchBets = Global::toUTF8((LPCSTR)strMatchBets).c_str();
	if (OpenClipboard()) {
		EmptyClipboard();
		if (!strMatchBets.IsEmpty()) {
			HGLOBAL hGlobal = GlobalAlloc(GHND, strMatchBets.GetLength() + 1);
			if (hGlobal != NULL) {
				char* buffer = (char*)GlobalLock(hGlobal);
				memcpy(buffer, strMatchBets, strMatchBets.GetLength());
				GlobalUnlock(hGlobal);
				SetClipboardData(CF_TEXT, hGlobal);
			}
		}
		CloseClipboard();
	}

	return 1L;
}



void DanLueDialog::InitControls() {
	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);

	DWORD dwStyleEx = LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP
		| LVS_EX_REGIONAL;
	ListView_SetExtendedListViewStyle(m_lstMatch.m_hWnd, dwStyleEx);
	ListView_SetExtendedListViewStyle(m_lstResult.m_hWnd, dwStyleEx);

	CRect rcItem, rcc;
	GetClientRect(rcc);

	m_lstResult.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcc.right - 10;
	rcItem.bottom = rcc.bottom - 10;
	m_lstResult.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER | SWP_NOMOVE);

	m_stBetArea.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	int temp = rcItem.Width();
	m_stBetArea.SetWindowPos(NULL, rcc.right - 10 - temp, rcItem.top, temp, rcItem.Height(), SWP_NOZORDER | SWP_NOSIZE);
	temp = rcc.right - 10 - temp;
	m_stBetAreaTitle.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	m_stBetAreaTitle.SetWindowPos(NULL, temp, rcItem.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	m_lstMatch.GetClientRect(rcItem);
	rcItem.right = temp - 20;
	m_lstMatch.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER | SWP_NOMOVE);

	m_stSep1.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.left = 0;
	rcItem.right = rcc.right;
	m_stSep1.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER);

	m_stSep2.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.left = 0;
	rcItem.right = rcc.right;
	m_stSep2.SetWindowPos(NULL, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOZORDER);



	//insert header;
	int colIndex = 0;
	m_lstMatch.InsertColumn(colIndex, "期号", LVCFMT_CENTER, 100);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "赛事", LVCFMT_CENTER, 100);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "对阵", LVCFMT_CENTER, 250);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstMatch.InsertColumn(colIndex, "过期时间", LVCFMT_CENTER, 130);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "胜平负", LVCFMT_CENTER, 200);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstMatch.InsertColumn(colIndex, "让胜平负", LVCFMT_CENTER, 200);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstMatch.InsertColumn(colIndex, "选择结果", LVCFMT_CENTER, 400);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);

	m_lstMatch.SetSortColumn(0);

	m_lstResult.GetClientRect(rcItem);
	colIndex = 0;
	m_lstResult.InsertColumn(colIndex, "编号", LVCFMT_CENTER, 70);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstResult.InsertColumn(colIndex, "奖金(元)", LVCFMT_CENTER, 90);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstResult.InsertColumn(colIndex, "结果", LVCFMT_CENTER, rcItem.Width() - 160);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	//m_lstMatch.setItemH
	//set sort type
}



void DanLueDialog::ReloadMatchListData() {
	m_lstMatch.DeleteAllItems();
	
	auto& iter = m_JCMatchItems.begin();// equal_range((LPCSTR)m_strQH);
	int iIndex = 0;
	for (; iter != m_JCMatchItems.end(); ++iter) {
		if (iter->first.find(m_strQH) != 0) {
			continue;
		}
		int colIndex = 0;
		std::shared_ptr<JCMatchItem> ji = iter->second;
		iIndex = m_lstMatch.InsertItem(iIndex, ji->id.c_str());
		m_lstMatch.SetItemText(iIndex, ++colIndex, ji->match_category.c_str());
		m_lstMatch.SetItemText(iIndex, ++colIndex, ji->descrition.c_str());
		m_lstMatch.SetItemText(iIndex, ++colIndex, ji->last_buy_time.c_str());
		CStringATL temp;
		double a = 0.00, b = 0.00, c = 0.00;
		JCMatchItem::Subject* sub = ji->get_subject(6, 3);
		if (sub == NULL) {
			temp = "未 开 售";
		} else {
			a = sub->odds;
			sub = ji->get_subject(6, 1);
			b = sub->odds;
			sub = ji->get_subject(6, 0);
			c = sub->odds;
			temp.Format("%.2f  %.2f  %.2f", a , b , c);
		}
		m_lstMatch.SetItemText(iIndex, ++colIndex, temp);

		sub = ji->get_subject(1, 3);
		a = sub->odds;
		sub = ji->get_subject(1, 1);
		b = sub->odds;
		sub = ji->get_subject(1, 0);
		c = sub->odds;
		if (ji->hand < 0)
			temp.Format("%.2f(%d)  %.2f  %.2f", a, (int)ji->hand, b, c);
		else 
			temp.Format("%.2f(+%d)  %.2f  %.2f", a, (int)ji->hand, b, c);
		m_lstMatch.SetItemText(iIndex, ++colIndex, temp);
	}
	m_lstMatch.DoSortItems(0, false);
	m_CurrentMatchItem.reset();
	m_stBetArea.Invalidate();
}



void DanLueDialog::CreateWorkDir() {
	CStringATL strPath(Global::GetAppPath().c_str());
	m_strRootDir = strPath + _T("JC");
	CStringATL strInitFileName;
	CreateDirectory(m_strRootDir, NULL);
	strPath = m_strRootDir + _T("\\") + m_strQH;
	CreateDirectory(strPath, NULL);
	m_strWorkDir = strPath;
}


