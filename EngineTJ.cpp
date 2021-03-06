#include "stdafx.h"
#include "Engine.h"
#include "Global.h"

// jugde exemple (H|1A|==|6|)
BOOL CEngine::IsFilterH(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr) {
	if (strTJ.find(_T("H")) == std::string::npos) {
		return FALSE;
	}
    //depart std::string
    CStlStrArray arrPart;
    TCHAR cbDim[] = _T("|");
    Global::DepartString(strTJ, cbDim, arrPart);
    if(arrPart.size() != 4) return FALSE;
    std::string strIndexs = arrPart[1];
    std::string strOperator = arrPart[2];
    long iMark = _ttol(arrPart[3].c_str());
    CIntArray arrIndex;
    for(int i = 0; i < strIndexs.size(); i++) {
		if (strIndexs[i] >= _T('1') && strIndexs[i] <= _T('9')) {
			arrIndex.push_back(strIndexs[i] - _T('1'));
		} else if (strIndexs[i] == _T('A') || strIndexs[i] == _T('B') || strIndexs[i] == _T('C')
			|| strIndexs[i] == _T('D') || strIndexs[i] == _T('E')) {
			arrIndex.push_back(strIndexs[i] - _T('A') + 9);
		} else {
			return FALSE;
		}
    }
    long lSum = 0;
    for(int i = 0; i < arrIndex.size(); i++) {
        int iIndex = arrIndex[i];
        lSum += tempArr[iIndex];
    }
    Global::TrimBlank(strOperator);

    if(strOperator.compare(_T(">=")) == 0) return lSum < iMark;
    else if(strOperator.compare(_T("<=")) == 0) return lSum > iMark;
    else if(strOperator.compare(_T(">")) == 0) return lSum <= iMark;
    else if(strOperator.compare(_T("<")) == 0) return lSum >= iMark;
    else if(strOperator.compare(_T("==")) == 0) return lSum != iMark;
    else if(strOperator.compare(_T("!=")) == 0) return lSum == iMark;

    return FALSE;
}

// jugde exemple (L|01A|>|0|1|)
BOOL CEngine::IsFilterL(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr) {
	if (strTJ.find(_T("L")) == std::string::npos) {
		return FALSE;
	}
    //depart std::string
    CStlStrArray arrPart;
    TCHAR cbDim[] = _T("|");
    Global::DepartString(strTJ, cbDim, arrPart);
    if(arrPart.size() != 5) return FALSE;
    std::string strIndexs = arrPart[1];
    std::string strOperator = arrPart[2];
    long iMarkCode = _ttol(arrPart[3].c_str());
    long lMarkCount = _ttol(arrPart[4].c_str());
    CIntArray arrIndex;
    for(int i = 0; i < strIndexs.size(); i++) {
		if (strIndexs[i] >= _T('1') && strIndexs[i] <= _T('9')) {
			arrIndex.push_back(strIndexs[i] - _T('1'));
		} else if (strIndexs[i] == _T('A') || strIndexs[i] == _T('B') || strIndexs[i] == _T('C')
				|| strIndexs[i] == _T('D') || strIndexs[i] == _T('E')) {
			arrIndex.push_back(strIndexs[i] - _T('A') + 9);
		} else {
			return FALSE;
		}
    }
    Global::TrimBlank(strOperator);
    long lCount = 0;
    for(int i = 0; i < arrIndex.size(); i++) {
        int iIndex = arrIndex[i];
        int code = tempArr[iIndex];
		if (strOperator.compare(_T(">=")) == 0 && code >= iMarkCode) lCount++;
		else if (strOperator.compare(_T("<=")) == 0 && code <= iMarkCode) lCount++;
		else if (strOperator.compare(_T(">")) == 0 && code > iMarkCode) lCount++;
		else if (strOperator.compare(_T("<")) == 0 && code < iMarkCode) lCount++;
		else if (strOperator.compare(_T("==")) == 0 && code == iMarkCode) lCount++;
		else if (strOperator.compare(_T("!=")) == 0 && code != iMarkCode) lCount++;
    }
    if(lCount < lMarkCount) return TRUE;
    return FALSE;
}

