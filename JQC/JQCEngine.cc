#include "stdafx.h"
#include "JQCEngine.h"
#include "Global.h"


static const std::string dbgview_prefix = "jqc_dbgtrace: ";
static const std::string dbgview_exception = "jqc_exception: ";

static int lua_table_getfield(lua_State *L, const char *key, int defval) {
	int result = defval;
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if (lua_type(L, -1) == LUA_TNUMBER) {
		result = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return result;
}

static std::string lua_table_getfield(lua_State *L, const char *key,
	const char *defval) {
	std::string result;
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if (lua_type(L, -1) == LUA_TSTRING) {
		result = lua_tostring(L, -1);
	}
	else {
		if (defval != NULL) {
			result = defval;
		}
	}
	lua_pop(L, 1);
	return result;
}

static std::string lua_table_getfield(lua_State *L, int index, const char *defval) {
	std::string result;
	lua_pushnumber(L, index);
	lua_gettable(L, -2);
	if (lua_type(L, -1) == LUA_TSTRING) {
		result = lua_tostring(L, -1);
	} else {
		if (defval != NULL) {
			result = defval;
		}
	}
	lua_pop(L, 1);
	return result;
}



int __cdecl JQCEngine::LUA_DbgTrace(lua_State *L) {
	if (lua_type(L, 1) == LUA_TSTRING) {
		std::string line = dbgview_prefix + lua_tostring(L, 1);
		OutputDebugStringA(line.c_str());
	}
	return 0;
}

int __cdecl JQCEngine::LUA_IsFilterTJ(lua_State *L) {
	BOOL ret = FALSE;
	if (lua_type(L, 1) == LUA_TSTRING && lua_type(L, 2) == LUA_TSTRING) {
		const char* codes = lua_tostring(L, 1);
		const char* tj = lua_tostring(L, 2);
		int codes_len = strlen(codes);
		CIntArray tempArr;
		for (int i = 0; i < codes_len; i++) {
			tempArr.push_back(codes[i] - '0');
		}
		JQCEngine* pThis = (JQCEngine*)lua_touserdata(L, lua_upvalueindex(1));
		ret = pThis->IsFilterTJ(tempArr, tj);
	}
	lua_pushinteger(L, ret ? 1 : 0);
	return 1;
}


JQCEngine::JQCEngine(const CStlString& script) {
	m_strScript = script;
}

BOOL JQCEngine::CalculateAllResult(CStlString& failed_reason) {
	m_arrResult.clear();
	m_strMatchScores.clear();
	BOOL result = CalculateAllResultImpl(failed_reason);
	if (!result) {
		if (!failed_reason.empty()) {
			failed_reason = "unknow";
		}
		CStlString trace = dbgview_exception + failed_reason;
		OutputDebugStringA(trace.c_str());
	}
	return result;
}

BOOL JQCEngine::CalculateAllResultImpl(CStlString& failed_reason) {
	BOOL result = FALSE;
	lua_State* lua_state = InitLua(failed_reason);
	if (lua_state == NULL) {
		return FALSE;
	}

	CIntxyArray allResult, tempAll;
	if (GeneratorCodes(m_strMatchScores, allResult)) {
		std::stable_sort(allResult.begin(), allResult.end());
		allResult.erase(std::unique(allResult.begin(), allResult.end()), allResult.end());
		for (const auto& record : allResult) {
			if (IsAValidRecordImpl(record, lua_state, &failed_reason)) {
				tempAll.push_back(record);
			}
		}
		m_arrResult.swap(tempAll);
		result = TRUE;
	}
	else {
		failed_reason = "GeneratorCodes failed!";
	}
	TermLua(lua_state);
	return result;
}

BOOL JQCEngine::IsAValidRecordImpl(const CIntArray& record, lua_State* L,
	CStlString* invalid_reason) {
	BOOL result = TRUE;
	lua_getglobal(L, "IsFilterLua");					// 获取函数，压入栈中  
	push_scriptfunc_params(L, record);
	int call_ret = lua_pcall(L, 1, 1, 0);			    // 调用函数，调用完成以后，会将返回值压入栈中，1表示参数个数，2表示返回结果个数。
	if (call_ret != 0) {
		std::string errtext = dbgview_exception + lua_tostring(L, -1);
		OutputDebugStringA(errtext.c_str());
		if (invalid_reason != NULL) {
			*invalid_reason = CA2T(errtext.c_str()).m_psz;
		}
		lua_pop(L, 1);
		return FALSE;
	}
	int ret_type = lua_type(L, -1);
	if (ret_type != LUA_TTABLE) {
		if (invalid_reason != NULL) {
			*invalid_reason = _T("lua function invalid result!");
		}
		lua_pop(L, 1);
		return FALSE;
	}
	int lua_ret = lua_table_getfield(L, "code", 0);
	BOOL isValid = (lua_ret != 1);
	if (!isValid) {
		std::string err = lua_table_getfield(L, "info", "unknow");
		if (invalid_reason != NULL) {
			*invalid_reason = CA2T(err.c_str()).m_psz;
		}
	}
	lua_pop(L, 1);
	return isValid;
}

lua_State* JQCEngine::InitLua(CStlString& failed_reason) {
	m_strMatchScores.clear();
	lua_State* L = luaL_newstate();
	if (L == NULL) {
		failed_reason = _T("luaL_newstate failed!");
		return NULL;
	}
	luaL_openlibs(L);
	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, JQCEngine::LUA_IsFilterTJ, 1);
	lua_setglobal(L, "IsFilterTJ");

	lua_pushcclosure(L, JQCEngine::LUA_DbgTrace, 0);
	lua_setglobal(L, "dbgview_print");

	if (luaL_dostring(L, m_strScript.c_str()) != 0) {
		failed_reason = _T("luaL_dostring failed: ");
		if (lua_type(L, -1) == LUA_TSTRING) {
			std::string err_text = dbgview_exception + lua_tostring(L, -1);
			failed_reason += CA2T(err_text.c_str()).m_psz;
			lua_pop(L, 1);
		}
		lua_close(L);
		return NULL;
	}

	if (lua_getglobal(L, "kMatchScores") == LUA_TTABLE) {
		int index = 0;
		while (++index > 0) {
			std::string codes = lua_table_getfield(L, index, "none");
			if (codes == "none") {
				break;
			}
			m_strMatchScores.push_back(codes);
		}
		lua_pop(L, 1);
	}
	if (m_strMatchScores.empty()) {
		failed_reason = _T("no match scores!!");
		lua_close(L);
		return NULL;
	}
	return L;
}

