#include "stdafx.h"
#include "DanLueDialog.h"
#include "Global.h"
//"http://appserver.87.cn/jc/match"

static const CStlString DZ_FILTER_NAME = _T("����ļ�(*.txt)");
static const CStlString DZ_FILTER = _T("*.txt");

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
			menu.AppendMenu(MF_STRING, 100, _T("���ѡ��"));
			m_lstMatch.ClientToScreen(&pt);
			UINT cmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
			DoMatchListMenuCommand(cmd, index);
			menu.DestroyMenu();
		}
	}
	return lRet;
}

void DanLueDialog::DoMatchListMenuCommand(UINT cmd, UINT index) {
	if (cmd == 100) {
		CStringATL strID;
		m_lstMatch.GetItemText(index, 0, strID);
		auto& iter = m_JCMatchItems.begin();
		for (; iter != m_JCMatchItems.end(); ++iter) {
			if (iter->second->id.compare(strID) == 0) {
				m_CurrentMatchItem = iter->second;
				for (auto& sub : m_CurrentMatchItem->subjects) {
					sub.checked = false;
				}
				m_lstMatch.SetItemText(index, 6, "");
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




/*
LRESULT DanLueDialog::OnListLButtonDbclk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	LRESULT lRet = m_lstMatch.DefWindowProc(uMsg, wParam, lParam);
	return lRet;
}
*/

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
		MessageBox("Lua�ű��ļ���ȡʧ�� 0��", "����", MB_ICONERROR | MB_OK);
		return 1L;
	}
	if (!Global::ReadFileData((LPCTSTR)strLuaFile, strScript) || strScript.empty()) {
		MessageBox("Lua�ű��ļ���ȡʧ�� 1��", "����", MB_ICONERROR | MB_OK);
		return 1L;
	}
	DanLueEngine engine(strScript);
	if (engine.CalculateAllResult(reason)) {
		strLuaFile.Format(_T("��%s��������ɣ� ���С�%u��ע����� �Ƿ񱣴棿"), m_strQH, engine.GetResult().size());
		if (MessageBox(strLuaFile, "���", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			DoSaveResult(engine);
		}
	} else {
		strLuaFile.Format(_T("��%s������ʧ�ܣ�"), m_strQH);
		MessageBox(strLuaFile, "����", MB_ICONERROR | MB_OK);
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
	return 1L;
}

LRESULT DanLueDialog::OnUpload(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT DanLueDialog::OnClearAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	return 1L;
}

LRESULT DanLueDialog::OnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	doJcMatchList();
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

	m_lstResult.GetClientRect(rcItem);
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
	m_lstMatch.InsertColumn(colIndex, "�ں�", LVCFMT_CENTER, 100);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "����", LVCFMT_CENTER, 100);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "����", LVCFMT_CENTER, 250);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstMatch.InsertColumn(colIndex, "����ʱ��", LVCFMT_CENTER, 130);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);
	m_lstMatch.InsertColumn(colIndex, "ʤƽ��", LVCFMT_CENTER, 200);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstMatch.InsertColumn(colIndex, "��ʤƽ��", LVCFMT_CENTER, 200);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_NONE);
	m_lstMatch.InsertColumn(colIndex, "ѡ����", LVCFMT_CENTER, 400);    //70
	m_lstMatch.SetColumnSortType(colIndex++, LVCOLSORT_TEXT);

	m_lstMatch.SetSortColumn(0);

	m_lstResult.GetClientRect(rcItem);
	colIndex = 0;
	m_lstResult.InsertColumn(colIndex, "���", LVCFMT_CENTER, 70);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstResult.InsertColumn(colIndex, "����(Ԫ)", LVCFMT_CENTER, 90);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_LONG);

	m_lstResult.InsertColumn(colIndex, "���", LVCFMT_CENTER, rcItem.Width() - 160);    //70
	m_lstResult.SetColumnSortType(colIndex++, LVCOLSORT_NONE);

	//m_lstMatch.setItemH
	//set sort type
}



