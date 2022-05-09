#include "stdafx.h"
#include "BeiDanDialog.h"
#include "BeiDanStat.h"
#include "Global.h"
#include "MiscHelper.h"


static const CStlString LUA_FILTER_NAME = _T("脚本文件(*.lua)");
static const CStlString LUA_FILTER = _T("*.lua");

BeiDanDialog BeiDanDialog::sInst;
void BeiDanDialog::PopUp(const std::shared_ptr<SQLite::Database>& db) {
	if (!sInst.IsWindow()) {
		sInst.m_pDatabase = db;
		sInst.Create(::GetDesktopWindow());
	}
	if (sInst.IsWindow()) {
		sInst.ShowWindow(SW_SHOW);
		::SetForegroundWindow(sInst.m_hWnd);
	}
}

void BeiDanDialog::Destroy() {
	if (sInst.IsWindow()) {
		sInst.DestroyWindow();
	}
}

BeiDanDialog::BeiDanDialog() :
	m_Engine((BeiDanEngine*)NULL),
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
	m_buExtractLua(this, 100),
	m_buUpload(this, 100) {
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

LRESULT BeiDanDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddIdleHandler(this);

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
	SetWindowText("澳客北单");
	return TRUE;
}

LRESULT BeiDanDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	//_Module.GetMessageLoop()->RemoveIdleHandler(this);
	LRESULT lRet = CAxDialogImpl<BeiDanDialog>::OnDestroy(uMsg, wParam, lParam, bHandled);
	//doLogOff();
	return lRet;
}

LRESULT BeiDanDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, sizeof(RECT));
	LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
	pMinMaxInfo->ptMaxSize.x = pMinMaxInfo->ptMaxTrackSize.x = rcDesktop.right - rcDesktop.left - 10;
	pMinMaxInfo->ptMaxSize.y = pMinMaxInfo->ptMaxTrackSize.y = rcDesktop.bottom - rcDesktop.top - 10;
	return 1L;
}

LRESULT BeiDanDialog::OnListLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
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

LRESULT BeiDanDialog::OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstMatch.DefWindowProc(uMsg, wParam, lParam);
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LVHITTESTINFO lvh = { 0 };
	lvh.pt = pt;
	UINT index = m_lstMatch.HitTest(&lvh);
	if (index != -1) {
		CStringATL strMatchText;
		m_lstMatch.GetItemText(index, 2, strMatchText);
		ShowMatchWebBrowser(strMatchText);
	}
	return lRet;
}