//F|12A|103|1|3|
BOOL CEngine::IsFilterF(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr) {
	if (strTJ.find(_T("F")) == std::string::npos) {
		return FALSE;
	}
    CIntPair CountRange;
    //depart std::string
    CStlStrArray arrPart;
    TCHAR cbDim[] = _T("|");
    Global::DepartString(strTJ, cbDim, arrPart);
    //jugde valid para
    if(arrPart.size() != 5) return FALSE;
    std::string strHead = arrPart[0];
    std::string strIndexs = arrPart[1];
    std::string strScores = arrPart[2];
    CountRange.first = _ttol(arrPart[3].c_str());
    CountRange.second = _ttol(arrPart[4].c_str());
    Global::TrimBlank(strIndexs);
    Global::TrimBlank(strScores);
    if(CountRange.first > CountRange.second) return false;

    CIntArray arrIndex;
    for(int i = 0; i < strIndexs.size(); i++) {
		if (strIndexs[i] >= _T('1') && strIndexs[i] <= _T('9')) {
			arrIndex.push_back(strIndexs[i] - _T('1'));
		}
        else if(strIndexs[i] == _T('A') || strIndexs[i] == _T('B') 
			|| strIndexs[i] == _T('C') || strIndexs[i] == _T('D') || strIndexs[i] == _T('E'))
            arrIndex.push_back(strIndexs[i] - _T('A') + 9);
        else return FALSE;
    }
    CIntArray arrFirst;
    for(int i = 0; i < strScores.size(); i++) {
        int iScoreValue = strScores[i] - _T('0');
        if(iScoreValue == 0 || iScoreValue == 1 || iScoreValue == 3)
            arrFirst.push_back(iScoreValue);
        else return FALSE;
    }
    if(arrFirst.size() != arrIndex.size()) return FALSE;

    int iCount = 0;
    for(int i = 0; i < arrIndex.size(); i++) {
        int iIndex = arrIndex[i];
        if(arrFirst[i] == tempArr[iIndex]) iCount++;
    }
    if(iCount > CountRange.second || iCount < CountRange.first)
        return TRUE;
    return FALSE;
}

BOOL CEngine::IsFilterW(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr) {
	if (strTJ.find(_T("W")) == std::string::npos) {
		return FALSE;
	}
    CStlStrArray arrPart;
    CIntPair CountRange;
    TCHAR cbDim[] = _T("&");
    Global::DepartString(strTJ, cbDim, arrPart);
    if(arrPart.size() != tempArr.size() + 4) return FALSE;
    std::string strHead = arrPart[1];
    CountRange.first = _ttol(arrPart[2].c_str());
    CountRange.second = _ttol(arrPart[3].c_str());
    CIntxyArray arrOtherChoice;
    for(int i = 4; i < arrPart.size(); i++) {
        CIntArray arrOneChoice;
        std::string strChoices = arrPart[i];
        Global::TrimBlank(strChoices);
		if (strChoices == "#") {
			arrOneChoice.push_back(3);
			arrOneChoice.push_back(1);
			arrOneChoice.push_back(0);
		} else {
			for (int j = 0; j < strChoices.size(); j++) {
				int iChoiceValue = strChoices[j] - _T('0');
				if (iChoiceValue == 0 || iChoiceValue == 1 || iChoiceValue == 3)
					arrOneChoice.push_back(iChoiceValue);
				else return FALSE;
			}
		}
        arrOtherChoice.push_back(arrOneChoice);
    }
    if(arrOtherChoice.size() != tempArr.size()) return FALSE;
    int iSameCount = 0;
    for(int i = 0; i < arrOtherChoice.size(); i++) {
		if (tempArr[i] == 8) {
			iSameCount++;
		}
        if(std::find(arrOtherChoice[i].begin(), arrOtherChoice[i].end(), tempArr[i])
                != arrOtherChoice[i].end())
            iSameCount++;
    }

    if(iSameCount < CountRange.first || iSameCount > CountRange.second)
        return TRUE;
    return FALSE;
}

//�ַ������� Q|31000|
BOOL CEngine::IsFilterQ(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr) {
	if (strTJ.find(_T("Q")) == std::string::npos) {
		return FALSE;
	}
    CStlStrArray arrPart;
    TCHAR cbDim[] = _T("|");
    Global::DepartString(strTJ, cbDim, arrPart);
    //jugde valid para
    if(arrPart.size() != 2) return FALSE;
    std::string strHead = arrPart[0];
    std::string strQueue = arrPart[1];
    if(strHead != std::string("Q")) return FALSE;
    std::stringstream oStr;
    for(int i = 1; i < tempArr.size(); i++) {
        oStr << tempArr[i];
    }
    std::string strAll = oStr.str();
    if(pStr != NULL) {
        std::stringstream oTmp;
        oTmp << strQueue;
        *pStr = oTmp.str();
    }
	if (strAll.find(strQueue) != std::string::npos) {
		return TRUE;
	}
    return FALSE;
}

