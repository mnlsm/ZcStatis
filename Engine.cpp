#include "stdafx.h"
#include "Engine.h"
#include "Global.h"

std::map<CStlString, CIntArray> CEngine::s_mapAllRen9Pos;

CEngine::CEngine() {
    m_lMaxRate = 8;
    m_lMaxLimit = 10000000;
    m_lMaxLose = 0;

	m_iInitRecordCount = 0;
	m_arrPLScope.push_back(0.0);
	m_arrPLScope.push_back(2.25);
	m_arrPLScope.push_back(4.0);
	m_arrPLScope.push_back(6.0);
	m_arrPLScope.push_back(1000.0);

	CEngine::InitAllRen9Pos();
}

CEngine::~CEngine() {
}

void CEngine::SetChoices(const CStlString& strChoices) {
	CIntxyArray arrChoices;
	CEngine::GetChoices(strChoices, arrChoices);
	SetChoices(arrChoices);
}

void CEngine::SetDZRecords(const CStlString& strRecords) {
	CIntxyArray arrRecords;
	CEngine::GetRecords(strRecords, arrRecords);
	SetDZRecords(arrRecords);
}

void CEngine::SetDZRecords9(const CStlString& strRecords) {
	CIntxyArray arrRecords;
	CEngine::GetRecords(strRecords, arrRecords);
	m_arrAllRecord9 = arrRecords;
}

void CEngine::SetPL(const CStlString &pl) {
	m_strPL = pl;
	GetPLDatas(m_strPL, m_arrPLData, m_arrGVData);
}

void CEngine::SetMaxLose(long lLose) {
	m_lMaxLose = lLose;
}

void CEngine::SetChoices(const CIntxyArray &arrChoices) {
	m_arrChoices.clear();
	m_arrChoices.assign(arrChoices.begin(), arrChoices.end());
	m_arrAllRecord.clear();
}

void CEngine::SetDZRecords(const CIntxyArray &arrRecords) {
	m_arrAllRecord.clear();
	m_arrAllRecord.assign(arrRecords.begin(), arrRecords.end());
	m_arrChoices.clear();
}

BOOL CEngine::CalculateAllResult(CStlString& failed_reason) {
	return CalculateAllResultImpl(NULL, failed_reason);
}

BOOL CEngine::IsAValidRecord(const CIntArray& record, CStlString& failed_reason) {
	failed_reason.clear(); 
	BOOL bRet = FALSE;
	if (record.size() != TOTO_COUNT) {
		failed_reason = _T("source record invalid!");
		return bRet;
	}
	int index = 0;
	int maxSame = 0;
	CIntArray maxSameRecord;
	for (const auto& r : m_arrAllRecord) {
		++index;
		int samecount = 0;
		for (int i = 0; i < TOTO_COUNT; i++) {
			if (r[i] == record[i]) {
				samecount++;
			}
		}
		if (samecount >= TOTO_MAXLOSE) {
			bRet = TRUE;
			CStlString codes = GetRecordString(r);
			TCHAR szInfo[128] = { _T('\0') };
			_stprintf(szInfo, _T("found record: codes=[%s], index=%d, losed=%d\r\n"),
				codes.c_str(), index, TOTO_COUNT - samecount);
			failed_reason += szInfo;
		}
		if (samecount > maxSame) {
			maxSame = samecount;
			maxSameRecord = r;
		}
	}
	if (!bRet) {
		TCHAR szInfo[128] = { _T('\0') };
		CStlString codes = GetRecordString(maxSameRecord);
		_stprintf(szInfo, _T("\r\nmax same record: codes=[%s], losed=%d\r\n"),
			codes.c_str(), TOTO_COUNT - maxSame);
		failed_reason += szInfo;
	}
	return bRet;
}


