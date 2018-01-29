#pragma once
class DanLueEngine {

public:
	DanLueEngine(const CStlString& script);

public:
	BOOL CalculateAllResult(CStlString& failed_reason);
	const CIntxyArray& GetResult() { return m_arrResult; };
	void GetResultString(CStlString& result);

protected:
	CStlString m_strScript;

protected:
	CIntxyArray m_arrResult;
};
