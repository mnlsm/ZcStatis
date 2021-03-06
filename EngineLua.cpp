#include "stdafx.h"
#include "EngineLua.h"
#include "Global.h"

static const std::string dbgview_prefix = "lua_dbgtrace: ";
static const std::string dbgview_exception = "lua_exception: ";

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

int __cdecl CEngineLua::LUA_DbgTrace(lua_State *L) {
	if (lua_type(L, 1) == LUA_TSTRING) {
		std::string line = dbgview_prefix + lua_tostring(L, 1);
		OutputDebugStringA(line.c_str());
	}
	return 0;
}

int __cdecl CEngineLua::LUA_IsFilterTJ(lua_State *L) {
	BOOL ret = FALSE;
	if (lua_type(L, 1) == LUA_TSTRING && lua_type(L, 2) == LUA_TSTRING) {
		const char* codes = lua_tostring(L, 1);
		const char* tj = lua_tostring(L, 2);
		int codes_len = strlen(codes);
		CIntArray tempArr;
		for (int i = 0; i < codes_len; i++) {
			tempArr.push_back(codes[i] - '0');
		}
		CEngineLua* pThis = (CEngineLua*)lua_touserdata(L, lua_upvalueindex(1));
		ret = pThis->IsFilterTJ(tempArr, tj);
	}
	lua_pushinteger(L, ret ? 1 : 0);
	return 1;
}

CEngineLua::CEngineLua(const CStlString& script) 
	: m_strScript(script) {
	m_nCalcRen9 = 0;
}

CEngineLua::~CEngineLua() {

}

BOOL CEngineLua::CalculateAllResult(CStlString& failed_reason) {
	BOOL result = FALSE;
	lua_State* lua_state = InitLua(failed_reason);
	if (lua_state == NULL) {
		return FALSE;
	}
	result = CEngine::CalculateAllResultImpl(lua_state, failed_reason);
	if (result) {
		if (m_nCalcRen9 != 0) {
			OutputDebugString(_T("lua_CalcRen9 Begin"));
			CalculateAllResult9(lua_state, failed_reason);
			OutputDebugString(_T("lua_CalcRen9 End"));
		}
	}
	TermLua(lua_state);
	return result;
}

BOOL CEngineLua::IsAValidRecord(const CIntArray& record, CStlString& failed_reason) {
	failed_reason.clear();
	BOOL bRet = FALSE;
	if (record.size() != TOTO_COUNT) {
		failed_reason = _T("source record invalid!");
		return bRet;
	}
	int index = 0;
	int maxSame = 0;
	std::map<int, int> g9Stat;
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
		if (samecount >= 9) {
			if (g9Stat.find(samecount) != g9Stat.end()) {
				g9Stat[samecount]++;
			} else {
				g9Stat[samecount] = 1;
			}
		}
		if (samecount > maxSame) {
			maxSame = samecount;
			maxSameRecord = r;
		}
	}
	
	if (maxSame != TOTO_COUNT) {
		CStlString failed_reason1;
		lua_State* lua_state = InitLua(failed_reason1);
		if (lua_state != NULL) {
			if (IsAValidRecordImpl(record, lua_state, &failed_reason1)) {
				failed_reason1 = _T("filter by compress");
			}
		}
		TermLua(lua_state);

		TCHAR szInfo[128] = { _T('\0') };
		CStlString codes = GetRecordString(maxSameRecord);
		_stprintf(szInfo, _T("\r\nmax same record: codes=[%s], same=%d\r\n"),
			codes.c_str(),  maxSame);
		failed_reason1 += szInfo;
		failed_reason += failed_reason1;
	}

	failed_reason = failed_reason + _T("\r\n\r\nren9 stat begin");
	for (auto& s9 : g9Stat) {
		TCHAR szInfo[128] = { _T('\0') };
		_stprintf(szInfo, _T("\r\nsame[%d] = [%d]"), s9.first, s9.second);
		failed_reason = failed_reason + szInfo;
	}
	failed_reason = failed_reason + _T("\r\nren9 stat end\r\n");

	if (m_nCalcRen9 != 0) {
		failed_reason = failed_reason + _T("\r\n\r\nren9 search begin");
		maxSame = 0;
		index = 0;
		maxSameRecord.clear();
		for (const auto& r9 : m_arrAllRecord9) {
			int samecount = 0;
			for (int i = 0; i < TOTO_COUNT; i++) {
				if (r9[i] == record[i]) {
					samecount++;
				}
			}
			if (samecount >= 9) {
				CStlString codes = GetRecordString(r9);
				TCHAR szInfo[128] = { _T('\0') };
				_stprintf(szInfo, _T("\r\nfound record: codes=[%s], index=[%d]"),
					codes.c_str(), index);
				failed_reason += szInfo;
			}
			if (samecount > maxSame) {
				maxSame = samecount;
				maxSameRecord = r9;
			}
			++index;
		}
		TCHAR szInfo[128] = { _T('\0') };
		CStlString codes = GetRecordString(maxSameRecord);
		_stprintf(szInfo, _T("\r\nmax same record: codes=[%s], same=%d"),
			codes.c_str(), maxSame);
		failed_reason += szInfo;
		failed_reason = failed_reason + _T("\r\nren9 search end\r\n");
	}
	return bRet;
}

