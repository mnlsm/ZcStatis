#pragma once

#include "../JC/jc_defs.h"

class OkoooEngine {

public:
	OkoooEngine(const CStlString& script, const char* logf);

public:
	BOOL CalculateAllResult(CStlString& failed_reason);
	void getAllMatchIds(CStlStrArray& matchIds);
	void getResults(int start, int max_count, CStlStrArray& matchIds, CStlStrxyArray& records, bool& last);
	const std::vector<JcBetItemSource>& getSource() { return m_vecSources; }
	const TBetResult& getResult() { return m_vecResults; }
	const std::string& getFanAnTitle() { return m_strFanAnTitle; }
	const std::string& getFanAnDesc() { return m_strFanAnDesc; }
	void setScriptFile(const char* file) { m_strScriptFile = file; }
	const CStlString& getScriptFile() { return m_strScriptFile; }
	const std::vector<JcBetItemSource>& GetFixedSources() { return m_vecFixedSources; }

	void setCheckResult(const CStlString& result) { m_strCheckResult = result; }
	const CStlString& getCheckResult(){ return m_strCheckResult; }
	const CStlString& getScriptFileData() { return m_strScript; }

	void setMatchItems(const std::vector<std::shared_ptr<JCMatchItem>>& items) { m_vecMatchItems = items; }

protected:
	void SetSources(const std::vector<JcBetItemSource>& items);
	void SetFixedSources(const std::vector<JcBetItemSource>& items);
	BOOL CalculateAllResultImpl(CStlString& failed_reason);
	lua_State* InitLua(CStlString& failed_reason);
	void TermLua(lua_State* L);
	static int __cdecl LUA_DbgTrace(lua_State* L);
	static int __cdecl LUA_IsFilterTJ(lua_State* L);
	BOOL GeneratorBets(const std::vector<JcBetItemSource>& arrMatchScores, TBetResult& result);
	BOOL IsAValidRecordImpl(const std::vector<JcBetItem>& record, lua_State* L, double& bonus, CStlString* invalid_reason);
	void push_scriptfunc_params(lua_State* L, const std::vector<JcBetItem>& record);
	void gatherMatchBets(const std::vector<JcBetItemSource>& split_scores,
		int index, std::vector<JcBetItem>& record, TBetResult& result);
	void doAvgMultipleResult(const TBetResult& validResult, TBetResult& result);
	void doFilterByBetsRankRatio(const TBetResult& allResult, const TBetResult& validResult, TBetResult& result);

protected:
	CStlString m_strScript;
	CStlString m_strScriptFile;
	CStlString m_strLogPath;
	std::vector<JcBetItemSource> m_vecSources;
	std::vector<JcBetItemSource> m_vecFixedSources;
	TBetResult m_vecResults;
	TBetResult m_vecDiscardResults;

	CStlString m_strFanAnTitle, m_strFanAnDesc;
	int m_nMatchBetsLose;
	double m_dMinBonus;
	int m_nAvgMultiple;
	double m_dBetsRankRatioMax;
	double m_dBetsRankRatioMin;


	CStlString m_strCheckResult;
	std::vector<std::shared_ptr<JCMatchItem>> m_vecMatchItems;

};
