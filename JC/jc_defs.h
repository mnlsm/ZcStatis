#pragma once
#include <string>
#include <vector>
#include <sstream>

struct BetStruct {
	BetStruct() { hand = odds_hand = 0; }
	int hand;
	int odds_hand;
	int tid;
	int code;
	double odds;
	int getPan() const;
	//std::string betCode() const;
	std::string codeStr() const;
	std::string codeStrJC() const;
};

struct JcBetItemSource {
	std::string id;
	std::vector<BetStruct> bets;
	bool multi_selected;
};

struct JcBetItem {
	std::string id;
	BetStruct bet;
};

typedef std::vector<std::vector<JcBetItem>> TBetResult;

struct JCMatchItem {
	JCMatchItem() { multi_selected = false; hand = 0; odds_hand = 0; }
	std::string id;
	std::string match_category;
	std::string descrition;
	std::string start_time;
	std::string last_buy_time;
	int64 hand;
	std::string result;
	std::string orderid;
	std::string match_url;
	bool multi_selected;
	int64 odds_hand;
	struct Subject {
		int64 tid;
		int64 betCode;
		double odds;
		std::string tip;
		bool checked;
		void calcTip(int hand);
		std::string betStr() const;
		std::string betCodeStr() const;
		std::string lineStr() const;
		std::string buyStr() const;
		int getPan(int hand) const;
	};
	std::vector<Subject> subjects;
	Subject* get_subject(int tid, int betCode);
	Subject* get_subject(int tid, const char* tip);

	CStringATL get_lua_clause(int match_index, JCMatchItem* owner,
		std::map<std::string, std::vector<std::string>>& stat);
	CStringATL get_odds_string(int tid, int code);
};

CStringA CreateMatchDescription(const CStringA& ahost, const CStringA& aaway);
