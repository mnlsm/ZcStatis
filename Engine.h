#pragma once

typedef struct CommonFilterFactorsTag {
	CommonFilterFactorsTag() {
		Clear(); 
	}
	int mTotal3Count;
	int mTotal1Count;
	int mTotal0Count;

	int mLian3Count;
	int mLian1Count;
	int mLian0Count;

	int mBreakCount;

	int mLian31Count;
	int mLian30Count;
	int mLian10Count;

	int mPL1Count;
	int mPL2Count;
	int mPL3Count;

	double mPLSum;
	double mGvj;
	std::vector<int> mPLScopes;
	std::vector<int> mPLAvgs;
	std::vector<int> mPLAvgsPos;


	void Clear() {
		mTotal3Count = 0;
		mTotal1Count = 0;
		mTotal0Count = 0;
		mLian3Count = 0;
		mLian1Count = 0;
		mLian0Count = 0;
		mBreakCount = 0;
		mLian31Count = 0;
		mLian30Count = 0;
		mLian10Count = 0;
		mPL1Count = 0;
		mPL2Count = 0;
		mPL3Count = 0;
		mPLSum = 0.0;
		mGvj = 0.0;
		mPLScopes.clear();
		mPLAvgs.resize(6, 0);
		mPLAvgsPos.resize(TOTO_COUNT, -1);
	}
} CommonFilterFactors;


class CEngine {
public:
	CEngine();
	virtual ~CEngine();

public:
	void SetChoices(const CStlString& strChoices);
	void SetDZRecords(const CStlString& strRecords);
	void SetDZRecords9(const CStlString& strRecords);
	void SetPL(const CStlString &pl);

	void SetMaxLose(long lLose);
	void SetChoices(const CIntxyArray& arrChoices);
	void SetDZRecords(const CIntxyArray& arrRecords);

	const CIntxyArray& GetResult();
	virtual const CIntxyArray& GetResult9() { return m_arrAllRecord9; }
	const CIntxyArray& GetAllRecord() { return m_arrAllRecord; }
	const int* GetResultFenBu() { return (const int *)m_arrRecordFenBu; }
	const int GetInitRecordsCount() { return m_iInitRecordCount; }

public:
	//void static WriteRecordsToFile(const CStlString& filename, CIntxyArray &arrAllRecord);


public:
	virtual BOOL CalculateAllResult(CStlString& failed_reason);
	virtual BOOL IsAValidRecord(const CIntArray& record, CStlString& failed_reason);

protected:
	virtual BOOL CalculateAllResultImpl(void* ctx, CStlString& failed_reason);
	virtual BOOL IsAValidRecordImpl(const CIntArray& record, void* ctx, CStlString* invalid_reason);
	virtual void FilterG(){};
	virtual void StatisRen9() {};

//for suoshui
protected:							
	long m_lMaxRate;			//Max compress data
	long m_lMaxLimit;			//Save MAXLimit	
	long m_lMaxLose;			//Max Lose (0-13)

	
	CStlString m_strPL;
	CIntxyArray m_arrChoices;			//save user's choiced data
	CIntxyArray m_arrAllRecord;				//save All record!

	CIntxyArray m_arrCoverIndex;		//save cover Index
	CIntxyArray m_arrResultRecord;		//save rect result

	CDoublexyArray m_arrPLData;
	CDoublexyArray m_arrGVData;
	CDoubleArray m_arrPLScope;

	int m_iInitRecordCount;
	int m_arrRecordFenBu[TOTO_COUNT * 3];

	CIntxyArray m_arrAllRecord9;

protected:
	static void SearchAllRecord(CIntxyArray::iterator iter, CIntxyArray::iterator iter_end, 
		CIntArray &tempArr, CIntxyArray& arrAllRecord);
	bool FillAllCoverIndex(CIntxyArray &xyAll);
	void GetCoverIndexArr(CIntxyArray::iterator iter, CIntxyArray &arrSource, CIntArray &arrResult);
	void GreedyCalcRectRecord(CIntxyArray &F, CIntxyArray &G);


protected:
	virtual BOOL IsFilterH(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr = NULL);
	virtual BOOL IsFilterL(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr = NULL);
	virtual BOOL IsFilterF(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr = NULL);
	virtual BOOL IsFilterW(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr = NULL);
	virtual BOOL IsFilterQ(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr = NULL);
	virtual BOOL IsFilterX(const CIntArray &tempArr, const std::string& strTJ, std::string *pStr = NULL);
	virtual void doFilterG(const CStlString& strG);

public:
	static BOOL GetChoices(const CStlString& strChoices, CIntxyArray& arrChoices);
	static BOOL GetRecords(const CStlString& strCodes, CIntxyArray& arrRecords);
	static UINT GetRecordsCount(const CStlString& strCodes);
	static BOOL GetRecord(const CStlString& strCode, CIntArray& arrRecord);

	static CStlString GetRecordString(const CIntArray& arrRecord);
	static void GetRecordsString(const CIntxyArray& arrRecords, CStlString& strRecords);
	static void GetRecordsPrintRecords(const CIntxyArray& arrRecords, CStlStrArray& records);

	static BOOL GetPLDatas(const CStlString& strPL, CDoublexyArray& arrPLData, CDoublexyArray& arrGVData);
	static BOOL CalcCommonFilterFactors(const CIntArray& record, const CDoublexyArray& arrPLData,
		const CDoublexyArray& arrGVData, const CDoubleArray& arrPLScope, CommonFilterFactors& commonFF);

private:
	static BOOL GetLianXu(const CIntArray& record, int &nMaxSP, int &nMaxSF, int &nMaxPF);

private:
	struct FilterX_Params {
		FilterX_Params() {
			clear();
		}
		CStlString tj;
		bool isValid;
		CStlString filepath;
		int range_begin;
		int range_end;
		CIntxyArray arrRecords;

		void clear() {
			isValid = false;
			filepath = _T("");
			range_begin = -1;
			range_end = -1;
			arrRecords.clear();
		}
	};
	std::map<std::string, FilterX_Params> m_mapFilterX;

public:
	static void InitAllRen9Pos();

protected:
	static std::map<CStlString, CIntArray> s_mapAllRen9Pos;
	static void gatherAllRen9Pos(const CIntArray& source, CIntArray& tempArr,
		int start, int depth);
};