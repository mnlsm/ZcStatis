#include "stdafx.h"
#include "DialogTZ.h"
#include "Global.h"
#include "Engine.h"
#include "EngineLua.h"

CDialogTZ::CDialogTZ(const std::shared_ptr<SQLite::Database>& db, const CStlString& strWorkDir,
	const CStlString& qh, int gambleID) {
	m_pDatabase = db;
	m_strWorkDir = strWorkDir;
	m_strRecommDir = m_strWorkDir + _T("\\recomm");
	m_strQH = qh;
	m_GambleID = gambleID;
	m_bDataChanged = FALSE;
    m_brush = GetSysColorBrush(COLOR_MENU);
}

BOOL CDialogTZ::IsDbDataChanged() {
	return m_bDataChanged;
}

LRESULT CDialogTZ::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    HWND wnd = (HWND)lParam;
    HDC dc = (HDC)wParam;
    if(wnd == GetDlgItem(IDC_STDI) || wnd == GetDlgItem(IDC_STSHOWQI)
            || wnd == GetDlgItem(IDC_STQI)) {
        ::SetTextColor(dc, RGB(0, 0, 255));
        ::SetBkMode(dc, TRANSPARENT);
        return (LRESULT)(HBRUSH)m_brush;
    }
    bHandled = FALSE;
    return 1L;
}

LRESULT CDialogTZ::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CenterWindow();
	initConctrols();
	ReLoadDataToShow(true);
    return TRUE;
}

LRESULT CDialogTZ::OnResultSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CComboBox co = GetDlgItem(wID);
	if (co.GetCurSel() == 0) {
		return 1;
	}
	int pos = wID - IDC_CORESULT1;
	int pos1 = pos + 1;
	int pos2 = pos + 2;
	if ((pos % 3) == 1) {
		pos1 = pos - 1;
		pos2 = pos + 1;
	} else if ((pos % 3) == 2) {
		pos1 = pos - 1;
		pos2 = pos - 2;
	}
	pos1 += IDC_CORESULT1;
	pos2 += IDC_CORESULT1;
	co = GetDlgItem(pos1);
	co.SetCurSel(0);
	co = GetDlgItem(pos2);
	co.SetCurSel(0);
	return 1L;
}

LRESULT CDialogTZ::OnRecommendMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	if (wID == IDM_RECOMMEND_ONE) {
		for (int i = 0; i < TOTO_COUNT; i++) {
			UINT ctlID = IDC_CORESULT1 + i * 3;
			CComboBox co = GetDlgItem(ctlID);
			co.SetCurSel(1);
			co = GetDlgItem(ctlID + 1);
			co.SetCurSel(0);
			co = GetDlgItem(ctlID + 2);
			co.SetCurSel(0);
		}
	}
	else if (wID == IDM_RECOMMEND_TWO) {
		for (int i = 0; i < TOTO_COUNT; i++) {
			UINT ctlID = IDC_CORESULT2 + i * 3;
			CComboBox co = GetDlgItem(ctlID);
			co.SetCurSel(1);
			co = GetDlgItem(ctlID + 1);
			co.SetCurSel(0);
			co = GetDlgItem(ctlID - 1);
			co.SetCurSel(0);
		}
	}
	else if (wID == IDM_RECOMMEND_THREE) {
		for (int i = 0; i < TOTO_COUNT; i++) {
			UINT ctlID = IDC_CORESULT3 + i * 3;
			CComboBox co = GetDlgItem(ctlID);
			co.SetCurSel(1);
			co = GetDlgItem(ctlID - 1);
			co.SetCurSel(0);
			co = GetDlgItem(ctlID - 2);
			co.SetCurSel(0);
		}
	} else if (wID == IDM_RECOMMEND_RECOMMEND_TWO) {
		DoRecommendTwoChoice();
	}
	return 1L;
}


LRESULT CDialogTZ::OnClickedBuAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	CStlString strResults;
	CStringATL strError;
	if (!ReadUserChoice(strResults, strError)) {
		MessageBox(strError, _T("错误"), MB_OK | MB_ICONWARNING);
		return 0;
	}
	BOOL bRet = DoUpdateDatabase(strResults);
	if (bRet) {
		CStringATL text = (m_GambleID == -1) ? _T("添加成功") : _T("更新成功");
		MessageBox(text, _T("提示"), MB_OK | MB_ICONWARNING);
		if (m_GambleID == -1) {
			EndDialog(IDOK);
		}
	} else {
		CStringATL text = (m_GambleID == -1) ? _T("添加失败") : _T("更新失败");
		MessageBox(text, _T("错误"), MB_OK | MB_ICONWARNING);
	}
	return 0;
}

