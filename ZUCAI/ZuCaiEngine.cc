#include "stdafx.h"
#include "ZuCaiEngine.h"
#include "Global.h"


static const std::string dbgview_prefix = "";
static const std::string dbgview_exception = "jc_exception: ";


static double lua_table_getdouble(lua_State* L, const char* key, double defval) {
	double result = defval;
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if (lua_type(L, -1) == LUA_TNUMBER) {
		result = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);
	return result;
}

static int lua_table_getfield(lua_State* L, const char* key, int defval) {
	int result = defval;
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if (lua_type(L, -1) == LUA_TNUMBER) {
		result = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return result;
}

static std::string lua_table_getfield(lua_State* L, const char* key,
	const char* defval) {
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

static std::string lua_table_getfield(lua_State* L, int index, const char* defval) {
	std::string result;
	lua_pushnumber(L, index);
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

ZuCaiEngine::ZuCaiEngine(const CStlString& script, const char* logf) {
	m_strScript = script;
	m_dMinBonus = 0.0;
	m_nMatchBetsLose = 0;
	m_nAvgMultiple = 0;
	m_dBetsRankRatioMin = 1.0;
	m_dBetsRankRatioMax = 1.0;
	if (logf != NULL) {
		m_strLogPath = logf;
		m_strLogPath += "\\debug.log";
		DeleteFileA(m_strLogPath.c_str());
	}
}

int __cdecl ZuCaiEngine::LUA_DbgTrace(lua_State* L) {
	if (lua_type(L, 1) == LUA_TSTRING) {
		std::string line = dbgview_prefix + lua_tostring(L, 1);
		ZuCaiEngine* pThis = (ZuCaiEngine*)lua_touserdata(L, lua_upvalueindex(1));
		const CStlString& path = pThis->m_strLogPath;
		if (!path.empty()) {
			Global::SaveFileData(path.c_str(), line, TRUE);
		}
		else {
			OutputDebugStringA(line.c_str());
		}
	}
	return 0;
}

int __cdecl ZuCaiEngine::LUA_IsFilterTJ(lua_State* L) {
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
		ZuCaiEngine* pThis = (ZuCaiEngine*)lua_touserdata(L, lua_upvalueindex(1));
		ret = pThis->IsFilterTJ(tempArr, tj);
	}
	*/
	lua_pushinteger(L, ret ? 1 : 0);
	return 1;
}


BOOL ZuCaiEngine::CalculateAllResult(CStlString& failed_reason) {
	failed_reason.clear();
	m_vecFixedSources.clear();
	m_vecSources.clear();
	m_vecResults.clear();
	m_vecDiscardResults.clear();
	DeleteFile(m_strLogPath.c_str());
	BOOL result = CalculateAllResultImpl(failed_reason);
	if (!result) {
		if (failed_reason.empty()) {
			failed_reason = "unknow";
		}
	}
	Global::TrimBlank(failed_reason);
	if (!failed_reason.empty()) {
		CStlString trace = dbgview_exception + failed_reason;
		Global::SaveFileData(m_strLogPath.c_str(), trace, FALSE);
	}
	return result;
}

void ZuCaiEngine::SetSources(const std::vector<JcBetItemSource>& items) {
	m_vecSources = items;
	//调整赔率变化
	for (auto& item : m_vecSources) {
		for (auto& mitem : m_vecMatchItems) {
			if (mitem->id == item.id) {
				for (auto& sub : item.bets) {
					for (auto& sitem : mitem->subjects) {
						if (sub.tid == sitem.tid && sub.code == sitem.betCode) {
							sub.odds = sitem.odds;
							break;
						}
					}
				}
				break;
			}
		}
	}
}

void ZuCaiEngine::SetFixedSources(const std::vector<JcBetItemSource>& items) {
	m_vecFixedSources = items;
}

BOOL ZuCaiEngine::CalculateAllResultImpl(CStlString& failed_reason) {
	BOOL result = FALSE;
	lua_State* lua_state = InitLua(failed_reason);
	if (lua_state == NULL) {
		return FALSE;
	}
	double bonus = 0.0;
	TBetResult allResult, tempAll, discardAll, validAll, filterAll;
	if (GeneratorBets(m_vecSources, allResult)) {
		for (const auto& record : allResult) {
			CStlString error_text;
			if (IsAValidRecordImpl(record, lua_state, bonus, &error_text)) {
				validAll.push_back(record);
			} else {
				discardAll.push_back(record);
			}
			if (!error_text.empty()) {
				failed_reason.append("\n").append(error_text);
			}
		}
		doFilterByBetsRankRatio(allResult, validAll, filterAll);
		doAvgMultipleResult(filterAll, tempAll);
		m_vecResults.swap(tempAll);
		m_vecDiscardResults.swap(discardAll);
		result = TRUE;
	} else {
		failed_reason.append("\n").append("GeneratorCodes failed!");
	}
	TermLua(lua_state);
	return result;
}

void ZuCaiEngine::doFilterByBetsRankRatio(const TBetResult& allResult, const TBetResult& validResult, TBetResult& result) {
	result.clear();
	if (m_dBetsRankRatioMax >= 1.0) {
		result = validResult;
		return;
	}
	
	std::vector<double> bets;
	for (const auto& record : allResult) {
		double temp = 1.0;
		for (const auto& item : record) {
			temp = temp * item.bet.odds;
		}
		bets.emplace_back(temp);
	}
	std::sort(bets.begin(), bets.end(), std::less<double>());
	int index = (int)((double)bets.size() * m_dBetsRankRatioMax + 1);
	if (index >= bets.size()) index = bets.size() - 1;
	double betMax = bets[index];
	index = (int)((double)bets.size() * m_dBetsRankRatioMin);
	double betMin = bets[index];
	for (const auto& record : validResult) {
		double temp = 1.0;
		for (const auto& item : record) {
			temp = temp * item.bet.odds;
		}
		if (temp >= betMin && temp <= betMax) {
			result.emplace_back(record);
		}
	}
}


void ZuCaiEngine::doAvgMultipleResult(const TBetResult& validResult, TBetResult& result) {
	result.clear();
	double avg_bonus = 0.0;
	for (const auto& row : validResult) {
		double row_bonus = 0.0;
		for (const auto& col : row) {
			if (row_bonus == 0.0) {
				row_bonus = col.bet.odds;
			} else {
				row_bonus = row_bonus * col.bet.odds;
			}
		}
		row_bonus = row_bonus * 2;
		avg_bonus += row_bonus;
		if (m_nAvgMultiple == 0) {
			result.push_back(row);
			double multi_bonus = row_bonus;
			while (multi_bonus < m_dMinBonus) {
				result.push_back(row);
				multi_bonus += row_bonus;
			}
		}
	}
	avg_bonus = avg_bonus / validResult.size();
	if (m_nAvgMultiple > 0) {
		result.clear();
		for (const auto& row : validResult) {
			double row_bonus = 0.0;
			for (const auto& col : row) {
				if (row_bonus == 0.0) {
					row_bonus = col.bet.odds;
				} else {
					row_bonus = row_bonus * col.bet.odds;
				}
			}
			row_bonus = row_bonus * 2;
			int multiple = (int)(round((double)m_nAvgMultiple * (avg_bonus / row_bonus)));
			if (multiple <= 0) multiple = 1;
			if (row_bonus * multiple < m_dMinBonus && m_dMinBonus > 0.0) {
				multiple = multiple + 1;
			}
			while (multiple > 0) {
				result.push_back(row);
				multiple = multiple - 1;
			}
		}
	}
}


static void getJcBetItemSource(lua_State* L, const char* key, std::vector<JcBetItemSource>& sources) {
	sources.clear();
	CStlStrArray arrMatchBets;
	//"kMatchBets"
	if (lua_getglobal(L, key) == LUA_TTABLE) {
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
		return;
	}
	for (const auto& match : arrMatchBets) {
		CStlStrArray arrParts;
		Global::DepartString(match, ";", arrParts);
		if (arrParts.size() < 3) {
			return;
		}
		JcBetItemSource jbs;
		jbs.id = arrParts[0];
		int hand = atoi(arrParts[1].c_str());
		for (int i = 2; i < arrParts.size(); i++) {
			CStlStrArray arrBets;
			Global::DepartString(arrParts[i], ",", arrBets);
			if (arrBets.size() != 3) {
				return;
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
}

lua_State* ZuCaiEngine::InitLua(CStlString& failed_reason) {
	lua_State* L = luaL_newstate();
	if (L == NULL) {
		failed_reason.append("\n").append(_T("luaL_newstate failed!"));
		return NULL;
	}
	luaL_openlibs(L);
	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, ZuCaiEngine::LUA_IsFilterTJ, 1);
	lua_setglobal(L, "IsFilterTJ");

	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, ZuCaiEngine::LUA_DbgTrace, 1);
	lua_setglobal(L, "dbgview_print");

	if (luaL_dostring(L, m_strScript.c_str()) != 0) {
		failed_reason.append("\n").append(_T("luaL_dostring failed: "));
		if (lua_type(L, -1) == LUA_TSTRING) {
			std::string err_text = dbgview_exception + lua_tostring(L, -1);
			failed_reason.append(CA2T(err_text.c_str()).m_psz);
			lua_pop(L, 1);
		}
		lua_close(L);
		return NULL;
	}

	if (lua_getglobal(L, "kMatchBetsLose") == LUA_TNUMBER) {
		m_nMatchBetsLose = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}

	if (lua_getglobal(L, "kAvgMultiple") == LUA_TNUMBER) {
		m_nAvgMultiple = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}

	if (lua_getglobal(L, "kMinBonus") == LUA_TNUMBER) {
		m_dMinBonus = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}

	if (lua_getglobal(L, "kBetsRankRatioRange") == LUA_TSTRING) {
		//m_dBetsRankRatio = lua_tonumber(L, -1);
		CStlString ratioRange = lua_tostring(L, -1);
		lua_pop(L, 1);
		std::vector<CStlString> arrParts;
		Global::DepartString(ratioRange, "-", arrParts);
		if (arrParts.size() == 2) {
			double lower = std::atof(arrParts[0].c_str());
			double upper = std::atof(arrParts[1].c_str());
			if (lower >= 0.0 && upper <= 1.0 && lower <= upper) {
				m_dBetsRankRatioMin = lower;
				m_dBetsRankRatioMax = upper;
			}
		}
	}

	if (lua_getglobal(L, "kMatchTitle") == LUA_TSTRING) {
		m_strFanAnTitle = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	if (lua_getglobal(L, "kMatchDesc") == LUA_TSTRING) {
		m_strFanAnDesc = lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	std::vector<JcBetItemSource> sources;
	getJcBetItemSource(L, "kMatchBets", sources);
	if (sources.empty()) {
		failed_reason.append("\n").append(_T("no match scores 1 !!"));
		lua_close(L);
		return NULL;
	}
	SetSources(sources);
	if (m_nMatchBetsLose + 1 >= sources.size()) {
		m_nMatchBetsLose = 0;
	}
	getJcBetItemSource(L, "kMatchBetsFixed", sources);
	SetFixedSources(sources);

	return L;
}

void ZuCaiEngine::TermLua(lua_State* L) {
	if (L != NULL) {
		lua_close(L);
	}
}

void ZuCaiEngine::gatherMatchBets(const std::vector<JcBetItemSource>& split_scores,
		int index, std::vector<JcBetItem>& record, TBetResult& result) {
	int matchCount = split_scores.size() - m_nMatchBetsLose;
	int leftCount = split_scores.size() - index;
	if (record.size() >= matchCount) {
		result.push_back(record);
		return;
	}
	if (leftCount + record.size() < matchCount) {
		return;
	}
	for (int i = index; i < split_scores.size(); i++) {
		const auto& codes = split_scores[i];
		{
			for (const auto& code : codes.bets) {
				JcBetItem item;
				item.id = codes.id;
				item.bet = code;
				record.push_back(item);
				gatherMatchBets(split_scores, i + 1, record, result);
				record.pop_back();
			}
		}
	}
}

BOOL ZuCaiEngine::GeneratorBets(const std::vector<JcBetItemSource>& arrMatchScores,
	TBetResult& result) {
	std::vector<JcBetItem> record;
	gatherMatchBets(arrMatchScores, 0, record, result);
	return TRUE;
}

BOOL ZuCaiEngine::IsAValidRecordImpl(const std::vector<JcBetItem>& record,
	lua_State* L, double& bonus, CStlString* invalid_reason) {
	bonus = 0.0;
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
	else {
		bonus = lua_table_getdouble(L, "bonus", 0.0);
	}
	lua_pop(L, 1);
	return isValid;
}

void ZuCaiEngine::push_scriptfunc_params(lua_State* L, const std::vector<JcBetItem>& record) {
	lua_newtable(L);
	double betbouns = 0.0;
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
			if (betbouns == 0.0) {
				betbouns = item.bet.odds;
			} else {
				betbouns = betbouns * item.bet.odds;
			}
			lua_pushstring(L, "pan");
			lua_pushinteger(L, item.bet.getPan());
			lua_settable(L, -3);
		}
		lua_settable(L, -3);
	}
	lua_settable(L, -3);

	lua_pushstring(L, "betbouns");
	lua_pushnumber(L, 2 * betbouns);
	lua_settable(L, -3);
}

void ZuCaiEngine::getAllMatchIds(CStlStrArray& matchIds) {
	matchIds.clear();
	for (const auto& v : m_vecSources) {
		matchIds.push_back(v.id);
	}
}

void ZuCaiEngine::getResults(int start, int max_count,
	CStlStrArray& matchIds, CStlStrxyArray& records, bool& last) {
	last = false;
	records.clear();
	matchIds.clear();
	if (start >= m_vecResults.size()) {
		return;
	}
	int i = start, count = 0;
	for (; i < m_vecResults.size(); i++) {
		if (++count > max_count) {
			break;
		}
		const auto& record = m_vecResults.at(i);
		CStlStrArray vecRecord, vecMatchIds;
		for (const auto& item : record) {
			char bet[32] = { '\0' };
			sprintf(bet, "%d-%d", item.bet.tid, item.bet.code);
			vecMatchIds.push_back(item.id);
			vecRecord.push_back(bet);
		}
		if (matchIds.empty()) {
			matchIds = vecMatchIds;
		}
		else if (matchIds != vecMatchIds) {
			break;
		}
		records.push_back(vecRecord);
	}
	last = (i >= m_vecResults.size());
}
