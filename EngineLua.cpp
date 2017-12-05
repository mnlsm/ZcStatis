#include "stdafx.h"
#include "EngineLua.h"

int __cdecl CEngineLua::LUA_IsFilterTJ(lua_State *L) {
	BOOL ret = TRUE;
	if (lua_isstring(L, 1) == 0 && lua_isstring(L, 2) == 0) {
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
	lua_pushboolean(L, ret ? 1 : 0);
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

BOOL CEngineLua::IsAValidRecord(const CIntArray& record, void* ctx, CStlString* invalid_reason) {
	lua_State *L = (lua_State *)ctx;
	lua_getglobal(L, "IsAValidRecord");					// 获取函数，压入栈中  
	push_scriptfunc_params(L, record);
	int call_ret = lua_pcall(L, 1, 2, 0); // 调用函数，调用完成以后，会将返回值压入栈中，1表示参数个数，2表示返回结果个数。
	if (call_ret != 0) {
		if (invalid_reason != NULL) {
			*invalid_reason = CA2T(lua_tostring(L, -1)).m_psz;
		}
		lua_pop(L, 1);
		return FALSE;
	}
	if (lua_isstring(L, -1) == 0 && lua_isinteger(L, -2) == 0) {
		const char* error = lua_tostring(L, -1);
		int lua_ret = lua_tointeger(L, -1);
		if (lua_ret != 0) {
			if (invalid_reason != NULL) {
				*invalid_reason = CA2T(error).m_psz;
			}
			lua_pop(L, 2);
			return FALSE;
		}
	}
	lua_pop(L, 2);
	return TRUE;
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
		} else {
			plscope = plscope + cNum;
		}
	}
	lua_pushstring(L, "plscope");
	lua_pushstring(L, plscope.c_str());
	lua_settable(L, -3);
}


lua_State* CEngineLua::InitLua(CStlString& failed_reason) {
	lua_State* lua_state = luaL_newstate();
	if (lua_state == NULL) {
		failed_reason = _T("luaL_newstate failed!");
		return lua_state;
	}

	luaL_openlibs(lua_state);

	lua_pushlightuserdata(lua_state, this);
	lua_pushcclosure(lua_state, CEngineLua::LUA_IsFilterTJ, 1);
	lua_setglobal(lua_state, "IsFilterTJ");

	if (luaL_dostring(lua_state, m_strScript.c_str()) != 0) {
		failed_reason = _T("luaL_dostring failed!");
		lua_close(lua_state);
		lua_state = NULL;
	}

	return lua_state;
}

void CEngineLua::TermLua(lua_State* state) {
	if (state != NULL) {
		lua_close(state);
	}
}


BOOL CEngineLua::IsFilterTJ(const CIntArray& record, const CStlString& strTJ) {

	return FALSE;
}
