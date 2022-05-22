#include "stdafx.h"
#include "jc_defs.h"
#include "Global.h"

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

/*
std::string BetStruct::betCode() const {
	std::string ret;
	char cb[20] = { '\0' };
	sprintf(cb, "%d-%d", (int)tid, (int)code);
	ret = cb;
	return ret;
}
*/

std::string BetStruct::codeStr() const {
	std::string ret;
	char cb[20] = { '\0' };
	if (tid == 6 || tid == 1 || tid == 2 || tid == 7) {
		sprintf(cb, "%d", (int)code);
		ret = cb;
	}
	else if (tid == 4) {
		std::map<int, std::string> pair = {
			{0, "ʤ/ʤ,"},
			{1, "ʤ/ƽ,"},
			{2, "ʤ/��,"},
			{3, "ƽ/ʤ,"},
			{4, "ƽ/ƽ,"},
			{5, "ƽ/��,"},
			{6, "��/ʤ,"},
			{7, "��/ƽ,"},
			{8, "��/��,"},
		};
		const auto& iter = pair.find(code);
		if (iter != pair.end()) {
			ret = iter->second;
		}
	}
	else if (tid == 3) {
		std::vector<std::string> cn = { "1:0,", "2:0,", "2:1,", "3:0,", "3:1,", "3:2,", "4:0,", "4:1,", "4:2,", "5:0,", "5:1,", "5:2,", "ʤ����,", "0:0,", "1:1,", "2:2,", "3:3,", "ƽ����,", "0:1,", "0:2,", "1:2,", "0:3,", "1:3,", "2:3,", "0:4,", "1:4,", "2:4,", "0:5,", "1:5,", "2:5,", "������," };
		std::vector<std::string> bets = { "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60" };
		std::map<int, std::string> pair;
		for (int i = 0; i < bets.size(); i++) {
			int k = atoi(bets[i].c_str()) - 30;
			pair[k] = cn[k];
		}
		const auto& iter = pair.find(code);
		if (iter != pair.end()) {
			ret = iter->second;
		}
	}
	return ret;
}

std::string BetStruct::codeStrJC() const {
	std::string ret;
	char cb[20] = { '\0' };
	if (tid == 6) {
		if (code == 3) {
			ret = "ʤ,";
		} else if (code == 1) {
			ret = "ƽ,";
		} else if (code == 0) {
			ret = "��,";
		}
	}
	else if (tid == 1) {
		if (code == 3) {
			ret = "��ʤ,";
		}
		else if (code == 1) {
			ret = "��ƽ,";
		}
		else if (code == 0) {
			ret = "�ø�,";
		}
	} else if (tid == 2) {
		sprintf(cb, "%d", (int)code);
		ret = cb;
	} else if (tid == 4) {
		std::map<int, std::string> pair = {
			{0, "ʤ/ʤ,"},
			{1, "ʤ/ƽ,"},
			{2, "ʤ/��,"},
			{3, "ƽ/ʤ,"},
			{4, "ƽ/ƽ,"},
			{5, "ƽ/��,"},
			{6, "��/ʤ,"},
			{7, "��/ƽ,"},
			{8, "��/��,"},
		};
		const auto& iter = pair.find(code);
		if (iter != pair.end()) {
			ret = iter->second;
		}
	} else if (tid == 3) {
		std::vector<std::string> cn = { "1:0,", "2:0,", "2:1,", "3:0,", "3:1,", "3:2,", "4:0,", "4:1,", "4:2,", "5:0,", "5:1,", "5:2,", "ʤ����,", "0:0,", "1:1,", "2:2,", "3:3,", "ƽ����,", "0:1,", "0:2,", "1:2,", "0:3,", "1:3,", "2:3,", "0:4,", "1:4,", "2:4,", "0:5,", "1:5,", "2:5,", "������," };
		std::vector<std::string> bets = { "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60" };
		std::map<int, std::string> pair;
		for (int i = 0; i < bets.size(); i++) {
			int k = atoi(bets[i].c_str()) - 30;
			pair[k] = cn[k];
		}
		const auto& iter = pair.find(code);
		if (iter != pair.end()) {
			ret = iter->second;
		}
	}
	else if (tid == 7) {
		if (code == 1) {
			ret = "�µ�,";
		}
		else if (code == 2) {
			ret = "��˫,";
		}
		else if (code == 3) {
			ret = "�ϵ�,";
		}
		else if (code == 4) {
			ret = "��˫,";
		}
	}
	return ret;
}

