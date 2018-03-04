#pragma once

struct BetStruct {
	int hand;
	int tid;
	int code;
	double odds;
	int getPan() const {
		int ret = 0;
		if (odds == 0.00) {
			return 0;
		}
		if (tid == 6) {
			if (hand < 0) {
				if (code == 3) {
					ret = -1;
				} else if (code == 1) {
					ret = 3;
				} else if (code == 0) {
					ret = 4;
				}
			} else {
				if (code == 3) {
					ret = 4;
				} else if (code == 1) {
					ret = 3;
				} else if (code == 0) {
					ret = -1;
				}
			}

		} else if (tid == 1) {
			if (hand < 0) {
				if (code == 3) {
					ret = 1;
				} else if (code == 1) {
					ret = 2;
				} else if (code == 0) {
					ret = -2;
				}
			} else {
				if (code == 3) {
					ret = -2;
				} else if (code == 1) {
					ret = 2;
				} else if (code == 0) {
					ret = 1;
				}
			}
		}
		return ret;
	}
};

struct JcBetItemSource {
	std::string id;
	std::vector<BetStruct> bets;
};

struct JcBetItem {
	std::string id;
	BetStruct bet;
};

typedef std::vector<std::vector<JcBetItem>> TBetResult;

class DanLueEngine {

public:
	DanLueEngine(const CStlString& script, const char* logf);

public:
	BOOL CalculateAllResult(CStlString& failed_reason);
	//inline const TBetResult& GetResult(){ return m_vecResults; }
	//const std::vector<JcBetItemSource>& GetSources(){ return m_vecSources;  }
	void getMatchIds(CStlStrArray& matchIds);
	void getResults(CStlStrxyArray& records);
	const std::vector<JcBetItemSource>& getSource() { return m_vecSources; }
	const TBetResult& getResult() { return m_vecResults; }
	const std::string& getFanAnTitle() { return m_strFanAnTitle; }
	const std::string& getFanAnDesc() { return m_strFanAnDesc; }
	void setScriptFile(const char* file) { m_strScriptFile = file; }
	const CStlString& getScriptFile() { return m_strScriptFile; }

protected:
	void SetSources(const std::vector<JcBetItemSource>& items);
	BOOL CalculateAllResultImpl(CStlString& failed_reason);
	lua_State* InitLua(CStlString& failed_reason);
	void TermLua(lua_State* L);
	static int __cdecl LUA_DbgTrace(lua_State *L);
	static int __cdecl LUA_IsFilterTJ(lua_State *L);
	BOOL GeneratorBets(const std::vector<JcBetItemSource>& arrMatchScores, TBetResult& result);
	BOOL IsAValidRecordImpl(const std::vector<JcBetItem>& record, lua_State* L, double& bonus, CStlString* invalid_reason);
	void push_scriptfunc_params(lua_State *L, const std::vector<JcBetItem>& record);

protected:
	CStlString m_strScript;
	CStlString m_strScriptFile;
	CStlString m_strLogPath;
	std::vector<JcBetItemSource> m_vecSources;
	TBetResult m_vecResults;
	CStlString m_strFanAnTitle, m_strFanAnDesc;
	double m_dMinBonus;

};