BOOL CEngine::IsFilterX(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr) {
	if (strTJ.find(_T("X")) != 0) {
		return FALSE;
	}

	if (m_mapFilterX.find(strTJ) == m_mapFilterX.end()) {
		FilterX_Params fp;
		fp.tj = strTJ;
		CStlStrArray arrPart;
		TCHAR cbDim[] = _T("|");
		Global::DepartString(strTJ, cbDim, arrPart);
		//jugde valid para
		if (arrPart.size() != 4) {
			m_mapFilterX[strTJ] = fp;
			OutputDebugStringA((strTJ + " error 0 lua_").c_str());
			return FALSE;
		}
		if (!PathFileExists(arrPart[1].c_str())) {
			m_mapFilterX[strTJ] = fp;
			OutputDebugStringA((strTJ + " error 1 lua_").c_str());
			return FALSE;
		}
		
		int begin = _ttol(arrPart[2].c_str());
		int end = _ttol(arrPart[3].c_str());
		if (begin == -1 || end == -1) {
			m_mapFilterX[strTJ] = fp;
			OutputDebugStringA((strTJ + " error 2 lua_").c_str());
			return FALSE;
		}
		if (end < begin) {
			m_mapFilterX[strTJ] = fp;
			OutputDebugStringA((strTJ + " error 3 lua_").c_str());
			return FALSE;
		}
		fp.filepath = arrPart[1];
		fp.range_begin = begin;
		fp.range_end = end;
		std::string filedata;
		if (!Global::ReadFileData(fp.filepath, filedata)) {
			m_mapFilterX[strTJ] = fp;
			OutputDebugStringA((strTJ + " error 4  lua_").c_str());
			return FALSE;
		}
		if (!GetRecords(filedata, fp.arrRecords)) {
			m_mapFilterX[strTJ] = fp;
			OutputDebugStringA((strTJ + " error 5 lua_").c_str());
			return FALSE;
		}
		fp.isValid = TRUE;
		m_mapFilterX[strTJ] = fp;
		OutputDebugStringA((strTJ + " prepared lua_").c_str());
	}

	const auto& item = m_mapFilterX.find(strTJ);
	if (item == m_mapFilterX.cend()) {
		return FALSE;
	}
	const FilterX_Params& fp = item->second;
	if (!fp.isValid) {
		return FALSE;
	}
	const CIntxyArray& arrRecords = item->second.arrRecords;
	for (const auto& record : arrRecords) {
		if (record.size() != tempArr.size()) {
			return FALSE;
		}
		int same_count = 0;
		for (int i = 0; i < record.size(); i++) {
			if (record[i] == tempArr[i]) {
				same_count++;
			}
		}
		if (same_count >= fp.range_begin && same_count <= fp.range_end) {
			return FALSE;
		}
	}
	return TRUE;
}

void CEngine::doFilterG(const CStlString& strG) {
	if (strG.size() <= 0) return;
	CStlStrArray arrPart, arrCounts;
	if (!Global::DepartString(strG, _T("|"), arrPart)) {
		return;
	}
	if (arrPart.size() != 2) return;
	Global::TrimBlank(arrPart[0]);
	Global::TrimBlank(arrPart[1]);
	if (arrPart[0] != "G") return;
	if (!Global::DepartString(arrPart[1], _T(","), arrCounts)) {
		return;
	}
	if (arrCounts.size() == 0) return;
	std::set<int> setCounts;
	for (auto& num : arrCounts) {
		Global::TrimBlank(num);
		setCounts.insert(_ttoi(num.c_str()));
	}
	TCHAR szLog[256] = { _T('\0') };
	_stprintf(szLog, "lua_doFilterG: tj = [%s]",strG.c_str());
	OutputDebugString(szLog);

	CIntxyArray *pNew = new (std::nothrow) CIntxyArray();
	if (pNew == NULL) return;
	int nCurMaxLose = m_lMaxLose;
	m_lMaxLose = 1;
	CIntxyArray &xyAll = m_arrAllRecord;
	CIntxyArray arrCoverIndex;
	CIntArray arrTemp;
	std::map<size_t, size_t> cover_statis;
	for (CIntxyArray::iterator iter = xyAll.begin(); 
			iter != xyAll.end(); iter++) {
		GetCoverIndexArr(iter, xyAll, arrTemp);
		size_t cover_count = arrTemp.size();
		if (cover_count > 0) {
			arrCoverIndex.push_back(arrTemp);
			cover_count = cover_count - 1;
			if (cover_statis.find(cover_count) == cover_statis.end()) {
				cover_statis[cover_count] = 0;
			}
			cover_statis[cover_count]++;
		} else {
			break;
		}
	}
	if (arrCoverIndex.size() != xyAll.size()) {
		m_lMaxLose = nCurMaxLose;
		return;
	}
	
	OutputDebugString("\r\n");
	for (const auto& cs : cover_statis) {
		_stprintf(szLog, "lua_doFilterG: cover28[%u] = [%u]", 
			cs.first, cs.second);
		OutputDebugString(szLog);
	}
	OutputDebugString("\r\n");

	for (int i = 0; i < arrCoverIndex.size(); i++) {
		int iCount = arrCoverIndex[i].size() - 1;
		if (setCounts.find(iCount) != setCounts.end()) {
			pNew->push_back(xyAll[i]);
		} else {
			_stprintf(szLog, "lua_doFilterG: not found iCount=[%d]", iCount);
			//OutputDebugString(szLog);
		}
	}
	m_arrAllRecord.swap(*pNew);
	m_lMaxLose = nCurMaxLose;
	return;
}