LRESULT BeiDanDialog::OnListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstMatch.DefWindowProc(uMsg, wParam, lParam);
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LVHITTESTINFO lvh = { 0 };
	lvh.pt = pt;
	UINT index = m_lstMatch.HitTest(&lvh);
	if (index != -1) {
		CMenu menu;
		if (menu.CreatePopupMenu()) {
			menu.AppendMenu(MF_STRING, 100, _T("清空选择"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, 101, _T("高赔全包"));
			menu.AppendMenu(MF_STRING, 102, _T("高赔上盘"));
			menu.AppendMenu(MF_STRING, 103, _T("高赔下盘"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, 104, _T("低赔全包"));
			menu.AppendMenu(MF_STRING, 105, _T("低赔上盘"));
			menu.AppendMenu(MF_STRING, 106, _T("低赔下盘"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, 107, _T("分析预测"));
			m_lstMatch.ClientToScreen(&pt);
			UINT cmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
			DoMatchListMenuCommand(cmd, index);
			menu.DestroyMenu();
		}
	}
	return lRet;
}

void BeiDanDialog::DoMatchListMenuCommand(UINT cmd, UINT index) {
	if (cmd >= 100) {
		CStringATL strID, strMatchText;
		m_lstMatch.GetItemText(index, 0, strID);
		m_lstMatch.GetItemText(index, 2, strMatchText);
		auto& iter = m_JCMatchItems.begin();
		for (; iter != m_JCMatchItems.end(); ++iter) {
			if (iter->second->id.compare(strID) == 0) {
				m_CurrentMatchItem = iter->second;
				for (auto& sub : m_CurrentMatchItem->subjects) {
					sub.checked = false;
				}
				int hand = iter->second->hand;
				if (cmd == 101 || cmd == 102 || cmd == 103) {
					for (auto& sub : m_CurrentMatchItem->subjects) {
						int pan = sub.getPan(hand);
						if (cmd == 101 && pan > 0) {
							sub.checked = true;
						}
						else if (cmd == 102 && (pan == 1 || pan == 2)) {
							sub.checked = true;
						}
						else if (cmd == 103 && (pan == 3 || pan == 4)) {
							sub.checked = true;
						}
					}
					DoRefreshMatchListResults();
				} else if (cmd == 104 || cmd == 105 || cmd == 106) {
					for (auto& sub : m_CurrentMatchItem->subjects) {
						int pan = sub.getPan(hand);
						if (cmd == 104 && pan < 0) {
							sub.checked = true;
						}
						else if (cmd == 105 && (pan == -1)) {
							sub.checked = true;
						}
						else if (cmd == 106 && (pan == -2)) {
							sub.checked = true;
						}
					}
					DoRefreshMatchListResults();
				} else if (cmd == 100) {
					m_lstMatch.SetItemText(index, 6, "");
				}
				m_stBetArea.Invalidate();
				if (cmd == 107) {
					ShowMatchWebBrowser(strMatchText);
				}
				break;
			}
		}
		//https://m.okooo.com/match/form.php?MatchID=1158125&from=%2Flive%2F
	}
}

LRESULT BeiDanDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CloseMatchWebBrowsers();
	ShowWindow(SW_HIDE);
	return 1L;
}

LRESULT BeiDanDialog::OnLoginIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	doLogin();
	DoReloadBackupResult();
	return 1L;
}

LRESULT BeiDanDialog::OnLoginOff(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	m_Engine.reset();
	m_JCMatchItems.clear();
	m_CurrentMatchItem.reset();
	m_lstMatch.DeleteAllItems();
	m_lstResult.DeleteAllItems();
	m_stResult.SetWindowText("结果列表:");
	m_stBetArea.Invalidate();
	doLogOff();
	return 1L;
}

LRESULT BeiDanDialog::OnCalc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	std::string filedata;
	TCHAR szFilterName[30] = { _T('\0') };
	_tcscpy(szFilterName, LUA_FILTER_NAME.c_str());
	_tcscat(szFilterName + LUA_FILTER_NAME.length() + 1, LUA_FILTER.c_str());
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilterName, m_hWnd);
	dlg.m_ofn.lpstrInitialDir = m_strWorkDir;
	CStringATL strInitFileName = _T("1.lua");
	TCHAR szFileName[MAX_PATH + 1] = { _T('\0') };
	_tcscpy(szFileName, strInitFileName);
	dlg.m_ofn.lpstrFile = szFileName;
	if (dlg.DoModal() != IDOK) {
		return 1;
	}
	CWaitCursor wait;
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	if (!Global::ReadFileData(strLoadPath, filedata) || filedata.size() == 0) {
		wait.Restore();
		MessageBox("Script文件读取失败！", "错误", MB_ICONERROR | MB_OK);
		return 1;
	}
	wait.Set();
	std::vector<std::shared_ptr<JCMatchItem>> match_items;
	for (auto& iter : m_JCMatchItems) {
		match_items.push_back(iter.second);
	}
	std::shared_ptr<BeiDanEngine> engine(new (std::nothrow) BeiDanEngine(filedata, m_strWorkDir));
	if (engine.get() != NULL) {
		engine->setMatchItems(match_items);
		engine->setScriptFile(strLoadPath.c_str());
		CStlString reason = "";
		if (!engine->CalculateAllResult(reason)) {
			wait.Restore();
			CStringATL errorStr;
			errorStr.Format("计算错误： %s", reason.c_str());
			MessageBox(errorStr, "错误", MB_ICONERROR | MB_OK);
			return 1L;
		}
		engine.swap(m_Engine);
	}
	DoRefreshBetArea();
	DoRefreshMatchListResults();
	std::string buyLines, checkLines;
	CStringATL&& temp = DoRefreshResultListResults(buyLines, checkLines);
	m_stResult.SetWindowText(temp);
	if (m_Engine.get() != NULL) {
		m_Engine->setCheckResult(checkLines);
	}
	CStlString strBuyFilePath = strLoadPath;
	CMiscHelper::string_replace(strBuyFilePath, ".lua", ".txt");
	DeleteFileA(strBuyFilePath.c_str());
	Global::SaveFileData(strBuyFilePath, buyLines, FALSE);
	return 1L;
