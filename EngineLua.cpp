#include "stdafx.h"
#include "EngineLua.h"

int __cdecl CEngineLua::LUA_IsFilterTJ(lua_State *L) {
	BOOL ret = TRUE;
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
	TermLua(lua_state);
	return result;
}

BOOL CEngineLua::IsAValidRecord(const CIntArray& record, CStlString& failed_reason) {
	failed_reason.clear();
	BOOL bRet = FALSE;
	if (record.size() != TOTO_COUNT) {
		failed_reason = _T("source record invalid!\n");
		return bRet;
	}
	int index = 0;
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
			CStlString codes(TOTO_COUNT, '\0');
			for (int i = 0; i < TOTO_COUNT; i++) {
				codes[i] = record[i] + _T('0');
			}
			TCHAR szInfo[128] = { _T('\0') };
			_stprintf(szInfo, _T("found record: codes=[%s], index=%d, losed=%d\n"),
				codes.c_str(), index, TOTO_COUNT - samecount);
			failed_reason += szInfo;
		}
	}
	if (!bRet) {
		lua_State* lua_state = InitLua(failed_reason);
		if (lua_state != NULL) {
			if (IsAValidRecordImpl(record, lua_state, &failed_reason)) {
				failed_reason = _T("filter by compress\n");
			}
		}
		TermLua(lua_state);
	}
	return bRet;
}


BOOL CEngineLua::IsAValidRecordImpl(const CIntArray& record, void* ctx, CStlString* invalid_reason) {
	BOOL result = TRUE;
	lua_State *L = (lua_State *)ctx;
	lua_getglobal(L, "IsFilterLua");					// 获取函数，压入栈中  
	push_scriptfunc_params(L, record);
	int call_ret = lua_pcall(L, 1, 2, 0);			    // 调用函数，调用完成以后，会将返回值压入栈中，1表示参数个数，2表示返回结果个数。
	if (call_ret != 0) {
		if (invalid_reason != NULL) {
			*invalid_reason = CA2T(lua_tostring(L, -1)).m_psz;
		}
		lua_pop(L, 1);
		return FALSE;
	}
	if (lua_type(L, -1) != LUA_TSTRING || lua_type(L, -2) != LUA_TNUMBER) {
		if (invalid_reason != NULL) {
			*invalid_reason = _T("exception by lua function result!");
		}
		lua_pop(L, 2);
		return FALSE;
	}
	const char* error = lua_tostring(L, -1);
	int lua_ret = lua_tointeger(L, -2);
	if (lua_ret != 0) {
		if (invalid_reason != NULL) {
			*invalid_reason = CA2T(error).m_psz;
		}
	}
	lua_pop(L, 2);
	return (lua_ret != 1);
}

lua_State* CEngineLua::InitLua(CStlString& failed_reason) {
	lua_State* L = luaL_newstate();
	if (L == NULL) {
		failed_reason = _T("luaL_newstate failed!");
		return NULL;
	}

	luaL_openlibs(L);

	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, CEngineLua::LUA_IsFilterTJ, 1);
	lua_setglobal(L, "IsFilterTJ");

	if (luaL_dostring(L, m_strScript.c_str()) != 0) {
		failed_reason = _T("luaL_dostring failed!");
		lua_close(L);
		return NULL;
	}
	lua_getglobal(L, "kMaxLose");
	if(lua_type(L, -1) != LUA_TNUMBER) {
		failed_reason = _T("kMaxLose invalid!");
		lua_close(L);
		return NULL;
	}
	m_lMaxLose = lua_tointeger(L, -1);
	lua_pop(L, 1);
	return L;
}

void CEngineLua::TermLua(lua_State* state) {
	if (state != NULL) {
		lua_close(state);
	}
}


BOOL CEngineLua::IsFilterTJ(const CIntArray& record, const CStlString& strTJ) {

	return FALSE;
}


//https://www.cnblogs.com/barrysgy/archive/2012/08/08/2679963.html
void CEngineLua::push_scriptfunc_params(lua_State *L, const CIntArray& record) {
	lua_newtable(L);
	lua_pushstring(L, "thisptr");
	lua_pushlightuserdata(L, this);
	lua_settable(L, -3);

	CommonFilterFactors commFF;
	CalcCommonFilterFactors(record, m_arrPLData, m_arrGVData, m_arrPLScope, commFF);
	std::string codes(record.size(), '\0');
	for (size_t i = 0; i < codes.size(); i++) {
		codes[i] = '0' + record.at(i);
	}
	lua_pushstring(L, "codes");
	lua_pushstring(L, codes.c_str());
	lua_settable(L, -3);

	lua_pushstring(L, "total3");
	lua_pushinteger(L, commFF.mTotal3Count);
	lua_settable(L, -3);

	lua_pushstring(L, "total1");
	lua_pushinteger(L, commFF.mTotal1Count);
	lua_settable(L, -3);

	lua_pushstring(L, "total0");
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

	std::string plscope;
	for (const auto& scope : commFF.mPLScopes) {
		char cNum[20] = { '\0' };
		sprintf(cNum, "%u", scope);
		if (plscope.empty()) {
			plscope = cNum;
		}
		else {
			plscope = plscope + ',' + cNum;
		}
	}
	lua_pushstring(L, "plscope");
	lua_pushstring(L, plscope.c_str());
	lua_settable(L, -3);
}