int BetStruct::getPan() const {
	int ret = 0;
	if (odds == 0.00) {
		return 0;
	}
	if (tid == 6) {
		int use_hand = hand;
		if (use_hand == 0) {
			use_hand = odds_hand;
		}
		if (use_hand < 0) {
			if (code == 3) {
				ret = -1;
			} else if (code == 1) {
				ret = 3;
			} else if (code == 0) {
				ret = 4;
			}
		} else if(use_hand > 0){
			if (code == 3) {
				ret = 4;
			} else if (code == 1) {
				ret = 3;
			} else if (code == 0) {
				ret = -1;
			}
		}
	}
	else if (tid == 1) {
		if (hand < 0) {
			if (code == 3) {
				ret = 1;
			} else if (code == 1) {
				ret = 2;
			} else if (code == 0) {
				ret = -2;
			}
		} else if (hand > 0) {
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
			tip = "ʤ";
		}
		else if (betCode == 1) {
			tip = "ƽ";
		}
		else if (betCode == 0) {
			tip = "��";
		}
	} else if (tid == 1) {
		if (betCode == 3) {
			if (hand >= 0) {
				temp.Format("ʤ(+%d)", hand);
			}
			else {
				temp.Format("ʤ(%d)", hand);
			}
			tip = temp;
		}
		else if (betCode == 1) {
			tip = "ƽ";
		}
		else if (betCode == 0) {
			tip = "��";
		}
	}
	else if (tid == 2) {
		if (betCode < 7) {
			temp.Format("%d��", betCode);
		}
		else {
			temp.Format("%d+��", betCode);
		}
		tip = temp;
	}
	else if (tid == 4) {
		std::map<int, std::string> pair = {
			{0, "ʤ/ʤ"},
			{1, "ʤ/ƽ"},
			{2, "ʤ/��"},
			{3, "ƽ/ʤ"},
			{4, "ƽ/ƽ"},
			{5, "ƽ/��"},
			{6, "��/ʤ"},
			{7, "��/ƽ"},
			{8, "��/��"},
		};
		const auto& iter = pair.find(betCode);
		if (iter != pair.end()) {
			tip = iter->second;
		}
	}
	else if (tid == 3) {
		std::vector<std::string> cn = { "1:0", "2:0", "2:1", "3:0", "3:1", "3:2", "4:0", "4:1", "4:2", "5:0", "5:1", "5:2", "ʤ����", "0:0", "1:1", "2:2", "3:3", "ƽ����", "0:1", "0:2", "1:2", "0:3", "1:3", "2:3", "0:4", "1:4", "2:4", "0:5", "1:5", "2:5", "������" };
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
	else if (tid == 7) {
		if (betCode == 1) {
			tip = "�µ�";
		}
		else if (betCode == 2) {
			tip = "��˫";
		}
		else if (betCode == 3) {
			tip = "�ϵ�";
		}
		else if (betCode == 4) {
			tip = "��˫";
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

std::string JCMatchItem::Subject::betCodeStr() const {
	std::string ret;
	char cb[20] = { '\0' };
	sprintf(cb, "%d", (int)betCode);
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
		tipa = "��ʤ";
	}
	else if (tid == 1 && betCode == 1) {
		tipa = "��ƽ";
	}
	else if (tid == 1 && betCode == 0) {
		tipa = "�ø�";
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

CStringATL JCMatchItem::get_odds_string(int tid, int code) {
	CStringATL result;
	for (auto& sub : subjects) {
		if (sub.tid == tid && code == sub.betCode) {
			result.Format("%.2f", sub.odds);
		}
	}
	return result;
}


static void appendStatItem(std::map<std::string, std::vector<std::string>>& stat,
	const std::string& key, const char* fmt, int index) {
	CStringATL tmp;
	tmp.Format(fmt, index);
	auto& iter = stat.find(key);
	if (iter != stat.end()) {
		iter->second.push_back((LPCSTR)tmp);
	}
	else {
		std::vector<std::string> v;
		v.push_back((LPCSTR)tmp);
		stat[key] = v;
	}
}


CStringATL JCMatchItem::get_lua_clause(int match_index, JCMatchItem* owner,
		std::map<std::string, std::vector<std::string>>& stat) {
	CStringATL temp;
	if (owner->multi_selected) return temp;
	std::ostringstream oss;
	bool first = true;
	bool has_spf = false, has_rspf = false, has_danshuang = false;
	bool has_jq = false;
	for (auto& sub : subjects) {
		if (!sub.checked) continue;
		if (first) {
			first = false;
			std::string tmp = descrition;
			Global::TrimBlank(tmp);
			oss << _T("\n\t-- [") << match_index << _T("], [") << id << _T("], [") << match_category << "]" << _T(", [") << tmp << "]";
		}
		if (sub.tid == 6) {
			has_spf = true;
		}
		if (sub.tid == 1) {
			has_rspf = true;
		}
		if (sub.tid == 7) {
			has_danshuang = true;
		}
		if (sub.tid == 2) {
			has_jq = true;
		}
	}
	if (has_spf) {
		const char* fmt5 = "\n\tlocal code_%d_3 = GetIndexCodeCount(%d, codes, 6, 3);         --����%d��3�ĸ���[%s]";
		const char* fmt6 = "\n\tlocal code_%d_1 = GetIndexCodeCount(%d, codes, 6, 1);         --����%d��1�ĸ���[%s]";
		const char* fmt7 = "\n\tlocal code_%d_0 = GetIndexCodeCount(%d, codes, 6, 0);         --����%d��0�ĸ���[%s]";
		temp.Format(fmt5, match_index, match_index, match_index, get_odds_string(6, 3));
		oss << temp;
		temp.Format(fmt6, match_index, match_index, match_index, get_odds_string(6, 1));
		oss << temp;
		temp.Format(fmt7, match_index, match_index, match_index, get_odds_string(6, 0));
		oss << temp;
		const char* fmt1 = "\n\tlocal pan_%d_3 = GetIndexPanCount(%d, codes, 3);              --����%d������1�ĸ���";
		const char* fmt2 = "\n\tlocal pan_%d_4 = GetIndexPanCount(%d, codes, 4);              --����%d������2�ĸ���";
		const char* fmt3 = "\n\tlocal pan_%d_up = GetIndexPanCount(%d, codes, -1);            --����%d�����̵ĸ���";
		const char* fmt4 = "\n\tlocal pan_%d_down = pan_%d_3 + pan_%d_4;                       --����%d�����̵ĸ���";
		temp.Format(fmt1, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt2, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt3, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt4, match_index, match_index, match_index, match_index);
		oss << temp;
		appendStatItem(stat, "local code_3_sum", "code_%d_3", match_index);
		appendStatItem(stat, "local code_1_sum", "code_%d_1", match_index);
		appendStatItem(stat, "local code_0_sum", "code_%d_0", match_index);
		appendStatItem(stat, "local pan_3_sum", "pan_%d_3", match_index);
		appendStatItem(stat, "local pan_4_sum", "pan_%d_4", match_index);
		appendStatItem(stat, "local pan_up_sum", "pan_%d_up", match_index);
		appendStatItem(stat, "local pan_down_sum", "pan_%d_down", match_index);
	}
	if (has_rspf) {
		const char* fmt5 = "\n\tlocal code_%d_3 = GetIndexCodeCount(%d, codes, 1, 3);         --����%d��3�ĸ���[%s]";
		const char* fmt6 = "\n\tlocal code_%d_1 = GetIndexCodeCount(%d, codes, 1, 1);         --����%d��1�ĸ���[%s]";
		const char* fmt7 = "\n\tlocal code_%d_0 = GetIndexCodeCount(%d, codes, 1, 0);         --����%d��0�ĸ���[%s]";
		temp.Format(fmt5, match_index, match_index, match_index, get_odds_string(6, 3));
		oss << temp;
		temp.Format(fmt6, match_index, match_index, match_index, get_odds_string(6, 1));
		oss << temp;
		temp.Format(fmt7, match_index, match_index, match_index, get_odds_string(6, 0));
		oss << temp;
		const char* fmt1 = "\n\tlocal pan_%d_1 = GetIndexPanCount(%d, codes, 1);              --����%d������1�ĸ���";
		const char* fmt2 = "\n\tlocal pan_%d_2 = GetIndexPanCount(%d, codes, 2);              --����%d������2�ĸ���";
		const char* fmt3 = "\n\tlocal pan_%d_down = GetIndexPanCount(%d, codes, -2);          --����%d�����̵ĸ���";
		const char* fmt4 = "\n\tlocal pan_%d_up = pan_%d_1 + pan_%d_2;                         --����%d�����̵ĸ���";
		temp.Format(fmt1, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt2, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt4, match_index, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt3, match_index, match_index, match_index);
		oss << temp;
		appendStatItem(stat, "local rq_code_3_sum", "code_%d_3", match_index);
		appendStatItem(stat, "local rq_code_1_sum", "code_%d_1", match_index);
		appendStatItem(stat, "local rq_code_0_sum", "code_%d_0", match_index);
		appendStatItem(stat, "local pan_1_sum", "pan_%d_1", match_index);
		appendStatItem(stat, "local pan_2_sum", "pan_%d_2", match_index);
		appendStatItem(stat, "local pan_up_sum", "pan_%d_up", match_index);
		appendStatItem(stat, "local pan_down_sum", "pan_%d_down", match_index);
	}
	if (has_danshuang) {
		const char* fmt1 = "\n\tlocal ds_%d_1 = GetIndexCodeCount(%d, codes, 7, 1);              --����%d���µ��ĸ���";
		const char* fmt2 = "\n\tlocal ds_%d_2 = GetIndexCodeCount(%d, codes, 7, 2);              --����%d����˫�ĸ���";
		const char* fmt3 = "\n\tlocal ds_%d_3 = GetIndexCodeCount(%d, codes, 7, 3);              --����%d���ϵ��ĸ���";
		const char* fmt4 = "\n\tlocal ds_%d_4 = GetIndexCodeCount(%d, codes, 7, 4);              --����%d����˫�ĸ���";
		const char* fmt5 = "\n\tlocal ds_%d_down = ds_%d_1 + ds_%d_2;						 --����%d��С��ĸ���";
		const char* fmt6 = "\n\tlocal ds_%d_up = ds_%d_3 + ds_%d_4;                          --����%d�Ĵ���ĸ���";
		temp.Format(fmt1, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt2, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt3, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt4, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt5, match_index, match_index, match_index, match_index);
		oss << temp;
		temp.Format(fmt6, match_index, match_index, match_index, match_index);
		oss << temp;
		appendStatItem(stat, "local ds_1_sum", "ds_%d_1", match_index);
		appendStatItem(stat, "local ds_2_sum", "ds_%d_2", match_index);
		appendStatItem(stat, "local ds_3_sum", "ds_%d_3", match_index);
		appendStatItem(stat, "local ds_4_sum", "ds_%d_4", match_index);
		appendStatItem(stat, "local ds_up_sum", "ds_%d_up", match_index);
		appendStatItem(stat, "local ds_down_sum", "ds_%d_down", match_index);
	}
	if (has_jq) {
		const char* fmt1 = "\n\tlocal jq_%d_code = GetIndexCode(%d, codes, 2);              --����%d�Ľ�����";
		temp.Format(fmt1, match_index, match_index, match_index);
		oss << temp;
	}
	std::string result = oss.str();
	return result.c_str();
}


