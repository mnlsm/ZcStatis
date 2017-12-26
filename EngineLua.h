#pragma once
#include "Engine.h"


class CEngineLua : public CEngine {
public:
	CEngineLua(const CStlString& script);
	virtual ~CEngineLua();


public:
	virtual BOOL CalculateAllResult(CStlString& failed_reason);
	virtual BOOL IsAValidRecord(const CIntArray& record, CStlString& failed_reason);

protected:
	virtual BOOL IsAValidRecordImpl(const CIntArray& record, void* ctx, CStlString* invalid_reason);


protected:
	lua_State* InitLua(CStlString& failed_reason);
	void TermLua(lua_State* state);

private:
	static int __cdecl LUA_IsFilterTJ(lua_State *L);
	static int __cdecl LUA_DbgTrace(lua_State *L);

	BOOL IsFilterTJ(const CIntArray& record, const CStlString& strTJ);
	void push_scriptfunc_params(lua_State *L, const CIntArray& record);

protected:
	CStlString m_strScript;

private:
	int m_nCalcRen9;
	CIntxyArray m_arrRen9Pos;

	void ResetAllRen9Pos(const std::string& val);
	BOOL CalculateAllResult9(lua_State* L, CStlString& failed_reason);
	void GatherOneResult9(const CIntArray& record, int index, int depth, CIntArray& record9, CIntxyArray& allRecord9);
	BOOL IsAValidRecord9(const CIntArray& record, void* ctx, CStlString* invalid_reason);

};