void JQCEngine::TermLua(lua_State* state) {
	if (state != NULL) {
		lua_close(state);
	}
}

static void gatherMatchScores(const CStlStrxyArray&split_scores,
		int index, CIntArray& record, CIntxyArray& result) {
	if (record.size() == split_scores.size()) {
		result.push_back(record);
	}
	if (index >= split_scores.size()) {
		return;
	}
	for (const auto& codes : split_scores[index]) {
		for (const auto& code : codes) {
			record.push_back(code - '0');
			gatherMatchScores(split_scores, index + 1, record, result);
			record.pop_back();
		}
	}

}

BOOL JQCEngine::GeneratorCodes(const CStlStrArray& arrAllMatchScores, CIntxyArray& result) {
	result.clear();
	CStlStrxyArray split_scores;
	for (const auto& match : arrAllMatchScores) {
		CStlStrArray scores;
		Global::DepartString(match, _T(","), scores);
		if (scores.empty()) {
			return FALSE;
		}
		for (auto& score : scores) {
			Global::TrimBlank(score);
			if (score.size() != 1) {
				return FALSE;
			}
			if (score[0] < '0' || score[0] > '9') {
				return FALSE;
			}
		}
		split_scores.push_back(scores);
	}
	CIntArray record;
	gatherMatchScores(split_scores, 0, record, result);
	return TRUE;
}

BOOL JQCEngine::IsFilterTJ(const CIntArray& record, const CStlString& strTJ) {
	if (IsFilterF(record, strTJ, NULL)) {
		return TRUE;
	}
	return FALSE;
}