//https://www.hipdf.cn/txt-to-pdf
}

LRESULT BeiDanDialog::OnExtractLua(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CWaitCursor wait;
	Sleep(1000);
	SYSTEMTIME tm = { 0 };
	GetLocalTime(&tm);
	CStringATL file_path;
	file_path.Format("%s\\%04d%02d%02d%02d%02d%02d.lua", this->m_strWorkDir, 
		tm.wYear, tm.wMonth,tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
	DeleteFile(file_path);
	CResource res;
	if (res.Load(_T("ADDIN"), MAKEINTRESOURCE(IDR_ADDIN2))) {
		Global::SaveFileData((LPCSTR)file_path, (uint8_t*)res.Lock(),
			res.GetSize(), FALSE);
		std::vector<const char*> select_files;
		select_files.push_back((LPCSTR)file_path);
		OpenDirAndSelectFiles((LPCSTR)m_strWorkDir, select_files);
	}
	return 1L;
}

LRESULT BeiDanDialog::OnUpload(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	if (m_Engine.get() == nullptr) {
		MessageBox("读取投注结果失败 0！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	if (m_Engine->getResult().empty()) {
		MessageBox("读取投注结果失败 1！", "错误", MB_ICONERROR | MB_OK);
		return 1L;
	}
	const CStlString& scriptData = m_Engine->getScriptFileData();
	const CStlString& checkResult = m_Engine->getCheckResult();
	const CStlString& scriptFile = m_Engine->getScriptFile();
	CStringATL strSQL;
	strSQL.Format(_T("SELECT ID FROM BDZQ_INVEST WHERE CHECK_RESULT='%s'"), checkResult.c_str());
	if (TRUE) {
		SQLite::Statement sm(*m_pDatabase, strSQL);
		if (sm.executeStep()) {
			MessageBox("该投注结果已经存在！", "错误", MB_ICONERROR | MB_OK);
			return 1L;
		}
	}
	SYSTEMTIME tm = { 0 };
	GetLocalTime(&tm);
	CStringATL id, date;
	id = Global::GetFileName(scriptFile.c_str()); id.Replace(_T(".lua"), _T(""));
	date = m_strQH;
	strSQL = _T("INSERT INTO BDZQ_INVEST (ID, DATE, SCRIPT, CHECK_RESULT, INVEST, INCOME) VALUES(?,?,?,?,?,?)");
	if (TRUE) {
		try {
			SQLite::Statement sm(*m_pDatabase, strSQL);
			sm.bindNoCopy(1, id);
			sm.bindNoCopy(2, date);
			sm.bindNoCopy(3, scriptData);
			sm.bindNoCopy(4, checkResult);
			sm.bindNoCopy(5, "");
			sm.bindNoCopy(6, "");
			if (sm.exec() > 0) {
			}
		} catch (SQLite::Exception &e) {
			MessageBox("投注结果插入失败！", "错误", MB_ICONERROR | MB_OK);
			return 1L;
		}
	}
	MessageBox("备份结果完成！", "info", MB_ICONINFORMATION | MB_OK);
	return 1L;
}

LRESULT BeiDanDialog::OnClearAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CWaitCursor wait;
	for (auto& item : m_JCMatchItems) {
		for (auto& sub : item.second->subjects) {
			sub.checked = false;
		}
	}
	m_Engine.reset();
	DoRefreshMatchListResults();
	m_stBetArea.Invalidate();
	m_lstResult.DeleteAllItems();
	m_stResult.SetWindowText("结果列表:");
	return 1L;
}

LRESULT BeiDanDialog::OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CWaitCursor wait;
	doJcMatchList();
	return 1L;
}

