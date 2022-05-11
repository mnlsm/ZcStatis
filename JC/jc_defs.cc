#include "stdafx.h"
#include "jc_defs.h"

std::string BetStruct::betCode() const {
	std::string ret;
	char cb[20] = { '\0' };
	sprintf(cb, "%d-%d", (int)tid, (int)code);
	ret = cb;
	return ret;
}

int BetStruct::getPan() const {
	int ret = 0;
	if (odds == 0.00) {
		return 0;
	}
	if (tid == 6) {
		if (hand < 0) {
			if (code == 3) {
				ret = -1;
			}
			else if (code == 1) {
				ret = 3;
			}
			else if (code == 0) {
				ret = 4;
			}
		}
		else {
			if (code == 3) {
				ret = 4;
			}
			else if (code == 1) {
				ret = 3;
			}
			else if (code == 0) {
				ret = -1;
			}
		}

	}
	else if (tid == 1) {
		if (hand < 0) {
			if (code == 3) {
				ret = 1;
			}
			else if (code == 1) {
				ret = 2;
			}
			else if (code == 0) {
				ret = -2;
			}
		}
		else {
			if (code == 3) {
				ret = -2;
			}
			else if (code == 1) {
				ret = 2;
			}
			else if (code == 0) {
				ret = 1;
			}
		}
	}
	return ret;
}

void JCMatchItem::Subject::calcTip(int hand) {
	CStringATL temp;
	if (tid == 6) {
		if (betCode == 3) {
			tip = "胜";
		}
		else if (betCode == 1) {
			tip = "平";
		}
		else if (betCode == 0) {
			tip = "负";
		}
	} else if (tid == 1) {
		if (betCode == 3) {
			if (hand >= 0) {
				temp.Format("胜(+%d)", hand);
			}
			else {
				temp.Format("胜(%d)", hand);
			}
			tip = temp;
		}
		else if (betCode == 1) {
			tip = "平";
		}
		else if (betCode == 0) {
			tip = "负";
		}
	}
	else if (tid == 2) {
		if (betCode < 7) {
			temp.Format("%d球", betCode);
		}
		else {
			temp.Format("%d+球", betCode);
		}
		tip = temp;
	}
	else if (tid == 4) {
		std::map<int, std::string> pair = {
			{0, "胜/胜"},
			{1, "胜/平"},
			{2, "胜/负"},
			{3, "平/胜"},
			{4, "平/平"},
			{5, "平/负"},
			{6, "负/胜"},
			{7, "负/平"},
			{8, "负/负"},
		};
		const auto& iter = pair.find(betCode);
		if (iter != pair.end()) {
			tip = iter->second;
		}
	}
	else if (tid == 3) {
		std::vector<std::string> cn = { "1:0", "2:0", "2:1", "3:0", "3:1", "3:2", "4:0", "4:1", "4:2", "5:0", "5:1", "5:2", "胜其他", "0:0", "1:1", "2:2", "3:3", "平其他", "0:1", "0:2", "1:2", "0:3", "1:3", "2:3", "0:4", "1:4", "2:4", "0:5", "1:5", "2:5", "负其他" };
		std::vector<std::string> bets = { "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60" };
		std::map<int, std::string> pair;
		for (int i = 0; i < bets.size(); i++) {
			int k = atoi(bets[i].c_str()) - 30;
			pair[k] = cn[k];
		}
		const auto& iter = pair.find(betCode);
		if (iter != pair.end()) {
			tip = iter->second;
		}
	}
}

std::string JCMatchItem::Subject::betStr() const {
	std::string ret;
	char cb[20] = { '\0' };
	sprintf(cb, "%d-%d(%.2f)", (int)tid, (int)betCode, odds);
	ret = cb;
	return ret;
}

std::string JCMatchItem::Subject::lineStr() const {
	std::string ret;
	std::string buy = buyStr();
	ret.resize(64, ' ');
	char* pos = (char*)ret.c_str();
	char tmp[30] = { '\0' };
	sprintf(tmp, "tid=%I64d,", this->tid);
	memcpy(pos, tmp, strlen(tmp));

	pos += 8;
	sprintf(tmp, "code=%I64d,", this->betCode);
	memcpy(pos, tmp, strlen(tmp));

	pos += 10;
	sprintf(tmp, "tip=%s", buy.c_str());
	memcpy(pos, tmp, strlen(tmp));

	ret.append(1, '\n');
	return ret;
}

std::string JCMatchItem::Subject::buyStr() const {
	std::string tipa = this->tip;
	if (tid == 1 && betCode == 3) {
		tipa = "让胜";
	}
	else if (tid == 1 && betCode == 1) {
		tipa = "让平";
	}
	else if (tid == 1 && betCode == 0) {
		tipa = "让负";
	}
	return tipa;
}

int JCMatchItem::Subject::getPan(int hand) const {
	int ret = 0;
	if (odds == 0.00) {
		return 0;
	}
	if (tid == 6) {
		if (hand < 0) {
			if (betCode == 3) {
				ret = -1;
			}
			else if (betCode == 1) {
				ret = 3;
			}
			else if (betCode == 0) {
				ret = 4;
			}
		}
		else {
			if (betCode == 3) {
				ret = 4;
			}
			else if (betCode == 1) {
				ret = 3;
			}
			else if (betCode == 0) {
				ret = -1;
			}
		}
	}
	else if (tid == 1) {
		if (hand < 0) {
			if (betCode == 3) {
				ret = 1;
			}
			else if (betCode == 1) {
				ret = 2;
			}
			else if (betCode == 0) {
				ret = -2;
			}
		}
		else {
			if (betCode == 3) {
				ret = -2;
			}
			else if (betCode == 1) {
				ret = 2;
			}
			else if (betCode == 0) {
				ret = 1;
			}
		}
	}
	return ret;
}

JCMatchItem::Subject* JCMatchItem::get_subject(int tid, int betCode) {
	JCMatchItem::Subject* result = NULL;
	for (auto& subject : subjects) {
		if (subject.tid == tid && subject.betCode == betCode) {
			result = &subject;
			break;
		}
	}
	return result;
}

JCMatchItem::Subject* JCMatchItem::get_subject(int tid, const char* tip) {
	JCMatchItem::Subject* result = NULL;
	for (auto& subject : subjects) {
		if (subject.tid == tid && subject.tip.compare(tip) == 0) {
			result = &subject;
			break;
		}
	}
	return result;
}

CStringA CreateMatchDescription(const CStringA& ahost, const CStringA& aaway) {
	const char* vs("  VS  ");
	const char* buf = "                   ";
	CStringA result;
	CStringA host(buf), away(buf);
	memcpy((char*)(LPCSTR)host + host.GetLength() - ahost.GetLength(), (LPCSTR)ahost, ahost.GetLength());
	memcpy((char*)(LPCSTR)away, (LPCSTR)aaway, aaway.GetLength());
	result = host + vs + away;
	return result;
}