LRESULT CDialogTZ::OnClickedBuClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	ClearUserChoice();
	return 0;
}

LRESULT CDialogTZ::OnClickedBuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	EndDialog(IDCANCEL);
	return 0;
}

void CDialogTZ::initConctrols() {
	HICON hIconBig = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR|LR_SHARED, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	SetIcon(hIconBig, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR | LR_SHARED, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON));
	SetIcon(hIconBig, TRUE);
	SetIcon(hIconSmall, FALSE);

    //init static data
    m_wndQI = GetDlgItem(IDC_STSHOWQI);
	if (m_GambleID != -1) {
		SetWindowText("更新选择");
		CWindow wnd = GetDlgItem(IDC_BUADD);
		wnd.SetWindowText(_T("更新"));
	}
	m_MenuButton = GetDlgItem(IDC_BUSET);
	m_MenuButton.AddMenuItem(IDM_RECOMMEND_ONE, _T("单选"));
	m_MenuButton.AddMenuItem(IDM_RECOMMEND_TWO, _T("双选"));
	m_MenuButton.AddMenuItem(IDM_RECOMMEND_THREE, _T("三选"));

	m_MenuButton.AddMenuItem(0, _T(""), MF_SEPARATOR);
	m_MenuButton.AddMenuItem(IDM_RECOMMEND_RECOMMEND_TWO, _T("双选优化"));

	
}

struct GvItem {
	CStlString code;
	double gv;
};

bool operator<(const GvItem &x, const GvItem &y) {
	return x.gv > y.gv;
}

void CDialogTZ::initComboxes() {
	CDoublexyArray arrPls, arrGvs;
	CEngine::GetPLDatas(m_strPL, arrPls, arrGvs);

	GvItem items[TOTO_COUNT][3];;
	for (int i = 0; i < TOTO_COUNT; i++) {
		for (int j = 0; j < 3; j++) {
			if (j == 0) {
				items[i][j].code = _T("3");
			} else if (j == 1) {
				items[i][j].code = _T("1");
			} else {
				items[i][j].code = _T("0");
			}
			items[i][j].gv = arrGvs[i][j];
		}
		GvItem* start = &items[i][0];
		std::stable_sort(start, start + 3);
	}
	CComboBox coWnd = NULL;
	for (int i = 0; i < TOTO_COUNT; i++) {
		coWnd = GetDlgItem(i * 3 + IDC_CORESULT1);
		coWnd.AddString(_T(" "));
		coWnd.AddString(items[i][0].code.c_str());
		coWnd.AddString(items[i][1].code.c_str());
		coWnd.AddString(items[i][2].code.c_str());

		coWnd = GetDlgItem(i * 3 + 1 + IDC_CORESULT1);
		coWnd.AddString(_T(" "));
		coWnd.AddString((items[i][0].code + items[i][1].code).c_str());
		coWnd.AddString((items[i][0].code + items[i][2].code).c_str());
		coWnd.AddString((items[i][1].code + items[i][2].code).c_str());

		coWnd = GetDlgItem(i * 3 + 2 + IDC_CORESULT1);
		coWnd.AddString(_T(" "));
		coWnd.AddString((items[i][0].code + items[i][1].code + items[i][2].code).c_str());
	}
}


BOOL CDialogTZ::ReadUserChoice(CStlString &strResults, CStringATL& strErrInfo) {
	strResults.clear();
	strErrInfo.Empty();
	for (int i = IDC_CORESULT1; i <= IDC_CORESULT42; i = i + 3) {
		CStringATL result;
		CComboBox co1 = GetDlgItem(i);
		CComboBox co2 = GetDlgItem(i + 1);
		CComboBox co3 = GetDlgItem(i + 2);
		if (co1.GetCurSel() > 0) {
			co1.GetLBText(co1.GetCurSel(), result);
		} else if (co2.GetCurSel() > 0) {
			co2.GetLBText(co2.GetCurSel(), result);
		} else if (co3.GetCurSel() > 0) {
			co3.GetLBText(co3.GetCurSel(), result);
		}
		if (result.IsEmpty()) {
			CStringATL strTemp;
			CWindow wnd = GetDlgItem(IDC_STMATCHONE1 + (i - IDC_CORESULT1) / 3);
			wnd.GetWindowText(strTemp);
			strErrInfo.Format(_T("请选择结果： %s"), strTemp);
			return FALSE;
		}
		if (strResults.empty()) {
			strResults = result;
		} else {
			strResults = strResults + _T(",") + (LPCTSTR)result;
		}
	}
	return TRUE;
}