void JQCEngine::push_scriptfunc_params(lua_State *L, const CIntArray& record) {
	lua_newtable(L);
	std::string codes(record.size(), '\0');
	std::string match_codes;
	int count3 = 0, count2 = 0, count1 = 0, count0 = 0;
	for (size_t i = 0; i < codes.size(); i++) {
		int code = record.at(i);
		codes[i] = '0' + code;
		if (code == 3) {
			count3++;
		} else if (code == 2) {
			count2++;
		} else if (code == 1) {
			count1++;
		} else if (code == 0) {
			count0++;
		}
		if ((i % 2) != 0) {
			int pre_code = record.at(i - 1);
			int gap = pre_code - code;
			if (gap > 0) {
				match_codes.append(1, '3');
			} else if (gap < 0) {
				match_codes.append(1, '0');
			} else {
				match_codes.append(1, '1');
			}
		}
	}
	lua_pushstring(L, "codes");
	lua_pushstring(L, codes.c_str());
	lua_settable(L, -3);

	lua_pushstring(L, "match_codes");
	lua_pushstring(L, match_codes.c_str());
	lua_settable(L, -3);

	lua_pushstring(L, "count3");
	lua_pushinteger(L, count3);
	lua_settable(L, -3);

	lua_pushstring(L, "count2");
	lua_pushinteger(L, count2);
	lua_settable(L, -3);

	lua_pushstring(L, "count1");
	lua_pushinteger(L, count1);
	lua_settable(L, -3);

	lua_pushstring(L, "count0");
	lua_pushinteger(L, count0);
	lua_settable(L, -3);

}

void JQCEngine::GetResultString(CStlString& result) {
	result.clear();
	for (const auto& record : m_arrResult) {
		CStlString line(record.size(), _T('\0'));
		for (int i = 0; i < record.size(); i++) {
			const auto& code = record[i];
			line[i] = _T('0') + code;
		}
		if (result.empty()) {
			result = line;
		} else {
			result = result + _T('\n') + line;
		}
	}
}

//F|12A|103|1|3|
BOOL JQCEngine::IsFilterF(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr) {
	if (strTJ.find(_T("F")) == std::string::npos) {
		return FALSE;
	}
	CIntPair CountRange;
	//depart std::string
	CStlStrArray arrPart;
	TCHAR cbDim[] = _T("|");
	Global::DepartString(strTJ, cbDim, arrPart);
	//jugde valid para
	if (arrPart.size() != 5) return FALSE;
	std::string strHead = arrPart[0];
	std::string strIndexs = arrPart[1];
	std::string strScores = arrPart[2];
	CountRange.first = _ttol(arrPart[3].c_str());
	CountRange.second = _ttol(arrPart[4].c_str());
	Global::TrimBlank(strIndexs);
	Global::TrimBlank(strScores);
	if (CountRange.first > CountRange.second) return false;

	CIntArray arrIndex;
	for (int i = 0; i < strIndexs.size(); i++) {
		if (strIndexs[i] >= _T('1') && strIndexs[i] <= _T('9')) {
			arrIndex.push_back(strIndexs[i] - _T('1'));
		}
		else if (strIndexs[i] == _T('A') || strIndexs[i] == _T('B')
			|| strIndexs[i] == _T('C') || strIndexs[i] == _T('D') || strIndexs[i] == _T('E'))
			arrIndex.push_back(strIndexs[i] - _T('A') + 9);
		else return FALSE;
	}
	CIntArray arrFirst;
	for (int i = 0; i < strScores.size(); i++) {
		int iScoreValue = strScores[i] - _T('0');
		arrFirst.push_back(iScoreValue);
	}
	if (arrFirst.size() != arrIndex.size()) return FALSE;

	int iCount = 0;
	for (int i = 0; i < arrIndex.size(); i++) {
		int iIndex = arrIndex[i];
		if (arrFirst[i] == tempArr[iIndex]) iCount++;
	}
	if (iCount > CountRange.second || iCount < CountRange.first)
		return TRUE;
	return FALSE;
}