BOOL CEngineLua::IsAValidRecordImpl(const CIntArray& record, void* ctx, CStlString* invalid_reason) {
	BOOL result = TRUE;
	lua_State *L = (lua_State *)ctx;
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

void CEngineLua::FilterG() {
	CStringATL strLog;
	strLog.Format(_T("lua_FilterG Begin, allrecord count=[%d]"), m_arrAllRecord.size());
	OutputDebugString(strLog);
	if (!m_strCheck28.empty()) {
		strLog = _T("lua_doFilterG Call!");
		OutputDebugString(strLog);
		doFilterG(m_strCheck28);
	}
	strLog.Format(_T("lua_FilterG End, allrecord count=[%d]"), m_arrAllRecord.size());
	OutputDebugString(strLog);
}

void CEngineLua::StatisRen9() {
	if (m_nCalcRen9 == 0) {
		return;
	}
	CStringATL strLog;
	strLog.Format(_T("lua_StatisRen9 Begin, allrecord count=[%u]"), s_mapAllRen9Pos.size());
	OutputDebugString(strLog);
	std::multimap<size_t, CStlString> mapStatis;
	for (const auto& poss : s_mapAllRen9Pos) {
		const auto& group = poss.second;
		std::set<CStlString> setTemp;
		for (const auto& record : m_arrResultRecord) {
			CStlString temp;
			for (const auto& pos : group) {
				TCHAR c = _T('0') + record[pos];
				temp.append(1, c);
			}
			setTemp.insert(temp);
		}
		mapStatis.insert(std::pair<size_t, CStlString>(setTemp.size(), poss.first));
	}
	size_t allcount = 0;
	for (const auto& stat : mapStatis) {
		strLog.Format(_T("lua_StatisRen9 Item: [%s]=[%u]"), stat.second.c_str(), stat.first);
		OutputDebugString(strLog);
		allcount += stat.first;
	}
	strLog.Format(_T("lua_StatisRen9 End, allrecord count=[%u]"), allcount);
	OutputDebugString(strLog);
}



lua_State* CEngineLua::InitLua(CStlString& failed_reason) {
	m_strCheck28.clear();
	lua_State* L = luaL_newstate();
	if (L == NULL) {
		failed_reason = _T("luaL_newstate failed!");
		return NULL;
	}
	luaL_openlibs(L);
	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, CEngineLua::LUA_IsFilterTJ, 1);
	lua_setglobal(L, "IsFilterTJ");

	lua_pushcclosure(L, CEngineLua::LUA_DbgTrace, 0);
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

	if (lua_getglobal(L, "kMaxLose") != LUA_TNUMBER) {
		failed_reason = _T("kMaxLose invalid!");
		lua_close(L);
		return NULL;
	}
	m_lMaxLose = lua_tointeger(L, -1);
	lua_pop(L, 1);

	if (lua_getglobal(L, "kCheck28") == LUA_TSTRING) {
		m_strCheck28 = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	int topType = lua_getglobal(L, "kCalcRen9");
	if (topType == LUA_TNUMBER) {
		m_nCalcRen9 = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	if (m_nCalcRen9 != 0) {
		CEngine::InitAllRen9Pos();
	}
	if (lua_getglobal(L, "kRen9Pos") == LUA_TSTRING) {
		std::string val = lua_tostring(L, -1);
		ResetAllRen9Pos(val);
	}
	lua_pop(L, 1);

	return L;
}

void CEngineLua::TermLua(lua_State* state) {
	if (state != NULL) {
		lua_close(state);
	}
}

BOOL CEngineLua::IsFilterTJ(const CIntArray& record, const CStlString& strTJ) {
	if (IsFilterH(record, strTJ)) {
		return TRUE;
	}
	if (IsFilterL(record, strTJ)) {
		return TRUE;
	}
	if (IsFilterF(record, strTJ)) {
		return TRUE;
	}
	if (IsFilterW(record, strTJ)) {
		return TRUE;
	}
	if (IsFilterQ(record, strTJ)) {
		return TRUE;
	}
	if (IsFilterX(record, strTJ)) {
		return TRUE;
	}
	return FALSE;
}

//https://www.cnblogs.com/barrysgy/archive/2012/08/08/2679963.html
void CEngineLua::push_scriptfunc_params(lua_State *L, const CIntArray& record) {
	lua_newtable(L);
	CommonFilterFactors commFF;
	CalcCommonFilterFactors(record, m_arrPLData, m_arrGVData, m_arrPLScope, commFF);
	std::string codes(record.size(), '\0');
	for (size_t i = 0; i < codes.size(); i++) {
		codes[i] = '0' + record.at(i);
	}
	lua_pushstring(L, "codes");
	lua_pushstring(L, codes.c_str());
	lua_settable(L, -3);

	lua_pushstring(L, "count3");
	lua_pushinteger(L, commFF.mTotal3Count);
	lua_settable(L, -3);

	lua_pushstring(L, "count1");
	lua_pushinteger(L, commFF.mTotal1Count);
	lua_settable(L, -3);

	lua_pushstring(L, "count0");
	lua_pushinteger(L, commFF.mTotal0Count);
	lua_settable(L, -3);

	lua_pushstring(L, "lian3");
	lua_pushinteger(L, commFF.mLian3Count);
	lua_settable(L, -3);

	lua_pushstring(L, "lian1");
	lua_pushinteger(L, commFF.mLian1Count);
	lua_settable(L, -3);

	lua_pushstring(L, "lian0");
	lua_pushinteger(L, commFF.mLian0Count);
	lua_settable(L, -3);

	lua_pushstring(L, "breakcount");
	lua_pushinteger(L, commFF.mBreakCount);
	lua_settable(L, -3);

	lua_pushstring(L, "lian31");
	lua_pushinteger(L, commFF.mLian31Count);
	lua_settable(L, -3);

	lua_pushstring(L, "lian30");
	lua_pushinteger(L, commFF.mLian30Count);
	lua_settable(L, -3);

	lua_pushstring(L, "lian10");
	lua_pushinteger(L, commFF.mLian10Count);
	lua_settable(L, -3);

	lua_pushstring(L, "pl1");
	lua_pushinteger(L, commFF.mPL1Count);
	lua_settable(L, -3);

	lua_pushstring(L, "pl2");
	lua_pushinteger(L, commFF.mPL2Count);
	lua_settable(L, -3);

	lua_pushstring(L, "pl3");
	lua_pushinteger(L, commFF.mPL3Count);
	lua_settable(L, -3);

	lua_pushstring(L, "plsum");
	lua_pushnumber(L, commFF.mPLSum);
	lua_settable(L, -3);

	lua_pushstring(L, "gvj");
	lua_pushnumber(L, commFF.mGvj);
	lua_settable(L, -3);

	lua_pushstring(L, "plscope");
	lua_newtable(L);
	for (int i = 0; i < commFF.mPLScopes.size(); i++) {
		lua_pushinteger(L, i + 1);
		lua_pushinteger(L, commFF.mPLScopes[i]);
		lua_settable(L, -3);
	}
	lua_settable(L, -3);

	lua_pushstring(L, "plavg");
	lua_newtable(L);
	for (int i = 0; i < commFF.mPLAvgs.size(); i++) {
		lua_pushinteger(L, i + 1);
		lua_pushinteger(L, commFF.mPLAvgs[i]);
		lua_settable(L, -3);
	}
	lua_settable(L, -3);

	lua_pushstring(L, "plavgpos");
	lua_newtable(L);
	for (int i = 0; i < commFF.mPLAvgsPos.size(); i++) {
		lua_pushinteger(L, i + 1);
		lua_pushinteger(L, commFF.mPLAvgsPos[i]);
		lua_settable(L, -3);
	}
	lua_settable(L, -3);
}


BOOL CEngineLua::CalculateAllResult9(lua_State* L, CStlString& failed_reason) {
	m_arrAllRecord9.clear();
	CStringATL strLog = _T("lua_Calc9 Begin");
	OutputDebugString(strLog);
	for (const auto& poss : m_arrCalcRen9Pos) {
		const auto& poss_iter = s_mapAllRen9Pos.find(poss);
		if (poss_iter == s_mapAllRen9Pos.end()){
			continue;
		}
		const auto& group = poss_iter->second;
		CIntArray arrRecordR9(TOTO_COUNT, 8);
		size_t old_count = m_arrAllRecord9.size();
		for (const auto& record : m_arrResultRecord) {
			arrRecordR9.resize(TOTO_COUNT, 8);
			for (const auto& pos : group) {
				arrRecordR9[pos] = record[pos];
			}
			const auto& fIter = std::find(m_arrAllRecord9.begin(), 
				m_arrAllRecord9.end(), arrRecordR9);
			if (fIter == m_arrAllRecord9.end()) {
				if (IsAValidRecord9(arrRecordR9, L, poss, &failed_reason)) {
					m_arrAllRecord9.push_back(arrRecordR9);
				}
			}
		}
		strLog.Format(_T("lua_Calc9 Item[%s], count=[%d]"), 
			poss.c_str(), m_arrAllRecord9.size() - old_count);
		OutputDebugString(strLog);
	}
	std::stable_sort(m_arrAllRecord9.begin(), m_arrAllRecord9.end());
	m_arrAllRecord9.erase(std::unique(m_arrAllRecord9.begin(), m_arrAllRecord9.end()), m_arrAllRecord9.end());

	strLog.Format(_T("lua_Calc9 End, allrecord count=[%d]"), m_arrAllRecord9.size());
	OutputDebugString(strLog);
	return TRUE;
}

BOOL CEngineLua::IsAValidRecord9(const CIntArray& record, void* ctx, 
		const CStlString& line, CStlString* invalid_reason) {
	BOOL result = TRUE;
	lua_State *L = (lua_State *)ctx;
	lua_getglobal(L, "IsFilterLua9");					// 获取函数，压入栈中  

	push_scriptfunc_params(L, record);
	//增加ren9位置信息
	lua_pushstring(L, "ren9pos");
	lua_pushstring(L, line.c_str());
	lua_settable(L, -3);

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
			*invalid_reason = _T("lua9 function invalid result!");
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

void CEngineLua::GatherOneResult9(const CIntArray& record, int index, int depth,
	CIntArray& record9, CIntxyArray& allRecord9) {
	if (depth >= 9) {
		int not8_count = 0;
		for (const auto& code : record9) {
			if (code != 8) {
				not8_count++;
			}
		}
		if (not8_count == 9) {
			allRecord9.push_back(record9);
		}
		return;
	}
	for (int i = index; i < record.size(); i++) {
		record9[i] = record[i];
		GatherOneResult9(record, i + 1, depth + 1, record9, allRecord9);
		record9[i] = 8;
	}
}

void CEngineLua::ResetAllRen9Pos(const std::string& val) {
	m_arrCalcRen9Pos.clear();
	std::vector<std::string> arrPos;
	Global::DepartString(val, ",", arrPos);
	for (auto& line : arrPos) {
		Global::TrimBlank(line);
		if (!line.empty()) {
			for (const auto& item : s_mapAllRen9Pos) {
				int match_count = 0;
				for (const auto& c : line) {
					if (item.first.find(c) != std::string::npos) {
						match_count++;
					}
				}
				if (match_count == line.size() || match_count == 9) {
					m_arrCalcRen9Pos.push_back(item.first);
				}
			}
		}
	}
	std::stable_sort(m_arrCalcRen9Pos.begin(), m_arrCalcRen9Pos.end());
	m_arrCalcRen9Pos.erase(std::unique(m_arrCalcRen9Pos.begin(), m_arrCalcRen9Pos.end()), 
		m_arrCalcRen9Pos.end());
}