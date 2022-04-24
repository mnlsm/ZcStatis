#pragma once
#include <string>
#include <vector>

struct BetStruct {
	int hand;
	int tid;
	int code;
	double odds;
	int getPan() const;
	std::string betCode() const;
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

struct JCMatchItem {
	std::string id;
	std::string match_category;
	std::string descrition;
	std::string start_time;
	std::string last_buy_time;
	int64 hand;
	std::string result;
	std::string orderid;
	std::string match_url;
	struct Subject {
		int64 tid;
		int64 betCode;
		double odds;
		std::string tip;
		bool checked;
		void calcTip(int hand);
		std::string betStr() const;
		std::string lineStr() const;
		std::string buyStr() const;
		int getPan(int hand) const;
	};
	std::vector<Subject> subjects;
	Subject* get_subject(int tid, int betCode);
	Subject* get_subject(int tid, const char* tip);
};