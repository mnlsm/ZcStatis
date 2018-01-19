#pragma once
class JQCEngine {
public:
	JQCEngine(const CStlString& script);
	BOOL CalculateAllResult(CStlString& failed_reason);



protected:
	BOOL CalculateAllResultImpl(CStlString& failed_reason);
	lua_State* InitLua(CStlString& failed_reason);
	void TermLua(lua_State* state);
	BOOL GeneratorCodes(const CStlString& strMatchScores, CIntxyArray& result);
	BOOL IsAValidRecordImpl(const CIntArray& record, lua_State* L, CStlString* invalid_reason);

private:
	static int __cdecl LUA_IsFilterTJ(lua_State *L);
	static int __cdecl LUA_DbgTrace(lua_State *L);
	void push_scriptfunc_params(lua_State *L, const CIntArray& record);
	BOOL IsFilterTJ(const CIntArray& record, const CStlString& strTJ);

protected:
	CStlString m_strScript;
	CStlString m_strMatchScores;
};