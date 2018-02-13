#include "stdafx.h"
#include "DanLueEngine.h"
#include "Global.h"

static const std::string dbgview_prefix = "jc_dbgtrace: ";
static const std::string dbgview_exception = "jc_exception: ";


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
	} else {
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

DanLueEngine::DanLueEngine(const CStlString& script) {
	m_strScript = script;
}

int __cdecl DanLueEngine::LUA_DbgTrace(lua_State *L) {
	if (lua_type(L, 1) == LUA_TSTRING) {
		std::string line = dbgview_prefix + lua_tostring(L, 1);
		OutputDebugStringA(line.c_str());
	}
	return 0;
}

int __cdecl DanLueEngine::LUA_IsFilterTJ(lua_State *L) {
	BOOL ret = FALSE;
	/*
	if (lua_type(L, 1) == LUA_TSTRING && lua_type(L, 2) == LUA_TSTRING) {
		const char* codes = lua_tostring(L, 1);
		const char* tj = lua_tostring(L, 2);
		int codes_len = strlen(codes);
		CIntArray tempArr;
		for (int i = 0; i < codes_len; i++) {
			tempArr.push_back(codes[i] - '0');
		}
		DanLueEngine* pThis = (DanLueEngine*)lua_touserdata(L, lua_upvalueindex(1));
		ret = pThis->IsFilterTJ(tempArr, tj);
	}
	*/
	lua_pushinteger(L, ret ? 1 : 0);
	return 1;
}


BOOL DanLueEngine::CalculateAllResult(CStlString& failed_reason) {
	m_vecSources.clear();
	m_vecResults.clear();
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

void DanLueEngine::SetSources(const std::vector<JcBetItemSource>& items) {
	m_vecSources = items;
}

BOOL DanLueEngine::CalculateAllResultImpl(CStlString& failed_reason) {
	BOOL result = FALSE;
	lua_State* lua_state = InitLua(failed_reason);
	if (lua_state == NULL) {
		return FALSE;
	}
	
	TBetResult allResult, tempAll;
	if (GeneratorBets(m_vecSources, allResult)) {
		//std::stable_sort(allResult.begin(), allResult.end());
		//allResult.erase(std::unique(allResult.begin(), allResult.end()), allResult.end());
		for (const auto& record : allResult) {
			if (IsAValidRecordImpl(record, lua_state, &failed_reason)) {
				tempAll.push_back(record);
			}
		}
		m_vecResults.swap(tempAll);
		result = TRUE;
	}
	else {
		failed_reason = "GeneratorCodes failed!";
	}

	TermLua(lua_state);
	return result;
}

lua_State* DanLueEngine::InitLua(CStlString& failed_reason) {
	//m_strMatchScores.clear();
	lua_State* L = luaL_newstate();
	if (L == NULL) {
		failed_reason = _T("luaL_newstate failed!");
		return NULL;
	}
	luaL_openlibs(L);
	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, DanLueEngine::LUA_IsFilterTJ, 1);
	lua_setglobal(L, "IsFilterTJ");

	lua_pushcclosure(L, DanLueEngine::LUA_DbgTrace, 0);
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

	if (lua_getglobal(L, "kMatchTitle") == LUA_TTABLE) {
		m_strFanAnTitle = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	if (lua_getglobal(L, "kMatchDesc") == LUA_TTABLE) {
		m_strFanAnDesc = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	CStlStrArray arrMatchBets;
	if (lua_getglobal(L, "kMatchBets") == LUA_TTABLE) {
		int index = 0;
		while (++index > 0) {
			std::string codes = lua_table_getfield(L, index, "none");
			if (codes == "none") {
				break;
			}
			arrMatchBets.push_back(codes);
		}
		lua_pop(L, 1);
	}
	if (arrMatchBets.empty()) {
		failed_reason = _T("no match scores!!");
		lua_close(L);
		return NULL;
	}
	std::vector<JcBetItemSource> sources;
	for (const auto& match : arrMatchBets) {
		CStlStrArray arrParts;
		Global::DepartString(match, ";", arrParts);
		if (arrParts.size() < 3) {
			failed_reason = _T("match score format error 1!");
			lua_close(L);
			return NULL;
		}
		JcBetItemSource jbs;
		jbs.id = arrParts[0];
		int hand = atoi(arrParts[1].c_str());
		for (int i = 2; i < arrParts.size(); i++) {
			CStlStrArray arrBets;
			Global::DepartString(arrParts[i], ",", arrBets);
			if (arrBets.size() != 3) {
				failed_reason = _T("match score format error 2!");
				lua_close(L);
				return NULL;
			}
			BetStruct bs;
			bs.hand = hand;
			bs.tid = atoi(arrBets[0].c_str());
			bs.code = atoi(arrBets[1].c_str());
			bs.odds = atof(arrBets[2].c_str());
			jbs.bets.push_back(bs);
		}
		sources.push_back(jbs);
	}
	if (sources.empty()) {
		failed_reason = _T("no match scores 1 !!");
		lua_close(L);
		return NULL;
	}
	SetSources(sources);

	return L;
}

void DanLueEngine::TermLua(lua_State* L) {
	if (L != NULL) {
		lua_close(L);
	}
}

static void gatherMatchBets(const std::vector<JcBetItemSource>& split_scores,
	int index, std::vector<JcBetItem>& record, TBetResult& result) {
	if (record.size() == split_scores.size()) {
		result.push_back(record);
	}
	if (index >= split_scores.size()) {
		return;
	}
	const auto& codes = split_scores[index];
	{
		for (const auto& code : codes.bets) {
			JcBetItem item;
			item.id = codes.id;
			item.bet = code;
			record.push_back(item);
			gatherMatchBets(split_scores, index + 1, record, result);
			record.pop_back();
		}
	}
}

BOOL DanLueEngine::GeneratorBets(const std::vector<JcBetItemSource>& arrMatchScores, 
		TBetResult& result) {
	std::vector<JcBetItem> record;
	gatherMatchBets(arrMatchScores, 0, record, result);
	return TRUE;
}

BOOL DanLueEngine::IsAValidRecordImpl(const std::vector<JcBetItem>& record, 
		lua_State* L, CStlString* invalid_reason) {
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

void DanLueEngine::push_scriptfunc_params(lua_State *L, const std::vector<JcBetItem>& record) {
	lua_newtable(L);

	lua_pushstring(L, "betcodes");
	lua_newtable(L);
	for (int i = 0; i < record.size(); i++) {
		const JcBetItem& item = record.at(i);
		lua_pushinteger(L, i + 1);
		if (TRUE) {
			lua_newtable(L);
			lua_pushstring(L, "id");
			lua_pushstring(L, item.id.c_str());
			lua_settable(L, -3);
			lua_pushstring(L, "tid");
			lua_pushinteger(L, item.bet.tid);
			lua_settable(L, -3);
			lua_pushstring(L, "code");
			lua_pushinteger(L, item.bet.code);
			lua_settable(L, -3);
			lua_pushstring(L, "hand");
			lua_pushinteger(L, item.bet.hand);
			lua_settable(L, -3);
			lua_pushstring(L, "odds");
			lua_pushnumber(L, item.bet.odds);
			lua_settable(L, -3);
		}
		lua_settable(L, -3);
	}
	lua_settable(L, -3);

}

void DanLueEngine::getMatchIds(CStlStrArray& matchIds) {
	matchIds.clear();
	for (const auto& v : m_vecSources) {
		matchIds.push_back(v.id);
	}
}

void DanLueEngine::getResults(CStlStrxyArray& records) {
	records.clear();
	for (const auto& record : m_vecResults) {
		CStlStrArray vecRecord;
		for (const auto& item : record) {
			char bet[32];
			sprintf(bet, "%d-%d", item.bet.tid, item.bet.code);
			vecRecord.push_back(bet);
		}
		records.push_back(vecRecord);
	}
}