LRESULT BeiDanDialog::OnCopyChoices(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CWaitCursor wait;
	CStringATL strMatchBets = "kMatchBets = {\r\n";
	std::vector<CStringATL> prefixs, subfixs;
	int max_prefixs_length = 0;
	for (auto& item : m_JCMatchItems) {
		CStringATL strBets;
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
			CStringATL prefix;
			prefix.Format("    \"%s;%d;%s\",    ", item.second->id.c_str(),
				(int)item.second->hand, strBets);
			prefixs.push_back(prefix);
			if (prefix.GetLength() > max_prefixs_length) {
				max_prefixs_length = prefix.GetLength();
			}
			subfixs.push_back(item.second->descrition.c_str());
		}
	}
	for (int i = 0; i < prefixs.size(); i++) {
		for (int j = prefixs[i].GetLength(); j < max_prefixs_length; j++) {
			prefixs[i].AppendChar(' ');
		}
		CStringATL strItem;
		strItem.Format("%s--%s\r\n", prefixs[i], subfixs[i]);
		strMatchBets += strItem;

	}
	strMatchBets += "};\r\n";
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

LRESULT BeiDanDialog::OnRefreshBiFen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	doBiFen();
	//BeiDanStat dlg(m_pDatabase);
	//dlg.DoModal();
	return 1L;
}

void BeiDanDialog::InitControls() {
	mMatchListFont.CreateFont(/*32*/-14, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_NORMAL, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		DEFAULT_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("微软雅黑"));
	m_lstMatch.SetFont(mMatchListFont);
	mBetAreaFont.CreateFont(/*32*/-12, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_NORMAL, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		DEFAULT_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("微软雅黑"));
	//m_stBetArea.SetFont(mBetAreaFont);


	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, 
			GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, 
			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
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

	m_lstResult.InsertColumn(colIndex, "倍数", LVCFMT_CENTER, 90);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstResult.InsertColumn(colIndex, "赔率", LVCFMT_CENTER, 90);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstResult.InsertColumn(colIndex, "结果", LVCFMT_CENTER, rcItem.Width() - 340);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	//m_lstMatch.setItemH
	//set sort type
}

void BeiDanDialog::ReloadMatchListData() {
	m_lstMatch.DeleteAllItems();

	auto& iter = m_JCMatchItems.begin();// equal_range((LPCSTR)m_strQH);
	int iIndex = 0;
	for (; iter != m_JCMatchItems.end(); ++iter) {
		if (iter->first.compare(m_strQH) != 0) {
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
			temp.Format("%.2f  %.2f  %.2f", a, b, c);
		}
		m_lstMatch.SetItemText(iIndex, ++colIndex, temp);

		sub = ji->get_subject(1, 3);
		a = sub->odds;
		sub = ji->get_subject(1, 1);
		b = sub->odds;
		sub = ji->get_subject(1, 0);
		c = sub->odds;
		if (ji->hand < 0) {
			temp.Format("%.2f(%d)  %.2f  %.2f", a, (int)ji->hand, b, c);
		} else {
			temp.Format("%.2f(+%d)  %.2f  %.2f", a, (int)ji->hand, b, c);
		}
		m_lstMatch.SetItemText(iIndex, ++colIndex, temp);
	}
	m_lstMatch.DoSortItems(0, false);
	m_CurrentMatchItem.reset();
	m_stBetArea.Invalidate();
}