const CIntxyArray& CEngine::GetResult() {
	return m_arrResultRecord;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CEngine::CalculateAllResultImpl(void* ctx, CStlString& failed_reason) {
	ZeroMemory(m_arrRecordFenBu, TOTO_COUNT * 3 * sizeof(int));
	m_mapFilterX.clear();
	m_arrCoverIndex.clear();
	m_arrResultRecord.clear();
	if (!m_arrChoices.empty()) {
		m_arrAllRecord.clear();
		CIntArray tempArr;  //temp data
		SearchAllRecord(m_arrChoices.begin(), m_arrChoices.end(), 
			tempArr, m_arrAllRecord);
	}
	else { //去重
		std::stable_sort(m_arrAllRecord.begin(), m_arrAllRecord.end());
		m_arrAllRecord.erase(std::unique(m_arrAllRecord.begin(), m_arrAllRecord.end()), m_arrAllRecord.end());
	}
	m_iInitRecordCount = m_arrAllRecord.size();
	CIntxyArray tempAll;

	CIntArray arrRecordTest;
	for (const auto& record : m_arrAllRecord) {
		if (IsAValidRecordImpl(record, ctx, NULL)) {
			for (size_t i = 0; i < record.size(); i++) {
				if (record[i] == 3) {
					m_arrRecordFenBu[3 * i]++;
				} else if (record[i] == 1) {
					m_arrRecordFenBu[3 * i + 1]++;
				} else {
					m_arrRecordFenBu[3 * i + 2]++;
				}
			}
			tempAll.push_back(record);
		}
	}
	m_arrAllRecord.swap(tempAll);

	FilterG(); //28校验
//	std::random_shuffle(m_arrAllRecord.begin(), m_arrAllRecord.end());
//	std::reverse(m_arrAllRecord.begin(), m_arrAllRecord.end());
	if (m_lMaxLose > 0) {
		FillAllCoverIndex(m_arrAllRecord);
		GreedyCalcRectRecord(m_arrCoverIndex, m_arrAllRecord);
	}
	else {
		m_arrResultRecord = m_arrAllRecord;
	}

	StatisRen9();
	return TRUE;
}

BOOL CEngine::IsAValidRecordImpl(const CIntArray& record, void* ctx, CStlString* invalid_reason) {
	return TRUE;
}


void CEngine::SearchAllRecord(CIntxyArray::iterator choice_iter, CIntxyArray::iterator iter_end, 
		CIntArray &tempArr, CIntxyArray& arrAllRecord) {
	//finish DiGui
	if (choice_iter == iter_end) {
		arrAllRecord.push_back(tempArr);
		//m_arrAllRecord.push_back(tempArr);
		return;
	}
	for (CIntArray::iterator codeIter = choice_iter->begin(); codeIter != choice_iter->end(); ++codeIter) {
		tempArr.push_back(*codeIter);
		SearchAllRecord(choice_iter + 1, iter_end, tempArr, arrAllRecord);
		tempArr.pop_back();
	}
}

bool CEngine::FillAllCoverIndex(CIntxyArray &xyAll) {
    m_arrCoverIndex.clear();
    CIntArray arrTemp;
    int i = 0;

    for(CIntxyArray::iterator iter = xyAll.begin(); iter != xyAll.end(); iter++, i++) {
        GetCoverIndexArr(iter, xyAll, arrTemp);
        if(arrTemp.size() > 0) m_arrCoverIndex.push_back(arrTemp);
    }
    return m_arrCoverIndex.size() == xyAll.size() ? true : false;
}

void CEngine::GetCoverIndexArr(CIntxyArray::iterator iter, CIntxyArray &arrSource, CIntArray &arrResult) {
    arrResult.clear();
    int iIndex = 0;
    int iBlank = 0;
    for(CIntxyArray::iterator LocalIter = arrSource.begin(); LocalIter != arrSource.end(); LocalIter++, iIndex++) {
        CIntArray::iterator InValueIter = LocalIter->begin();
        for(CIntArray::iterator outValueIter = iter->begin();
                outValueIter != iter->end(); outValueIter++, InValueIter++) {
            if(*outValueIter != *InValueIter) iBlank++;
            if(iBlank >= m_lMaxLose + 1) break;  //enhance efficience
        }
        if(iBlank < m_lMaxLose + 1) arrResult.push_back(iIndex);  //can cover!!!!!!
        iBlank = 0;
    }
}

void CEngine::GreedyCalcRectRecord(CIntxyArray &F, CIntxyArray &G) {
    bool bFillAll = false;
    CIntArray arrTemp;
	m_arrResultRecord.clear();
    int iLoopNum = 0;
    int *U = new int[G.size()];
    memset(U, 0, sizeof(int)*G.size());
    while(!bFillAll) {
        //Get Max S n U
        int iMax = 0, iIndex = 0, iMaxIndex = -1;
        CIntxyArray::iterator iter = F.begin();
        for(; iter != F.end(); iter++, iIndex++) {
            int count = 0;
            //if(U[iIndex]==1) continue;
            for(CIntArray::iterator ValueIter = iter->begin(); ValueIter != iter->end(); ValueIter++)
                if(U[*ValueIter] == 0) count++;
            if(count > iMax) {
                iMaxIndex = iIndex;
                iMax = count;
            }
        }
        if(iMaxIndex >= 0) {
            //U=U-S
            iter = F.begin();
            iter = iter + iMaxIndex;
            for(CIntArray::iterator ValueIter = iter->begin(); ValueIter != iter->end(); ValueIter++)
                U[*ValueIter] = 1;
            //C=C U {S}
            arrTemp.push_back(iMaxIndex);
            iLoopNum++;
        }
        int i = 0;
        for(; i < G.size(); i++) if(U[i] == 0) break;
        if(i >= G.size()) bFillAll = true;

        if(iLoopNum == log(G.size())*G.size() / m_lMaxRate + 1 && G.size() / m_lMaxRate > 0) {
            arrTemp.clear();
            break;
        }

    }
    delete[]U;

    if(arrTemp.size() > 0) {
        CIntxyArray::iterator resultIter = G.begin();
        for(int i = 0; i < arrTemp.size(); i++) {
			m_arrResultRecord.push_back(*(resultIter + arrTemp[i]));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL CEngine::GetPLDatas(const CStlString& strPL, CDoublexyArray& arrPLData, CDoublexyArray& arrGVData) {
	arrPLData.clear();
	arrGVData.clear();
	CStlStrArray arrPLParts;
	if (!Global::DepartString(strPL, "#", arrPLParts)) {
		return FALSE;
	}
	if (arrPLParts.size() != 42) {
		return FALSE;
	}
	for (int i = 0; i < arrPLParts.size(); i = i + 3) {
		CDoubleArray arrTemp, arrTemp1;
		double fTemp = 0.9;
		double dGap = 0.00;
		const double rThold = 0.005;
		double r3 = atof(arrPLParts[i].c_str());
		double r1 = atof(arrPLParts[i + 1].c_str());
		double r0 = atof(arrPLParts[i + 2].c_str());
		if (r3 == r1) {
			r1 += rThold;
		}
		if (r3 == r0) {
			r0 += rThold;
		}
		if (r1 == r0) {
			r0 += rThold;
		}
		arrTemp1.push_back(r3);
		arrTemp1.push_back(r1);
		arrTemp1.push_back(r0);
		arrPLData.push_back(arrTemp1);

		double d3 = fTemp / r3;
		double d1 = fTemp / r1;
		double d0 = fTemp / r0;
		dGap = 1 - (d3 + d1 + d0);
		d3 += (dGap / 3);
		d1 += (dGap / 3);
		d0 += (dGap / 3);
		arrTemp.push_back(d3);
		arrTemp.push_back(d1);
		arrTemp.push_back(d0);
		arrGVData.push_back(arrTemp);
	}
	if (arrGVData.size() != TOTO_COUNT || arrPLData.size() != TOTO_COUNT) {
		arrPLData.clear();
		arrGVData.clear();
		return FALSE;
	}
	return TRUE;
}

BOOL CEngine::GetLianXu(const CIntArray& record, int &nMaxSP, int &nMaxSF, int &nMaxPF) {
	if (record.size() != TOTO_COUNT) {
		return FALSE;
	}
	CIntPair pairCurSP, pairCurSF, pairCurPF;
	pairCurSP.first = 0;
	pairCurSP.second = 0;
	pairCurSF.first = 0;
	pairCurSF.second = 0;
	pairCurPF.first = 0;
	pairCurPF.second = 0;
	for (int i = 0; i < record.size(); i++) {
		if (record[i] == 3) {
			++pairCurSP.first;
			++pairCurSF.first;
			if (pairCurPF.first != 0 && pairCurPF.second != 0) {
				if (nMaxPF < pairCurPF.first + pairCurPF.second)
					nMaxPF = pairCurPF.first + pairCurPF.second;
			}
			pairCurPF.first = 0;
			pairCurPF.second = 0;
		} else if (record[i] == 1) {
			++pairCurSP.second;
			++pairCurPF.first;
			if (pairCurSF.first != 0 && pairCurSF.second != 0) {
				if (nMaxSF < pairCurSF.first + pairCurSF.second)
					nMaxSF = pairCurSF.first + pairCurSF.second;
			}
			pairCurSF.first = 0;
			pairCurSF.second = 0;
		} else if (record[i] == 0) {
			++pairCurSF.second;
			++pairCurPF.second;

			if (pairCurSP.first != 0 && pairCurSP.second != 0) {
				if (nMaxSP < pairCurSP.first + pairCurSP.second)
					nMaxSP = pairCurSP.first + pairCurSP.second;
			}
			pairCurSP.first = 0;
			pairCurSP.second = 0;
		} else {
			nMaxSP = nMaxSF = nMaxPF = 0;
			return TRUE;
		}
	}
	if (pairCurSP.first != 0 && pairCurSP.second != 0) {
		if (nMaxSP < pairCurSP.first + pairCurSP.second)
			nMaxSP = pairCurSP.first + pairCurSP.second;
	}
	if (pairCurSF.first != 0 && pairCurSF.second != 0) {
		if (nMaxSF < pairCurSF.first + pairCurSF.second)
			nMaxSF = pairCurSF.first + pairCurSF.second;
	}
	if (pairCurPF.first != 0 && pairCurPF.second != 0) {
		if (nMaxPF < pairCurPF.first + pairCurPF.second)
			nMaxPF = pairCurPF.first + pairCurPF.second;
	}
	return TRUE;
}

BOOL CEngine::CalcCommonFilterFactors(const CIntArray& record, const CDoublexyArray& arrPLData,
	const CDoublexyArray& arrGVData, const CDoubleArray& arrPLScope, CommonFilterFactors& commonFF) {
	commonFF.Clear();
	if (record.size() != TOTO_COUNT) {
		return FALSE;
	}
	
	//总3, 总1, 总0
	commonFF.mTotal3Count = commonFF.mTotal1Count = commonFF.mTotal0Count = 0;
	for (const auto code : record) {
		if (code == 3) {
			commonFF.mTotal3Count++;
		} else if (code == 1) {
			commonFF.mTotal1Count++;
		} else if (code == 0) {
			commonFF.mTotal0Count++;
		}
	}

	//连3，连1, 连0, 断点数
	commonFF.mLian3Count = commonFF.mLian1Count = commonFF.mLian0Count = 0;
	commonFF.mBreakCount = 0;
	int l3 = 0, l1 = 0, l0 = 0;
	for (int index = 1; index < record.size(); index++) {
		int code = record[index];
		int pre_code = record[index - 1];
		if (code == pre_code) {
			if (code == 3) {
				if (++l3 == 1) {
					l3++;
				}
				if (l3 > commonFF.mLian3Count) {
					commonFF.mLian3Count = l3;
				}
			} else if (code == 1) {
				if (++l1 == 1) {
					l1++;
				}
				if (l1 > commonFF.mLian1Count) {
					commonFF.mLian1Count = l1;
				}
			} else if (code == 0) {
				if (++l0 == 1) {
					l0++;
				}
				if (l0 > commonFF.mLian0Count) {
					commonFF.mLian0Count = l0;
				}
			} else {
				commonFF.mLian3Count = commonFF.mLian1Count = commonFF.mLian0Count = 0;
				commonFF.mBreakCount = 0;
				break;
			}
		} else {
			l3 = l1 = l0 = 0;
			commonFF.mBreakCount++;
		}
	}
	
	//连31，连30, 连10
	GetLianXu(record, commonFF.mLian31Count, commonFF.mLian30Count, commonFF.mLian10Count);
	
	//赔率数据
	if (arrPLData.size() == TOTO_COUNT && arrGVData.size() == TOTO_COUNT) {
		commonFF.mPL1Count = commonFF.mPL2Count = commonFF.mPL3Count = 0;
		commonFF.mGvj = 1.0;
		commonFF.mPLSum = 0.0;
		commonFF.mPLScopes.clear();
		int arrSCOPE[1024];
		for (int i = 0; i < 1024; i++) {
			arrSCOPE[i] = 0;
		}
		for (int i = 0; i < record.size(); i++) {
			int nIndex = -1;
			if (record[i] == 3) {
				nIndex = 0;
			} else if (record[i] == 1) {
				nIndex = 1;
			} else if (record[i] == 0) {
				nIndex = 2;
			}
			if (nIndex >= 0) {
				CDoubleArray arrTemp = arrPLData[i];
				CDoubleArray arrTemp1 = arrGVData[i];
				double dCurPL = arrTemp[nIndex];
				double dCurGV = arrTemp1[nIndex];
				commonFF.mPLSum += dCurPL;
				std::stable_sort(arrTemp.begin(), arrTemp.end(), std::less<double>());
				int avgGap = 0;
				if ((arrTemp[2] - arrTemp[0]) < 2.0) {
					avgGap = 3;
				}
				if (arrTemp[0] == dCurPL) {
					commonFF.mPL1Count++;
					commonFF.mPLAvgs[0 + avgGap]++;
					commonFF.mPLAvgsPos[i] = (avgGap == 0) ? 1 : 2;
				}
				else if (arrTemp[1] == dCurPL) {
					commonFF.mPL2Count++;
					commonFF.mPLAvgs[1 + avgGap]++;
					commonFF.mPLAvgsPos[i] = (avgGap == 0) ? 5 : 3;
				}
				else {
					commonFF.mPL3Count++;
					commonFF.mPLAvgs[2 + avgGap]++;
					commonFF.mPLAvgsPos[i] = (avgGap == 0) ? 6 : 4;
				}
				commonFF.mGvj = commonFF.mGvj * dCurGV;
				for (int j = 1; j < arrPLScope.size(); j++) {
					if (dCurPL > arrPLScope[j - 1] && dCurPL <= arrPLScope[j])
						arrSCOPE[j - 1]++;
				}
			}
		}
		int avg5 = commonFF.mPLAvgs[1];
		int avg6 = commonFF.mPLAvgs[2];
		commonFF.mPLAvgs.erase(commonFF.mPLAvgs.begin() + 1);
		commonFF.mPLAvgs.erase(commonFF.mPLAvgs.begin() + 1);
		commonFF.mPLAvgs.push_back(avg5);
		commonFF.mPLAvgs.push_back(avg6);
		if (arrPLScope.size() > 1) {
			for (int i = 0; i < arrPLScope.size() - 1; i++) {
				commonFF.mPLScopes.push_back(arrSCOPE[i]);
			}
		}
	}
	return TRUE;
}

BOOL CEngine::GetChoices(const CStlString& strChoices, CIntxyArray& arrChoices) {
	arrChoices.clear();
	CStlStrArray choices;
	Global::DepartString(strChoices, _T(","), choices);
	if (choices.size() != TOTO_COUNT) {
		return FALSE;
	}
	for (const auto& codes : choices) {
		if (codes.empty()) {
			return FALSE;
		}
		CIntArray arrChoice;
		for (const auto& code : codes) {
			int iVal = code - _T('0');
			if (iVal != 3 && iVal != 1 && iVal != 0) {
				return FALSE;
			}
			arrChoice.push_back(iVal);
		}
		arrChoices.push_back(arrChoice);
	}
	return TRUE;
}

UINT CEngine::GetRecordsCount(const CStlString& strCodes) {
	CStlStrArray lines;
	Global::DepartString(strCodes, _T("\n"), lines);
	return lines.size();
}

BOOL CEngine::GetRecord(const CStlString& strCode, CIntArray& arrRecord) {
	arrRecord.clear();
	if (strCode.length() != TOTO_COUNT) {
		return FALSE;
	}
	for (const auto& code : strCode) {
		int iVal = code - _T('0');
		if (iVal != 3 && iVal != 1 && iVal != 0) {
			return FALSE;
		}
		arrRecord.push_back(iVal);
	}
	return TRUE;
}

BOOL CEngine::GetRecords(const CStlString& strCodes, CIntxyArray& arrRecords) {
	arrRecords.clear();
	CStlStrArray lines;
	Global::DepartString(strCodes, _T("\n"), lines);
	for (auto& line : lines) {
		CStringATL temp = line.c_str();
		temp.Trim();
		temp.Replace(_T(";"), _T(","));
		temp.Replace(_T("&"), _T(","));
		temp.Replace(_T("\r"), _T(""));
		temp.Replace(_T("\""), _T(""));
		temp.Replace(_T("'"), _T(""));
		line = temp;
		CIntArray arrRecord;
		if (line.find(_T(",")) != CStlString::npos) {
			CIntxyArray arrChoices;
			if (GetChoices(line, arrChoices)) {
				SearchAllRecord(arrChoices.begin(), arrChoices.end(), arrRecord, arrRecords);
			}
		} else if (line.size() == TOTO_COUNT) {
			for (const auto& code : line) {
				int iVal = code - _T('0');
				if (iVal != 3 && iVal != 1 && iVal != 0 && iVal != 8) {
					return FALSE;
				}
				arrRecord.push_back(iVal);
			}
			if (arrRecord.size() != TOTO_COUNT) {
				return FALSE;
			}
			arrRecords.push_back(arrRecord);
		}
	}
	std::stable_sort(arrRecords.begin(), arrRecords.end());
	arrRecords.erase(std::unique(arrRecords.begin(), arrRecords.end()), arrRecords.end());
	return TRUE;
}

CStlString CEngine::GetRecordString(const CIntArray& record) {
	if (record.size() != TOTO_COUNT) {
		return _T("");
	}
	CStlString strRecord;
	strRecord.resize(TOTO_COUNT, _T('\0'));
	for (int i = 0; i < TOTO_COUNT; i++) {
		const auto& code = record[i];
		strRecord[i] = _T('0') + code;
	}
	return strRecord;
}

void CEngine::GetRecordsString(const CIntxyArray& arrRecords, CStlString& strRecords) {
	strRecords.clear();
	for (const auto& record : arrRecords) {
		CStlString line(TOTO_COUNT, _T('\0'));
		for (int i = 0; i < TOTO_COUNT; i++) {
			const auto& code = record[i];
			line[i] = _T('0') + code;
		}
		if (strRecords.empty()) {
			strRecords = line;
		} else {
			strRecords = strRecords + _T('\n') + line;
		}
	}
}

void CEngine::GetRecordsPrintRecords(const CIntxyArray& arrRecords, CStlStrArray& records) {
	records.clear();
	TCHAR szNum[12] = { _T('\0') };
	for (const auto& record : arrRecords) {
		CStlString line;
		for (int i = 0; i < TOTO_COUNT; i++) {
			_stprintf(szNum, _T("%d"), record[i]);
			line = line + szNum;
			if (fmod(i + 1, 5) == 0) {
				line = line + _T("─");
			}
		}
		records.push_back(line);
	}
}

void CEngine::gatherAllRen9Pos(const CIntArray& source, CIntArray& tempArr, int start, int depth) {
	if (depth >= 9) {
		CStlString mark(9, _T('\0'));
		for (int k = 0; k < tempArr.size(); k++) {
			int pos = tempArr[k];
			if (pos < 9) {
				mark[k] = _T('0') + (pos + 1);
			} else {
				mark[k] = _T('A') + (pos - 9);
			}
		}
		s_mapAllRen9Pos[mark] = tempArr;
		return;
	}
	for (int i = start; i < TOTO_COUNT; i++) {
		tempArr.push_back(source[i]);
		gatherAllRen9Pos(source, tempArr, i + 1, depth + 1);
		tempArr.pop_back();
	}
}

void CEngine::InitAllRen9Pos() {
	if (s_mapAllRen9Pos.empty()) {
		CIntArray source, tempArr;
		for (int i = 0; i < TOTO_COUNT; i++) {
			source.push_back(i);
		}
		gatherAllRen9Pos(source, tempArr, 0, 0);
	}
}