void DanLueDialog::ReloadMatchListData() {
	m_lstMatch.DeleteAllItems();
	
	auto& iter = m_JCMatchItems.begin();// equal_range((LPCSTR)m_strQH);
	int iIndex = 0;
	while (iter != m_JCMatchItems.end()) {
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
			temp = "δ �� ��";
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

		++iter;
	}
	m_lstMatch.DoSortItems(0, false);
	m_CurrentMatchItem.reset();
	m_stBetArea.Invalidate();
}

/*
void DanLueDialog::ReloadStatisData() {
	return;
	CStlString strTextFile = Global::GetAppPath() + _T("jqc.txt");
	if (PathFileExists(strTextFile.c_str())) {
		m_lstMatch.DeleteAllItems();
		std::string filedate;
		Global::ReadFileData(strTextFile, filedate);
		std::vector<CStlString> arrLines;
		Global::DepartString(filedate, _T("\r\n"), arrLines);
		int iIndex = 0, maxQH = 0;
		for (const auto& line : arrLines) {
			int colIndex = 0;
			std::vector<CStlString> arrParts;
			Global::DepartString(line, _T(","), arrParts);
			if (arrParts.size() != 4) {
				continue;
			}
			if (arrParts[1].size() != 8) {
				continue;
			}
			int qh = _ttol(arrParts[0].c_str());
			if (qh > maxQH) {
				maxQH = qh;
			}
			iIndex = m_lstMatch.InsertItem(iIndex, arrParts[0].c_str());
			m_lstMatch.SetItemText(iIndex, ++colIndex, arrParts[3].c_str());
			m_lstMatch.SetItemText(iIndex, ++colIndex, arrParts[2].c_str());
			m_lstMatch.SetItemText(iIndex, ++colIndex, arrParts[1].c_str());
			CStlString& codes = arrParts[1];
			CIntArray arrCodes;
			int sumAll = 0;
			for (int j = 0; j < 8; j++) {
				int code = (codes[j] - _T('0'));
				sumAll += code;
				arrCodes.push_back(code);
			}
			CStringATL strNum;
			sprintf(strNum.GetBuffer(255), "%u", sumAll);
			strNum.ReleaseBuffer();
			m_lstMatch.SetItemText(iIndex, ++colIndex, strNum);
			sprintf(strNum.GetBuffer(255), "%02u-%02u-%02u-%02u", arrCodes[1] + arrCodes[0],
				arrCodes[3] + arrCodes[2], arrCodes[5] + arrCodes[4], arrCodes[7] + arrCodes[6]);
			strNum.ReleaseBuffer();
			m_lstMatch.SetItemText(iIndex, ++colIndex, strNum);
		}
		m_strQH.Format(_T("%u"), maxQH + 1);
		CreateWorkDir();
	}
}
*/

void DanLueDialog::CreateWorkDir() {
	CStringATL strPath(Global::GetAppPath().c_str());
	m_strRootDir = strPath + _T("JC");
	CStringATL strInitFileName;
	CreateDirectory(m_strRootDir, NULL);
	strPath = m_strRootDir + _T("\\") + m_strQH;
	CreateDirectory(strPath, NULL);
	m_strWorkDir = strPath;
}

void DanLueDialog::DoSaveResult(DanLueEngine& engine) {
	CStlString strResult;
	engine.GetResultString(strResult);
	if (strResult.empty()) {
		return;
	}
	TCHAR szFilterName[30] = { _T('\0') };
	_tcscpy(szFilterName, DZ_FILTER_NAME.c_str());
	_tcscat(szFilterName + DZ_FILTER_NAME.length() + 1, DZ_FILTER.c_str());
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilterName, m_hWnd);
	dlg.m_ofn.lpstrInitialDir = m_strWorkDir;
	CStringATL strInitFileName;
	strInitFileName.Format(_T("%s.txt"), m_strQH);
	TCHAR szFileName[MAX_PATH + 1] = { _T('\0') };
	_tcscpy(szFileName, strInitFileName);
	dlg.m_ofn.lpstrFile = szFileName;
	if (dlg.DoModal() != IDOK) {
		return;
	}
	CStlString strLoadPath = dlg.m_ofn.lpstrFile;
	std::string utf8 = Global::toUTF8((LPCTSTR)strResult.c_str());
	Global::SaveFileData(strLoadPath, utf8, FALSE);
}