void BeiDanDialog::CreateWorkDir() {
	CStringATL strPath(Global::GetAppPath().c_str());
	m_strRootDir = strPath + _T("JC");
	CStringATL strInitFileName;
	CreateDirectory(m_strRootDir, NULL);
	strPath = m_strRootDir + _T("\\") + m_strQH;
	CreateDirectory(strPath, NULL);
	m_strWorkDir = strPath;
	m_strWorkDir.Replace(".\\", "");
}

BOOL BeiDanDialog::GetItemFromDB(const JCMatchItem& new_item, JCMatchItem& item) {
	const std::string& id = new_item.id;
	item.id = "";
	item.subjects.clear();
	CStringATL strSQL;
	strSQL.Format(_T("SELECT ID, CATEGORY, DESCRIPTION, HAND, START_TIME, BUY_TIME, SUBJECTS, RESULT, MATCH_URL FROM BDZQ WHERE ID='%s'"), id.c_str());
	SQLite::Statement sm(*m_pDatabase, strSQL);
	if (sm.executeStep()) {
		item.id = sm.getColumn(0).getString().c_str();
		item.match_category = sm.getColumn(1).getString();
		item.descrition = sm.getColumn(2).getString();
		item.hand = sm.getColumn(3).getInt64();
		item.start_time = sm.getColumn(4).getString();
		item.last_buy_time = sm.getColumn(5).getString();
		std::string data = sm.getColumn(6).getString();
		item.result = sm.getColumn(7).getString();
		item.match_url = sm.getColumn(8).getString();
		CStlStrArray arrBetItems;
		Global::DepartString(data, "|", arrBetItems);
		for (auto& bet : arrBetItems) {
			CStlStrArray arrBetInfos;
			Global::DepartString(bet, ";", arrBetInfos);
			if (arrBetInfos.size() != 3) return FALSE;
			JCMatchItem::Subject sub;
			sub.tid = atoi(arrBetInfos[0].c_str());
			sub.betCode = atoi(arrBetInfos[1].c_str());
			sub.odds = atof(arrBetInfos[2].c_str());
			sub.calcTip(item.hand);
			sub.checked = false;
			item.subjects.push_back(sub);
		}
		item.subjects = new_item.subjects;
		if (!new_item.match_url.empty()) {
			item.match_url = new_item.match_url;
		}
		data.clear();
		for (auto& bet : item.subjects) {
			CStringATL betInfo;
			if (data.empty()) {
				betInfo.Format("%d;%d;%.2f", (int)bet.tid, (int)bet.betCode, bet.odds);
			} else {
				betInfo.Format("|%d;%d;%.2f", (int)bet.tid, (int)bet.betCode, bet.odds);
			}
			data += betInfo;
		}
		strSQL = _T("UPDATE BDZQ SET SUBJECTS=?, MATCH_URL=? WHERE ID=?");
		if (TRUE) {
			SQLite::Statement sm(*m_pDatabase, strSQL);
			sm.bindNoCopy(1, data);
			sm.bindNoCopy(2, new_item.match_url);
			sm.bindNoCopy(3, id);
			if (sm.exec()) {
				return TRUE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL BeiDanDialog::InsertItemToDB(const JCMatchItem& item) {
	CStringATL strSQL = _T("DELETE FROM BDZQ WHERE ID=?");
	if (TRUE) {
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bind(1, item.id);
		if (sm.exec() > 0) {
		}
	}
	strSQL = _T("INSERT INTO BDZQ (ID, CATEGORY, DESCRIPTION, HAND, START_TIME, BUY_TIME, SUBJECTS, MATCH_URL) VALUES(?,?,?,?,?,?,?,?)");
	if (TRUE) {
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bindNoCopy(1, item.id);
		sm.bind(2, item.match_category);
		sm.bind(3, item.descrition);
		sm.bind(4, item.hand);
		sm.bindNoCopy(5, item.start_time);
		sm.bindNoCopy(6, item.last_buy_time);
		std::string data;
		for (auto& bet : item.subjects) {
			CStringATL betInfo;
			if (data.empty()) {
				betInfo.Format("%d;%d;%.2f", (int)bet.tid, (int)bet.betCode, bet.odds);
			} else {
				betInfo.Format("|%d;%d;%.2f", (int)bet.tid, (int)bet.betCode, bet.odds);
			}
			data += betInfo;
		}
		sm.bindNoCopy(7, data);
		sm.bindNoCopy(8, item.match_url);
		if (sm.exec() > 0) {
		}
	}
	return TRUE;
}

BOOL BeiDanDialog::UpdateItemResultToDB(const std::string& id, const std::string& result) {
	CStringATL strSQL;
	strSQL.Format(_T("SELECT ID FROM BDZQ WHERE ID='%s'"), id.c_str());
	if (TRUE) {
		SQLite::Statement sm(*m_pDatabase, strSQL);
		if (!sm.executeStep()) {
			return FALSE;
		}
	}
	strSQL = _T("UPDATE BDZQ SET RESULT=? WHERE ID=?");
	if (TRUE) {
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bind(1, result);
		sm.bind(2, id);
		if (sm.exec()) {
			return TRUE;
		}
	}
	return FALSE;
}

void BeiDanDialog::DoRefreshBetArea() {
	for (auto& item : m_JCMatchItems) {
		for (auto& sub : item.second->subjects) {
			sub.checked = false;
		}
	}
	if (m_Engine.get() != nullptr) {
		for (const auto& r : m_Engine->getSource()) {
			for (const auto& item : r.bets) {
				JCMatchItem::Subject* sub = get_subjects(r.id, item.tid, item.code);
				if (sub != NULL) {
					sub->checked = true;
				}
			}
		}
	}
	m_stBetArea.Invalidate();
}

CStringATL BeiDanDialog::DoRefreshResultListResults(std::string& abuyLines, std::string& acheckLines) {
	abuyLines.clear();
	acheckLines.clear();
	CStringATL result = "结果列表:";
	m_lstResult.DeleteAllItems();
	if (m_Engine.get() == nullptr) {
		return result;
	}
	char sz[64] = { '\0' };
	std::map<CStringATL, std::tuple<int, double, CStringW, CStringATL>> rows;
	for (const auto& r : m_Engine->getResult()) {
		double bouns = 2.0;
		CStringATL strCodes; 
		CStringW strBuyLine;
		CStringATL strCheckLine;
		for (const auto& item : r) {
			bouns = bouns * item.bet.odds;
			JCMatchItem::Subject sub ;
			sub.tid = item.bet.tid;
			sub.betCode = item.bet.code;
			sub.calcTip(item.bet.hand);
			const std::string&& temp = sub.buyStr();
			sprintf(sz, "[%s]%s(%.2f)", item.id.c_str(), temp.c_str(), item.bet.odds);
			if (!strCodes.IsEmpty()) {
				strCodes += ",";
			}
			strCodes += sz;
			JCMatchItem m;
			CStringA match_descrition = item.id.c_str();
			/*
			if (GetItemFromDB(item.id, m)) {
				match_descrition = m.descrition.c_str();
			}
			*/
			match_descrition = match_descrition.Right(3);
			match_descrition.Trim();
			sprintf(sz, "[%s]: %s", (LPCSTR)match_descrition, temp.c_str());
			CStringW buyItem = CA2W(sz, CP_ACP).m_psz;
			for (UINT i = buyItem.GetLength(); i < 12; i++) {
				buyItem.AppendChar(L' ');
			}
			int nChineseCount = 0;
			for (UINT i = 0; i < buyItem.GetLength(); i++) {
				if (buyItem[i] > 127) {
					nChineseCount++;
				}
			}
			if ((nChineseCount % 2) != 0) {
				buyItem.AppendChar(L' ');
			}
			if (strBuyLine.IsEmpty()) {
				strBuyLine = buyItem;
			} else {
				//strBuyLine.Append(L"    ");
				strBuyLine.Append(buyItem);
			}
			sprintf(sz, "%s,%d,%d", item.id.c_str(), item.bet.tid, item.bet.code);
			if (strCheckLine.IsEmpty()) {
				strCheckLine = sz;
			} else {
				strCheckLine.AppendChar(';');
				strCheckLine.Append(sz);
			}
		}
		//sprintf(sz, "  %dX1..", r.size());
		//strBuyLine += sz;
		auto& iter = rows.find(strCodes);
		if (iter != rows.end()) {
			std::get<0>(iter->second) += 1;
			std::get<1>(iter->second) += bouns;
		} else {
			std::tuple<int, double, CStringW, CStringATL> tup;
			std::get<0>(tup) = 1;
			std::get<1>(tup) = bouns;
			std::get<2>(tup) = strBuyLine;
			std::get<3>(tup) = strCheckLine;
			rows[strCodes] = tup;
		}
	}
	CStringW buyLines;
	CStringATL checkLines;
	std::multimap<int, CStringW> mBuyLines;
	for (const auto& row : rows) {
		int multiple = std::get<0>(row.second);
		double bonus = std::get<1>(row.second);
		CStringW buyLine = std::get<2>(row.second);
		CStringATL checkLine = std::get<3>(row.second);
		buyLine.AppendFormat(L"  %d倍\n", multiple);
		//abuyLines.append(CW2A(buyLine, CP_UTF8).m_psz);
		checkLine.AppendFormat("|%d|%.2f\n", multiple, bonus);
		acheckLines.append(checkLine);
		mBuyLines.insert(std::pair<int, CStringW>(multiple, buyLine));
	}
	abuyLines.clear();
	int cur_multiple = 0;
	int line_count = 0;
	for (auto& buy : mBuyLines) {
		line_count++;
		if (abuyLines.empty()) {
			abuyLines = CW2A(buy.second, CP_UTF8).m_psz;
			cur_multiple = buy.first;
		} else {
			if (cur_multiple != buy.first) {
				abuyLines.append("\n");
				cur_multiple = buy.first;
			}
			if ((line_count % 5) == 0) {
				//abuyLines.append("\n");
			}
			abuyLines.append(CW2A(buy.second, CP_UTF8).m_psz);
		}
	}
	int index = 0;
	double maxBonus = 0.0, minBonus = -1;
	for (const auto& row : rows) {
		int colIndex = 0;
		CStringATL strResult;
		strResult.Format("%d", index + 1);
		int lIndex = m_lstResult.InsertItem(index++, strResult);
		strResult.Format("%.2f", std::get<1>(row.second));
		m_lstResult.SetItemText(lIndex, ++colIndex, strResult);
		strResult.Format("%d", std::get<0>(row.second));
		m_lstResult.SetItemText(lIndex, ++colIndex, strResult);
		strResult.Format("%.2f", std::get<1>(row.second) / std::get<0>(row.second)/ 2);
		m_lstResult.SetItemText(lIndex, ++colIndex, strResult);
		m_lstResult.SetItemText(lIndex, ++colIndex, row.first);
		double bonus = std::get<1>(row.second);
		if (bonus < minBonus || minBonus < 0) {
			minBonus = bonus;
		}
		if (bonus > maxBonus) {
			maxBonus = bonus;
		}
	}
	double maxBonusRatio = (maxBonus - 2 * m_Engine->getResult().size()) / (2 * m_Engine->getResult().size()) * 100;
	double minBonusRatio = (minBonus - 2 * m_Engine->getResult().size()) / (2 * m_Engine->getResult().size()) * 100;
	CStringATL maxBonusRate, minBonusRate;
	maxBonusRate.Format("%d", (int)maxBonusRatio); maxBonusRate += "%";
	minBonusRate.Format("%d", (int)minBonusRatio); minBonusRate += "%";
	result.Format("结果列表(共%d注,  投产比范围[%s-%s], 收益范围[%.2f-%.2f]):",
		m_Engine->getResult().size(), 
		minBonusRate, maxBonusRate,
		minBonus - 2 * m_Engine->getResult().size(), maxBonus - 2 * m_Engine->getResult().size()
	);
	return result;
}

void BeiDanDialog::DoRefreshMatchListResults() {
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

void BeiDanDialog::DoReloadBackupResult() {
	CStringATL strSQL;
	strSQL.Format(_T("SELECT ID, SCRIPT FROM BDZQ_INVEST WHERE DATE='%s'"), m_strQH);
	if (TRUE) {
		SQLite::Statement sm(*m_pDatabase, strSQL);
		while (sm.executeStep()) {
			std::string id = sm.getColumn(0).getString();
			std::string script = sm.getColumn(1).getString();
			std::string fp = m_strWorkDir + "\\" + id.c_str() + ".lua";
			if (!Global::IsFileExist(fp.c_str())) {
				Global::SaveFileData(fp, script, FALSE);
			}
		}
	}
}

JCMatchItem::Subject* BeiDanDialog::get_subjects(const std::string& id, int tid, int code) {
	for (const auto& item : m_JCMatchItems) {
		if (item.second->id == id) {
			return item.second->get_subject(tid, code);
		}
	}
	return NULL;
}

void BeiDanDialog::ShowMatchWebBrowser(const CStringATL& title) {
	if (m_CurrentMatchItem.get() == nullptr) {
		return;
	}
	if (m_CurrentMatchItem->match_url.empty()) {
		return;
	}
	std::shared_ptr<WebBrowser> ptr;
	const auto& iter = m_Browsers.find(m_CurrentMatchItem->match_url);
	if (iter == m_Browsers.end()) {
		ptr = WebBrowser::CreateWebBrowser(m_hWnd, (LPCSTR)title,
			m_CurrentMatchItem->match_url, this);
		m_Browsers[m_CurrentMatchItem->match_url] = ptr;
	}
	else {
		if (iter->second->IsWindowDestroyed()
			|| iter->second->IsWindowDestroying()) {
			m_delayDeleteBrowsers.push_back(iter->second);
			m_Browsers.erase(iter);
			ptr = WebBrowser::CreateWebBrowser(m_hWnd, (LPCSTR)title,
				m_CurrentMatchItem->match_url, this);
			m_Browsers[m_CurrentMatchItem->match_url] = ptr;
		} else {
			ptr = iter->second;
		}
	}
	if (ptr.get() != nullptr && ptr->IsWindow()) {
		ShowWindow(ptr->IsIconic() ? SW_RESTORE : SW_SHOW);
		::SetForegroundWindow(ptr->m_hWnd);
	}
}

void BeiDanDialog::CloseMatchWebBrowsers() {
	for (auto& iter : m_Browsers) {
		if (iter.second->IsWindowDestroyed()
			|| iter.second->IsWindowDestroying()) {
			m_delayDeleteBrowsers.push_back(iter.second);
		}
		else {
			m_delayDeleteBrowsers.push_back(iter.second);
			iter.second->DestroyWindow();
		}
	}
	m_Browsers.clear();
}

void BeiDanDialog::onWebBrowserClose(const std::string& url) {
	const auto& iter = m_Browsers.find(m_CurrentMatchItem->match_url);
	if (iter != m_Browsers.end()) {
		m_delayDeleteBrowsers.push_back(iter->second);
		m_Browsers.erase(iter);
	}
	ShowWindow(IsIconic()? SW_RESTORE:SW_SHOW);
	::SetForegroundWindow(m_hWnd);
}