void CDialogTZ::ClearUserChoice() {
	CComboBox coWnd = NULL;
	for (int i = IDC_CORESULT1; i <= IDC_CORESULT42; i++) {
		coWnd = GetDlgItem(i);
		if (coWnd.IsWindow()) coWnd.SetCurSel(0);
	}
}

BOOL CDialogTZ::ReLoadDataToShow(BOOL first) {
	m_wndQI.SetWindowText(m_strQH.c_str());
	ClearUserChoice();
	if (m_GambleID != -1) {
		CStringATL strSQL, strResults;
		strSQL.Format(_T("SELECT CODES, PLDATA, MATCHS FROM GAMBEL WHERE ID=%d"), m_GambleID);
		SQLite::Statement sm(*m_pDatabase, strSQL);
		if (sm.executeStep()) {
			strResults = sm.getColumn(0).getString().c_str();
			m_strPL = sm.getColumn(1).getString().c_str();
			m_strMatchs = sm.getColumn(2).getString().c_str();
		}

		if (first) {
			initComboxes();
		}

		CStlStrArray arrResults;
		Global::DepartString((LPCTSTR)strResults, _T(","), arrResults);
		for (int i = IDC_CORESULT1; i <= IDC_CORESULT42; i = i + 3) {
			int arrPos = (i - IDC_CORESULT1) / 3;
			int ctrlPos = i + arrResults[arrPos].length() - 1;
			CComboBox coWnd = GetDlgItem(ctrlPos);
			if (coWnd.IsWindow()) {
				int index = coWnd.FindStringExact(0, arrResults[arrPos].c_str());
				if (index >= 0) {
					coWnd.SetCurSel(index);
				} else {
					if (arrResults[arrPos] == _T("31")) {
						index = coWnd.FindStringExact(0, "13");
						coWnd.SetCurSel(index);
					} else if (arrResults[arrPos] == _T("30")) {
						index = coWnd.FindStringExact(0, "03");
						coWnd.SetCurSel(index);
					} else if (arrResults[arrPos] == _T("10")) {
						index = coWnd.FindStringExact(0, "01");
						coWnd.SetCurSel(index);
					}	else if (arrResults[arrPos] == _T("310")) {
						coWnd.SetCurSel(1);
					}
				}
			}
		}
	} else {
		CStringATL strSQL;
		strSQL.Format(_T("SELECT PLDATA, MATCHS FROM PLDATA WHERE ID='%s'"), m_strQH.c_str());
		SQLite::Statement sm(*m_pDatabase, strSQL);
		if (sm.executeStep()) {
			m_strPL = sm.getColumn(0).getString().c_str();
			m_strMatchs = sm.getColumn(1).getString().c_str();
		}
		if (first) {
			initComboxes();
		}
	}
	CStlStrArray arrMatchs;
	Global::DepartString(m_strMatchs, _T("\n"), arrMatchs);
	if (arrMatchs.size() == (IDC_STMATCHONE14 - IDC_STMATCHONE1) + 1) {
		for (int i = 0; i < arrMatchs.size(); i++) {
			int ctrlPos = i + IDC_STMATCHONE1;
			CStatic matchCtrl = GetDlgItem(ctrlPos);
			matchCtrl.SetWindowText(arrMatchs[i].c_str());
		}
	}
	return TRUE;
}

BOOL CDialogTZ::DoUpdateDatabase(const CStlString &strResults) {
	BOOL ret = FALSE;
	CStringATL strSQL;
	if (m_GambleID == -1) {
		strSQL = _T("INSERT INTO GAMBEL (QH, INUSE, CODESTYPE, CODES, PLDATA, MATCHS) VALUES(?,?,?,?,?,?)");
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bindNoCopy(1, m_strQH);
		sm.bind(2, 0);
		sm.bind(3, 0);
		sm.bindNoCopy(4, strResults);
		sm.bindNoCopy(5, m_strPL);
		sm.bindNoCopy(6, m_strMatchs);
		ret = (sm.exec() > 0);
	} else {                                 
		strSQL.Format(_T("UPDATE GAMBEL SET CODESTYPE=?, CODES=?, RESULT=? WHERE ID=%d"), m_GambleID);
		SQLite::Statement sm(*m_pDatabase, strSQL);
		sm.bind(1, 0);
		sm.bindNoCopy(2, strResults);
		sm.bindNoCopy(3, _T(""));
		ret = (sm.exec() > 0);
	}
	m_bDataChanged = TRUE;
	return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void gatherAllChoice(const CStlStrxyArray& arrChoiceXY, CStlStrxyArray& arrChoiceR, 
	CStlStrArray& codes, int index, int& count2) {
	if (index >= TOTO_COUNT) {
		if (codes.size() == TOTO_COUNT) {
			if (count2 == 8) {
				arrChoiceR.push_back(codes);
			}
		}
		return;
	}
	for (int i = index; i < arrChoiceXY.size(); i++) {
		const CStlStrArray& temp = arrChoiceXY[i];
		for (int j = 0; j < temp.size(); j++) {
			if (j == 0) {
				count2++;
			}
			codes.push_back(temp[j]);
			gatherAllChoice(arrChoiceXY, arrChoiceR, codes, i + 1, count2);
			if (j == 0) {
				count2--;
			}
			codes.pop_back();
		}
	}
}

static void filterChoiceR(CStlStrxyArray& arrChoiceR, CStringATL strScript) {
	const CStringATL tj_begin_mark = _T("--FILTER_RECOMM_XY_BEGIN");
	const CStringATL tj_end_mark = _T("--FILTER_RECOMM_XY_END");

	int pos = strScript.Find(tj_begin_mark);
	if (pos == -1) {
		return;
	}
	pos += tj_begin_mark.GetLength();
	int pos1 = strScript.Find(tj_end_mark);
	if (pos1 < pos) {
		return;
	}
	strScript = strScript.Mid(pos, pos1 - pos);
	strScript.Trim();
	strScript.Replace(_T("\r"), _T(""));
	strScript.Replace(_T("-"), _T(""));

	struct FilterX_Params {
		FilterX_Params() {
			clear();
		}
		int range_begin;
		int range_end;
		std::map<UINT, std::set<CStlString>> mapPosCodes;
		void clear() {
			range_begin = -1;
			range_end = -1;
			mapPosCodes.clear();
		}
	};

	std::vector<FilterX_Params> fps;
	CStlStrArray arrLines, arrPart;
	Global::DepartString((LPCTSTR)strScript, _T("\n"), arrLines);
	for (auto& line : arrLines) {
		FilterX_Params fp;
		Global::TrimBlank(line);
		Global::DepartString(line, _T("|"), arrPart);
		if (arrPart.size() != 5) continue;
		Global::TrimBlank(arrPart[0]);
		Global::TrimBlank(arrPart[1]);
		Global::TrimBlank(arrPart[2]);
		Global::TrimBlank(arrPart[3]);
		Global::TrimBlank(arrPart[4]);
		if (arrPart[0] != _T("F")) continue;
		CStlString& strIndexs = arrPart[1];
		CIntArray arrIndex;
		for (int i = 0; i < strIndexs.size(); i++) {
			if (strIndexs[i] >= _T('1') && strIndexs[i] <= _T('9')) {
				arrIndex.push_back(strIndexs[i] - _T('1'));
			}
			else if (strIndexs[i] == _T('A') || strIndexs[i] == _T('B')
				|| strIndexs[i] == _T('C') || strIndexs[i] == _T('D') || strIndexs[i] == _T('E'))
				arrIndex.push_back(strIndexs[i] - _T('A') + 9);
			else break;
		}
		CStlStrArray codes, codes1;
		Global::DepartString(arrPart[2], _T(","), codes);
		if (arrIndex.size() != codes.size()) continue;
		for (auto& code : codes) {
			Global::TrimBlank(code);
			CStlString bak = code;
			std::reverse(bak.begin(), bak.end());
			codes1.push_back(bak);
		}
		for (int i = 0; i < arrIndex.size(); i++) {
			if (fp.mapPosCodes.find(arrIndex[i]) == fp.mapPosCodes.end()) {
				fp.mapPosCodes[arrIndex[i]] = std::set<CStlString>();
			}
			fp.mapPosCodes[arrIndex[i]].insert(codes[i]);
			fp.mapPosCodes[arrIndex[i]].insert(codes1[i]);
		}
		fp.range_begin = _ttol(arrPart[3].c_str());
		fp.range_end = _ttol(arrPart[4].c_str());
		if (fp.range_begin < 0 || fp.range_end < 0) continue;
		if (fp.range_begin > fp.range_end) continue;
		fps.push_back(fp);
	}
	for (pos = arrChoiceR.size() - 1; pos >= 0; pos--) {
		CStlStrArray& record = arrChoiceR[pos];
		bool isFiltered = false;
		for (const auto& tj : fps) {
			int match_count = 0;
			for (const auto& pc : tj.mapPosCodes) {
				CStlString& record_code = record[pc.first];
				for (const auto& tj_code : pc.second) {
					if (record_code.find(tj_code) != CStlString::npos) {
						match_count++;
					}
				}
			}
			if (match_count < tj.range_begin || match_count > tj.range_end) {
				isFiltered = true;
				break;
			}
		}
		if (isFiltered) {
			arrChoiceR.erase(arrChoiceR.begin() + pos);
		}
	}









}


void CDialogTZ::DoRecommendTwoChoice() {
	CStringATL strLuaFile;
	CStlString strScript, reason;
	strLuaFile.Format(_T("%s\\%s_m2.lua"), m_strRecommDir.c_str(), m_strQH.c_str());
	if (!PathFileExists(strLuaFile)) {
		MessageBox("Lua脚本文件读取失败 0！", "错误", MB_ICONERROR | MB_OK);
		return;
	}
	if (!Global::ReadFileData((LPCTSTR)strLuaFile, strScript) || strScript.empty()) {
		MessageBox("Lua脚本文件读取失败 1！", "错误", MB_ICONERROR | MB_OK);
		return;
	}
	CStlStrxyArray arrChoiceXY, arrChoiceR;
	CStlStrArray codes;
	for (int i = 0; i < TOTO_COUNT; i++) {
		codes.clear();
		UINT ctlID = IDC_CORESULT2 + i * 3;
		CComboBox co = GetDlgItem(ctlID);
		CStringATL code;
		co.GetLBText(1, code);
		codes.push_back((LPCTSTR)code);
		code.Empty();
		co.GetLBText(2, code);
		codes.push_back((LPCTSTR)code);
		arrChoiceXY.push_back(codes);
	}
	codes.clear();
	std::map<CStlString, UINT> recommends;
	int count2 = 0;
	gatherAllChoice(arrChoiceXY, arrChoiceR, codes, 0, count2);
	filterChoiceR(arrChoiceR, strScript.c_str());
	strLuaFile.Format(_T("recomm: arrChoiceR size=[%u]"), arrChoiceR.size());
	OutputDebugStringA(strLuaFile);
	for (const auto& choice : arrChoiceR) {
		CStlString line;
		for (const auto& c : choice) {
			line = line + c + _T(",");
		}
		line.erase(line.end() - 1);
		recommends[line] = 0;
	}

	std::map<UINT, CStlStrArray, std::greater<UINT>> result;
	CIntxyArray resultDZ;
	UINT index = 0;
	for (auto& recomm : recommends) {
		std::unique_ptr<CEngine> pEngine;
		pEngine.reset(new CEngineLua(strScript));
		pEngine->SetChoices(recomm.first);
		pEngine->SetPL(m_strPL);
		reason.clear();
		if (pEngine->CalculateAllResult(reason)) {
			const CIntxyArray& curResult = pEngine->GetResult();
			recomm.second = curResult.size();
			if (result.find(recomm.second) == result.end()) {
				result[recomm.second] = CStlStrArray();
			}
			result[recomm.second].push_back(recomm.first);
			resultDZ.insert(resultDZ.end(), curResult.begin(), curResult.end());
			std::stable_sort(resultDZ.begin(), resultDZ.end());
			resultDZ.erase(std::unique(resultDZ.begin(), resultDZ.end()), resultDZ.end());
		}
		strLuaFile.Format(_T("recomm: code=[%s], result[%u], index = %u"), 
			recomm.first.c_str(), recomm.second, ++index);
		OutputDebugStringA(strLuaFile);
	}

	strLuaFile.Format(_T("%s\\%s_m2.comm"), m_strRecommDir.c_str(), m_strQH.c_str());
	strScript.clear();
	for (const auto& item : result) {
		CStringATL line;
		for (const auto& code : item.second) {
			line.Format(_T("%s    [%d]\n"), code.c_str(), item.first);
			strScript += line;
		}
	}
	Global::SaveFileData((LPCTSTR)strLuaFile, strScript, FALSE);

	strLuaFile.Format(_T("recomm: resultDZ count = [%u]"), resultDZ.size());
	OutputDebugStringA(strLuaFile);

	//保存单注
	strLuaFile.Format(_T("%s\\%s_m2.dz"), m_strWorkDir.c_str(), m_strQH.c_str());
	CEngine::GetRecordsString(resultDZ, strScript);
	std::string utf8 = Global::toUTF8(strScript);
	Global::SaveFileData((LPCTSTR)strLuaFile, utf8, FALSE);
}
