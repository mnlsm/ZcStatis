#include "stdafx.h"
#include "BeiDanDialog.h"
#include "Global.h"
#include "ZlibStream.h"
#include "MiscHelper.h"


BOOL BeiDanDialog::OnIdle() {
	int async_count = GetAsyncFuncCount();
	if (async_count > 0) {
		PostMessage(BeiDanDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	if (!m_delayDeleteBrowsers.empty()) {
		for (int i = (int)m_delayDeleteBrowsers.size() - 1; i >= 0; i--) {
			if (m_delayDeleteBrowsers[i]->IsWindowDestroyed()) {
				m_delayDeleteBrowsers.erase(m_delayDeleteBrowsers.begin() + i);
			}
		}
	}
	return FALSE;
}

LRESULT BeiDanDialog::OnAsyncDispatch(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DispathOneAsyncFunc();
	return 1L;
}

bool BeiDanDialog::AddOneAsyncFunc(talk_base::IAsyncFuncCall* pAsyncFunc) {
	BOOL bRet = CAsyncFuncDispatcher::AddOneAsyncFunc(pAsyncFunc);
	if (bRet) {
		bRet = PostMessage(BeiDanDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	return (bRet == TRUE);
}

void BeiDanDialog::OnHttpReturnGlobal(const CHttpRequestPtr& request,
	const CHttpResponseDataPtr& response) {
	sInst.AddOneAsyncFunc(talk_base::BindAsyncFunc(&BeiDanDialog::OnHttpReturn, &sInst, request, response));
}

CHttpRequestPtr BeiDanDialog::CreatePostRequest(const std::string& url, const std::string& idprefix,
	const std::string& data) {
	std::string request_id = CHttpClientMgr::generateRequestId();
	request_id = idprefix + "_" + request_id;
	CHttpRequestPtr ptr(new (std::nothrow) THttpRequestData());
	ptr->agent = "Dalvik/2.1.0 (Linux; U; Android 6.0; M5s Build/MRA58K)";
	ptr->request_headers.insert(std::make_pair("accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"));
	ptr->request_headers.insert(std::make_pair("Connection", "Keep-Alive"));
	ptr->request_headers.insert(std::make_pair("Accept-Encoding", "gzip"));
	//ptr->request_headers.insert(std::make_pair("cookie", "LastUrl=; __utmz=56961525.1650162259.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); FirstURL=kj.okooo.com / ; FirstOKURL=https % 3A//www.okooo.com/danchang/kaijiang/; First_Source=kj.okooo.com; _ga=GA1.2.1953148046.1650162259; PHPSESSID=f10e97ef44b47fd188dce251c03e1d4816228600; __utma=56961525.1953148046.1650162259.1651827501.1652296390.3; __utmc=56961525; pm=; LStatus=N; LoginStr=%7B%22welcome%22%3A%22%u60A8%u597D%uFF0C%u6B22%u8FCE%u60A8%22%2C%22login%22%3A%22%u767B%u5F55%22%2C%22register%22%3A%22%u6CE8%u518C%22%2C%22TrustLoginArr%22%3A%7B%22alipay%22%3A%7B%22LoginCn%22%3A%22%u652F%u4ED8%u5B9D%22%7D%2C%22tenpay%22%3A%7B%22LoginCn%22%3A%22%u8D22%u4ED8%u901A%22%7D%2C%22weibo%22%3A%7B%22LoginCn%22%3A%22%u65B0%u6D6A%u5FAE%u535A%22%7D%2C%22renren%22%3A%7B%22LoginCn%22%3A%22%u4EBA%u4EBA%u7F51%22%7D%2C%22baidu%22%3A%7B%22LoginCn%22%3A%22%u767E%u5EA6%22%7D%2C%22snda%22%3A%7B%22LoginCn%22%3A%22%u76DB%u5927%u767B%u5F55%22%7D%7D%2C%22userlevel%22%3A%22%22%2C%22flog%22%3A%22hidden%22%2C%22UserInfo%22%3A%22%22%2C%22loginSession%22%3A%22___GlobalSession%22%7D; acw_tc=2f624a7c16522984571547396e7b12d938bc88db7e47bf16f3047017ed7a19; acw_sc__v3=627c15baccdabfeed9a6a907689e5c1f59c6a40c; Hm_lvt_213d524a1d07274f17dfa17b79db318f=1650162260,1651827501,1652299222; Hm_lpvt_213d524a1d07274f17dfa17b79db318f=1652299222; __utmb=56961525.18.8.1652299221829"));
	ptr->request_url = url;
	ptr->request_id = request_id;
	ptr->request_type = HTTP_REQUEST_POST;
	ptr->request_post_data = data;
	ptr->response_type = RESPONSEMEMORY;
	ptr->request_time = 1000 * 60;
	ptr->callback = &BeiDanDialog::OnHttpReturnGlobal;
	//httpMgr_->DoHttpCommandRequest(ptr);
	return ptr;
}

CHttpRequestPtr BeiDanDialog::CreateGetRequest(const std::string& url, const std::string& idprefix) {
	std::string request_id = CHttpClientMgr::generateRequestId();
	request_id = idprefix + "_" + request_id;
	CHttpRequestPtr ptr(new (std::nothrow) THttpRequestData());
	ptr->agent = "Dalvik/2.1.0 (Linux; U; Android 6.0; M5s Build/MRA58K)";
	ptr->request_headers.insert(std::make_pair("accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"));
	ptr->request_headers.insert(std::make_pair("Connection", "Keep-Alive"));
	ptr->request_headers.insert(std::make_pair("Accept-Encoding", "gzip"));
	//ptr->request_headers.insert(std::make_pair("cookie", "LastUrl=; __utmz=56961525.1650162259.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); FirstURL=kj.okooo.com / ; FirstOKURL=https % 3A//www.okooo.com/danchang/kaijiang/; First_Source=kj.okooo.com; _ga=GA1.2.1953148046.1650162259; PHPSESSID=f10e97ef44b47fd188dce251c03e1d4816228600; __utma=56961525.1953148046.1650162259.1651827501.1652296390.3; __utmc=56961525; pm=; LStatus=N; LoginStr=%7B%22welcome%22%3A%22%u60A8%u597D%uFF0C%u6B22%u8FCE%u60A8%22%2C%22login%22%3A%22%u767B%u5F55%22%2C%22register%22%3A%22%u6CE8%u518C%22%2C%22TrustLoginArr%22%3A%7B%22alipay%22%3A%7B%22LoginCn%22%3A%22%u652F%u4ED8%u5B9D%22%7D%2C%22tenpay%22%3A%7B%22LoginCn%22%3A%22%u8D22%u4ED8%u901A%22%7D%2C%22weibo%22%3A%7B%22LoginCn%22%3A%22%u65B0%u6D6A%u5FAE%u535A%22%7D%2C%22renren%22%3A%7B%22LoginCn%22%3A%22%u4EBA%u4EBA%u7F51%22%7D%2C%22baidu%22%3A%7B%22LoginCn%22%3A%22%u767E%u5EA6%22%7D%2C%22snda%22%3A%7B%22LoginCn%22%3A%22%u76DB%u5927%u767B%u5F55%22%7D%7D%2C%22userlevel%22%3A%22%22%2C%22flog%22%3A%22hidden%22%2C%22UserInfo%22%3A%22%22%2C%22loginSession%22%3A%22___GlobalSession%22%7D; acw_tc=2f624a7c16522984571547396e7b12d938bc88db7e47bf16f3047017ed7a19; acw_sc__v3=627c15baccdabfeed9a6a907689e5c1f59c6a40c; Hm_lvt_213d524a1d07274f17dfa17b79db318f=1650162260,1651827501,1652299222; Hm_lpvt_213d524a1d07274f17dfa17b79db318f=1652299222; __utmb=56961525.18.8.1652299221829"));
	ptr->request_url = url;
	ptr->request_id = request_id;
	ptr->request_type = HTTP_REQUEST_GET;
	//ptr->request_post_data = query->GetCloudPathsRequestData();
	ptr->response_type = RESPONSEMEMORY;
	ptr->request_time = 1000 * 60;
	ptr->callback = &BeiDanDialog::OnHttpReturnGlobal;
	return ptr;
	//httpMgr_->DoHttpCommandRequest(ptr);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static const char* LOGIN_REQ_PREFIX = "login_req_prefix";
static const char* LOGOFF_REQ_PREFIX = "logoff_req_prefix";
static const char* INFO_REQ_PREFIX = "info_req_prefix";
static const char* RCTOKEN_REQ_PREFIX = "rctoken_req_prefix";
static const char* FRIENDLIST_REQ_PREFIX = "friendlist_req_prefix";
static const char* LOTTERYCATEGORIES_REQ_PREFIX = "lotterycategories_req_prefix";
static const char* JCMATCHLIST_REQ_PREFIX = "jcmatchlist_req_prefix";
static const char* HEMAI_REQ_PREFIX = "hemai_req_prefix";
static const char* HEMAI_REQ_PREFIX_FINISH = "hemai_req_prefix_finish";
static const char* BIFEN_REQ_PREFIX = "bifen_req_prefix";


static const char* BEIDANWDL_REQ_PREFIX = "beidanwdl_req_prefix";
static const char* BEIDANDS_REQ_PREFIX = "beidandanshuang_req_prefix";
static const char* BEIDANJQ_REQ_PREFIX = "beidanjingqiu_req_prefix";



void BeiDanDialog::OnHttpReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (request->request_id.find(LOGIN_REQ_PREFIX) == 0) {
		OnLoginReturn(request, response);
	}
	else if (request->request_id.find(JCMATCHLIST_REQ_PREFIX) == 0) {
		OnJcMatchListReturn(request, response);
	}
	else if (request->request_id.find(BIFEN_REQ_PREFIX) == 0) {
		OnBiFenReturn(request, response);
	}
	else if (request->request_id.find(BEIDANWDL_REQ_PREFIX) == 0) {
		OnBeiDanWDLReturn(request, response);
	}
	else if (request->request_id.find(BEIDANDS_REQ_PREFIX) == 0) {
		OnBeiDanDSReturn(request, response);
	}
	else if (request->request_id.find(BEIDANJQ_REQ_PREFIX) == 0) {
		OnBeiDanJQReturn(request, response);
	}
}

int BeiDanDialog::doLogin() {
	if (TRUE) {
		std::string url = "https://m.okooo.com/weixin/jing/d.php";
		CHttpRequestPtr request = CreateGetRequest(url, LOGIN_REQ_PREFIX);
		httpMgr_->DoHttpCommandRequest(request);
	}
	m_buLogin.EnableWindow(FALSE);
	m_buLogoff.EnableWindow(FALSE);
	return 0;
}


void BeiDanDialog::OnLoginReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		doJcMatchList();
	} else {
		MessageBoxA("登录失败，网络错误!", "提示", MB_OK);
		m_buLogin.EnableWindow(TRUE);
		m_buLogoff.EnableWindow(FALSE);
	}
}

int BeiDanDialog::doLogOff() {
	m_buLogin.EnableWindow(TRUE);
	m_buLogoff.EnableWindow(FALSE);
	return 0l;
}


int BeiDanDialog::doJcMatchList() {
	m_stProgress.SetWindowText("");
	std::string url = "https://www.okooo.com/danchang/";
	CHttpRequestPtr request = CreateGetRequest(url, JCMATCHLIST_REQ_PREFIX);
	httpMgr_->DoHttpCommandRequest(request);
	return 0l;
}

static void adjustXmlText(CStringA& temp) {
	int nFindBegin = -1, nFindEnd = -1;
	CStringA s = "<input ";
	CStringA e = ">";
	nFindBegin = temp.Find(s, 0);
	if (nFindBegin >= 0) {
		nFindEnd = temp.Find(e, nFindBegin);
		if (nFindEnd >= 0) {
			CStringA text = temp.Mid(nFindBegin, nFindEnd - nFindBegin + e.GetLength());
			temp.Replace(text, "");
		}
	}
}

static CStringA adjustXuHaoText(const CStringA& temp) {
	int len = temp.GetLength();
	if (len == 0) {
		return "000";
	} else if (len == 1) {
		return CStringA("00") + temp;
	} else if (len == 2) {
		return CStringA("0") + temp;
	}
	return temp;
}

void BeiDanDialog::OnJcMatchListReturn(const CHttpRequestPtr& request,
		const CHttpResponseDataPtr& response) {
	m_pending_request = 0;
	m_order_items.clear();

	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		std::string raw_response;
		if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
			CZlibStream zlib;
			zlib.DecompressGZip(response->response_content, raw_response);
		} else {
			raw_response = response->response_content;
		}
		int acp_code = CP_ACP;
		if (raw_response.find("UTF-8") != std::string::npos
			|| raw_response.find("utf-8") != std::string::npos) {
			acp_code = CP_UTF8;
		}
		CStringA temp = CW2A(CA2W(raw_response.c_str(), acp_code).m_psz).m_psz;

		CStringATL curTime = Global::GetTimeString();
		CStringATL expireTime = Global::GetNextDayString() + " 10:00:00";
		int nFindBegin = -1, nFindEnd = -1;
		CStringA section_begin = "<tr class=\"alltrObj";
		CStringA section_end = "</tr>";
		nFindBegin = 0;
		while ((nFindBegin = temp.Find(section_begin, nFindBegin)) >= 0) {
			nFindEnd = temp.Find(section_end, nFindBegin);
			if (nFindEnd == -1) {
				break;
			}
			CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
			adjustXmlText(xmlText);
			if (xmlText.IsEmpty()) {
				MessageBox("获取对阵列表失败 1！", "错误", MB_ICONERROR | MB_OK);
				return;
			}
			nFindBegin = nFindEnd + section_end.GetLength();
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLElement* tempElement = nullptr;
			if (doc.Parse(xmlText) != tinyxml2::XML_SUCCESS) {
				MessageBox("获取对阵列表失败 2！", "错误", MB_ICONERROR | MB_OK);
				break;
			}
			tinyxml2::XMLElement* child = doc.FirstChildElement();
			if (child == nullptr) {
				MessageBox("获取对阵列表失败 3！", "错误", MB_ICONERROR | MB_OK);
				break;
			}
			auto node = FindElementByClassAttr(child, "xh");
			if (node == nullptr) {
				MessageBox("获取对阵列表失败 4！", "错误", MB_ICONERROR | MB_OK);
				break;
			}
			CStringA xuhao = GetElementText(node->FirstChildElement());
			CStringA match_category = GetElementText(FindElementByClassAttr(child, "ls"));
			CStringA start_time = GetElementAttrValue(FindElementByClassAttr(
				child, "switchtime timetd td2"), "title");
			start_time.Replace("比赛时间：", ""); start_time.Trim();

			CStringA home = GetElementAttrValue(FindElementByClassAttr(
				child, "homenameobj homename"), "title");
			CStringA away = GetElementAttrValue(FindElementByClassAttr(
				child, "awaynameobj awayname"), "title");
			CStringA href = GetElementAttrValue(FindElementByClassAttr(
				child, "dc-link"), "href");
			href.Replace("odds", "history");
			href.Replace("node", "history");
			CStringA orderid = href;
			orderid.Replace("/soccer/match/", "");
			orderid.Replace("/history/", "");
			int cmp = strcmp(start_time, curTime);
			if (cmp <= 0) {
				continue;
			}
			cmp = strcmp(start_time, expireTime);
			if (cmp > 0) {
				break;
			}
			std::shared_ptr<JCMatchItem> ji(new JCMatchItem());
			ji->id = adjustXuHaoText(xuhao);
			ji->start_time = start_time;
			ji->match_category = match_category;
			ji->start_time = ji->last_buy_time = start_time;
			ji->descrition = CreateMatchDescription(home, away);
			ji->match_url = std::string("https://m.okooo.com/match/game.php?MatchID=") + (LPCSTR)orderid + "&from=";
			ji->orderid = orderid;
			auto homeNode = FindElementByClassAttr(child, "sbg");
			if (homeNode != nullptr) {
				CStringA hand = GetElementText(FindElementByClassAttr(homeNode, "handicapobj font_red"));
				if (hand.IsEmpty()) {
					hand = GetElementText(FindElementByClassAttr(homeNode, "handicapobj font_green"));
				}
				hand.Replace(")", "");hand.Replace("(", "");hand.Replace("+", "");
				ji->hand = atoi(hand);
				CStringA pl = GetElementText(FindElementByClassAttr(homeNode, "pltxt"));
				JCMatchItem::Subject sub;
				sub.tid = 1;
				sub.odds = atof(pl);
				sub.betCode = 3;
				sub.checked = false;
				sub.calcTip(ji->hand);
				ji->subjects.push_back(sub);
			}
			auto midNode = FindElementByClassAttr(child, "pbg");
			if (midNode != nullptr) {
				CStringA pl = GetElementText(midNode->FirstChildElement("em"));
				JCMatchItem::Subject sub;
				sub.tid = 1;
				sub.odds = atof(pl);
				sub.betCode = 1;
				sub.checked = false;
				sub.calcTip(ji->hand);
				ji->subjects.push_back(sub);
			}
			auto awayNode = FindElementByClassAttr(child, "fbg");
			if (awayNode != nullptr) {
				CStringA pl = GetElementText(FindElementByClassAttr(awayNode, "pltxt"));
				JCMatchItem::Subject sub;
				sub.tid = 1;
				sub.odds = atof(pl);
				sub.betCode = 0;
				sub.checked = false;
				sub.calcTip(ji->hand);
				ji->subjects.push_back(sub);
			}
			for (size_t i = 0; i <= 7; i++) {
				JCMatchItem::Subject sub;
				sub.tid = 2;
				sub.odds = 1.0f;
				sub.betCode = i;
				sub.checked = false;
				sub.calcTip(ji->hand);
				ji->subjects.push_back(sub);
			}
			m_order_items.insert(std::make_pair(ji->orderid, ji));
		}
		
		for (const auto& item : m_order_items) {
			/*
			if (item.second->hand == 0) {
				for (int i = (int)item.second->subjects.size() - 1; i >= 0;i--) {
					if (item.second->subjects[i].tid == 1) {
						item.second->subjects.erase(item.second->subjects.begin() + i);
					}
				}
			}
			*/
			CStringA url;
			url.Format("https://m.okooo.com/match/change.php?mid=%s&pid=24&Type=Odds&c=1", item.second->orderid.c_str());
			auto req = CreateGetRequest(std::string((LPCSTR)url), BEIDANWDL_REQ_PREFIX);
			req->cmd = item.second->orderid;
			m_pending_request++;
			httpMgr_->DoHttpCommandRequest(req);
		}

		if (!m_order_items.empty()) {
			//
			CStringA url = "https://www.okooo.com/danchang/danshuang/";
			auto req = CreateGetRequest(std::string((LPCSTR)url), BEIDANDS_REQ_PREFIX);
			req->request_headers.insert(std::make_pair("Referer", "https://www.okooo.com/danchang"));
			m_pending_request++;
			httpMgr_->DoHttpCommandRequest(req);
		}
		/*
		if (!m_order_items.empty()) {
			//
			CStringA url = "https://www.okooo.com/danchang/jinqiu/";
			auto req = CreateGetRequest(std::string((LPCSTR)url), BEIDANJQ_REQ_PREFIX);
			req->request_headers.insert(std::make_pair("Referer", "https://www.okooo.com/danchang"));
			m_pending_request++;
			httpMgr_->DoHttpCommandRequest(req);
		}
		*/
	}
	if (m_order_items.empty()) {
		MessageBox("获取对阵列表失败 5, 没有比赛场次！", "错误", MB_ICONERROR | MB_OK);
	}
}

void BeiDanDialog::OnBeiDanWDLReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	m_pending_request--;
	CStringATL progress;
	progress.Format("%d/%d", m_order_items.size() - m_pending_request, m_order_items.size());
	m_stProgress.SetWindowText(progress);
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		do {
			std::string raw_response;
			if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
				CZlibStream zlib;
				zlib.DecompressGZip(response->response_content, raw_response);
			} else {
				raw_response = response->response_content;
			}
			int acp_code = CP_ACP;
			if (raw_response.find("UTF-8") != std::string::npos 
					|| raw_response.find("utf-8") != std::string::npos) {
				acp_code = CP_UTF8;
			}
			CStringA temp = CW2A(CA2W(raw_response.c_str(), acp_code).m_psz).m_psz;
			CStringA section_begin = "<table width=";
			CStringA section_end = "</table>";

			int nFindBegin = temp.Find(section_begin);
			if (nFindBegin == -1) {
				break;
			}
			int nFindEnd = temp.Find(section_end, nFindBegin);
			CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
			std::string date;
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLElement* tempElement = nullptr;
			if (doc.Parse(xmlText) != tinyxml2::XML_SUCCESS) {
				break;
			}
			tinyxml2::XMLElement* child = doc.FirstChildElement();
			if (child == nullptr) {
				break;
			}
			child = child->FirstChildElement("tr");
			if (child == nullptr) {
				break;
			}
			child = child->FirstChildElement("td");
			if (child == nullptr) {
				break;
			}
			auto& item = m_order_items.find(request->cmd);
			if (item == m_order_items.end()) {
				break;
			}
			child = child->FirstChildElement("span");
			int index = 0;
			int minodds_code = 3;
			double minodds = 10000;
			std::set<double> odds_kind;
			while (child != nullptr) {
				CStringA pl = GetElementText(child);
				JCMatchItem::Subject sub;
				sub.tid = 6;
				sub.odds = atof(pl);
				sub.betCode = 3;
				if(index == 1) sub.betCode = 1;
				else if(index == 2) sub.betCode = 0;
				sub.checked = false;
				sub.calcTip(item->second->hand);
				item->second->subjects.push_back(sub);
				index++;
				child = child->NextSiblingElement("span");
				if (sub.odds < minodds) {
					minodds = sub.odds;
					minodds_code = sub.betCode;
				}
				odds_kind.insert(sub.odds);
			}
			item->second->odds_hand = -1;
			if (minodds_code == 0) {
				item->second->odds_hand = 1;
			}
			int odds_kind_count = odds_kind.size();
			if (odds_kind_count < 3) {
				int add_code = 0, minus_code = 1;
				if (minodds_code == 0) {
					add_code = 3;
					item->second->hand = 1;
				} else if (minodds_code == 1) {
					minus_code = 3;
				}
				for (auto& sub : item->second->subjects) {
					if (sub.betCode == add_code) {
						sub.odds += 0.01;
					}
					if (sub.betCode == minus_code) {
						sub.odds -= 0.01;
					}
				}
			}
		} while (false);
	}

	if (m_pending_request <= 0) {
		std::multimap<std::string, std::shared_ptr<JCMatchItem>> items;
		if (!m_order_items.empty()) {
			for (auto& iter : m_order_items) {
				InsertItemToDB(*iter.second);
				items.insert(std::make_pair(iter.second->id, iter.second));
			}
			m_JCMatchItems.swap(items);
		}
		ReloadMatchListData();
		m_buLogin.EnableWindow(FALSE);
		m_buLogoff.EnableWindow(TRUE);
		m_stProgress.SetWindowText("");
	}
}


void BeiDanDialog::OnBeiDanDSReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	m_pending_request--;
	CStringATL progress;
	progress.Format("%d/%d", m_order_items.size() - m_pending_request, m_order_items.size());
	m_stProgress.SetWindowText(progress);

	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		do {
			std::string raw_response;
			if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
				CZlibStream zlib;
				zlib.DecompressGZip(response->response_content, raw_response);
			}
			else {
				raw_response = response->response_content;
			}
			int acp_code = CP_ACP;
			if (raw_response.find("UTF-8") != std::string::npos
				|| raw_response.find("utf-8") != std::string::npos) {
				acp_code = CP_UTF8;
			}
			CStringA temp = CW2A(CA2W(raw_response.c_str(), acp_code).m_psz).m_psz;
			CStringA section_begin = "<tr class=\"alltrObj";
			CStringA section_end = "</tr>";
			int nFindBegin = 0;
			while((nFindBegin = temp.Find(section_begin, nFindBegin)) != -1) {
				int nFindEnd = temp.Find(section_end, nFindBegin);
				CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
				nFindBegin = nFindEnd;
				adjustXmlText(xmlText);
				std::string date;
				tinyxml2::XMLDocument doc;
				tinyxml2::XMLElement* tempElement = nullptr;
				if (doc.Parse(xmlText) != tinyxml2::XML_SUCCESS) {
					break;
				}
				tinyxml2::XMLElement* child = doc.FirstChildElement();
				if (child == nullptr) {
					break;
				}
				auto node = FindElementByClassAttr(child, "xh");
				if (node == nullptr) {
					//MessageBox("获取对阵列表失败 4！", "错误", MB_ICONERROR | MB_OK);
					break;
				}
				CStringA xuhao = adjustXuHaoText(GetElementText(node->FirstChildElement()));
				std::shared_ptr<JCMatchItem> item;
				for (auto& c : m_order_items) {
					if (xuhao == c.second->id.c_str()) {
						item = c.second;
						break;
					}
				}
				if (item.get() == nullptr) {
					continue;
				}
				node = FindElementByClassAttr(child, "td4 ztbox");
				std::map<CStringA, double> codes;
				if (node != nullptr) {
					auto son = node->FirstChildElement("a");
					while (son != nullptr) {
						auto son_child = son->FirstChildElement("em");
						while (son_child != nullptr) {
							CStringA code = GetElementText(son_child);
							if (!code.IsEmpty()) {
								CStringA name = GetElementAttrValue(son, "name");
								codes[name] = atof(code);
								break;
							}
							son_child = son_child->NextSiblingElement("em");
						}
						son = son->NextSiblingElement("a");
					}
				}
				if (codes.find("c1") == codes.cend() || codes.find("c3") == codes.cend()
					|| codes.find("c5") == codes.cend() || codes.find("c7") == codes.cend()) {
					break;
				}
				for (const auto& c : codes) {
					if (c.first == "c7") {
						JCMatchItem::Subject sub;
						sub.tid = 7;
						sub.odds = c.second;
						sub.betCode = 2;
						sub.checked = false;
						sub.calcTip(item->hand);
						item->subjects.push_back(sub);
					}
					else if (c.first == "c5") {
						JCMatchItem::Subject sub;
						sub.tid = 7;
						sub.odds = c.second;
						sub.betCode = 1;
						sub.checked = false;
						sub.calcTip(item->hand);
						item->subjects.push_back(sub);
					}
					else if (c.first == "c3") {
						JCMatchItem::Subject sub;
						sub.tid = 7;
						sub.odds = c.second;
						sub.betCode = 4;
						sub.checked = false;
						sub.calcTip(item->hand);
						item->subjects.push_back(sub);
					}
					if (c.first == "c1") {
						JCMatchItem::Subject sub;
						sub.tid = 7;
						sub.odds = c.second;
						sub.betCode = 3;
						sub.checked = false;
						sub.calcTip(item->hand);
						item->subjects.push_back(sub);
					}
				}
			}
		} while (false);
	}

	if (m_pending_request <= 0) {
		std::multimap<std::string, std::shared_ptr<JCMatchItem>> items;
		if (!m_order_items.empty()) {
			for (auto& iter : m_order_items) {
				InsertItemToDB(*iter.second);
				items.insert(std::make_pair(iter.second->id, iter.second));
			}
			m_JCMatchItems.swap(items);
		}
		ReloadMatchListData();
		m_buLogin.EnableWindow(FALSE);
		m_buLogoff.EnableWindow(TRUE);
		m_stProgress.SetWindowText("");
	}
}



void BeiDanDialog::OnBeiDanJQReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	m_pending_request--;
	CStringATL progress;
	progress.Format("%d/%d", m_order_items.size() - m_pending_request, m_order_items.size());
	m_stProgress.SetWindowText(progress);

	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		do {
			std::string raw_response;
			if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
				CZlibStream zlib;
				zlib.DecompressGZip(response->response_content, raw_response);
			}
			else {
				raw_response = response->response_content;
			}
			int acp_code = CP_ACP;
			if (raw_response.find("UTF-8") != std::string::npos
				|| raw_response.find("utf-8") != std::string::npos) {
				acp_code = CP_UTF8;
			}
			CStringA temp = CW2A(CA2W(raw_response.c_str(), acp_code).m_psz).m_psz;
			CStringA section_begin = "<tr class=\"alltrObj";
			CStringA section_end = "</tr>";
			int nFindBegin = 0;
			while ((nFindBegin = temp.Find(section_begin, nFindBegin)) != -1) {
				int nFindEnd = temp.Find(section_end, nFindBegin);
				CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
				nFindBegin = nFindEnd;
				adjustXmlText(xmlText);
				std::string date;
				tinyxml2::XMLDocument doc;
				tinyxml2::XMLElement* tempElement = nullptr;
				if (doc.Parse(xmlText) != tinyxml2::XML_SUCCESS) {
					break;
				}
				tinyxml2::XMLElement* child = doc.FirstChildElement();
				if (child == nullptr) {
					break;
				}
				auto node = FindElementByClassAttr(child, "xh");
				if (node == nullptr) {
					//MessageBox("获取对阵列表失败 4！", "错误", MB_ICONERROR | MB_OK);
					break;
				}
				CStringA xuhao = adjustXuHaoText(GetElementText(node->FirstChildElement()));
				std::shared_ptr<JCMatchItem> item;
				for (auto& c : m_order_items) {
					if (xuhao == c.second->id.c_str()) {
						item = c.second;
						break;
					}
				}
				if (item.get() == nullptr) {
					continue;
				}
				node = FindElementByClassAttr(child, "td4 ztbox");
				std::map<CStringA, double> codes;
				if (node != nullptr) {
					auto son = node->FirstChildElement("a");
					while (son != nullptr) {
						auto son_child = son->FirstChildElement("em");
						while (son_child != nullptr) {
							CStringA code = GetElementText(son_child);
							if (!code.IsEmpty()) {
								CStringA name = GetElementAttrValue(son, "name");
								codes[name] = atof(code);
								break;
							}
							son_child = son_child->NextSiblingElement("em");
						}
						son = son->NextSiblingElement("a");
					}
				}
				if (codes.find("c1") == codes.cend() || codes.find("c3") == codes.cend()
					|| codes.find("c5") == codes.cend() || codes.find("c7") == codes.cend()) {
					break;
				}
				for (const auto& c : codes) {
					if (c.first == "c7") {
						JCMatchItem::Subject sub;
						sub.tid = 7;
						sub.odds = c.second;
						sub.betCode = 2;
						sub.checked = false;
						sub.calcTip(item->hand);
						item->subjects.push_back(sub);
					}
					else if (c.first == "c5") {
						JCMatchItem::Subject sub;
						sub.tid = 7;
						sub.odds = c.second;
						sub.betCode = 1;
						sub.checked = false;
						sub.calcTip(item->hand);
						item->subjects.push_back(sub);
					}
					else if (c.first == "c3") {
						JCMatchItem::Subject sub;
						sub.tid = 7;
						sub.odds = c.second;
						sub.betCode = 4;
						sub.checked = false;
						sub.calcTip(item->hand);
						item->subjects.push_back(sub);
					}
					if (c.first == "c1") {
						JCMatchItem::Subject sub;
						sub.tid = 7;
						sub.odds = c.second;
						sub.betCode = 3;
						sub.checked = false;
						sub.calcTip(item->hand);
						item->subjects.push_back(sub);
					}
				}
			}
		} while (false);
	}

	if (m_pending_request <= 0) {
		std::multimap<std::string, std::shared_ptr<JCMatchItem>> items;
		if (!m_order_items.empty()) {
			for (auto& iter : m_order_items) {
				InsertItemToDB(*iter.second);
				items.insert(std::make_pair(iter.second->id, iter.second));
			}
			m_JCMatchItems.swap(items);
		}
		ReloadMatchListData();
		m_buLogin.EnableWindow(FALSE);
		m_buLogoff.EnableWindow(TRUE);
		m_stProgress.SetWindowText("");
	}
}



int BeiDanDialog::doBiFen() {
	/*
	CStringATL beginDay, endDay, beginWeekDay;
	Global::getBiFenDateInfo(beginDay, endDay, beginWeekDay);
	CStringATL url;
	url.Format("http://www.okooo.com/jingcai/kaijiang/?LotteryType=SportteryWDL&StartDate=%s&EndDate=%s",
		beginDay, endDay);
	std::string request_id = BIFEN_REQ_PREFIX;
	CHttpRequestPtr request = CreateGetRequest((LPCSTR)url, request_id);
	httpMgr_->DoHttpCommandRequest(request);
	*/
	return 0;
}

void BeiDanDialog::OnBiFenReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	const CStringATL strRowBegin = "<tr align=\"center\" class=\"WhiteBg BlackWords trClass\">\r\n";
	const CStringATL strRowBegin1 = "<tr align=\"center\" class=\"ContentLight BlackWords trClass\">\r\n";

	const CStringATL strRowEnd = "</tr>";
	const CStringATL strColBegin = "<td>";
	const CStringATL strColBegin1 = "<td class=\"border2\">";
	const CStringATL strColBegin2 = "<td class=\"noborder\">";
	const CStringATL strColEnd = "</td>";
	if (response->httperror == talk_base::HE_NONE) {
		std::map<CStringATL, CStringATL> mapBiFen;
		CStringATL beginDay, endDay, beginWeekDay;
		Global::getBiFenDateInfo(beginDay, endDay, beginWeekDay);
		CZlibStream zlib;
		std::string raw_response;
		zlib.DecompressGZip(response->response_content, raw_response);
		CStringATL html = raw_response.c_str();
		int nFindRow = html.Find(strRowBegin);
		int nFindRow1 = html.Find(strRowBegin1);
		if (nFindRow1 >= 0 && nFindRow > nFindRow1) {
			nFindRow = nFindRow1;
		}
		while (nFindRow >= 0) {
			int nFindRowEnd = html.Find(strRowEnd, nFindRow + 1);
			if (nFindRowEnd == -1) {
				break;
			}
			int nRowStart = nFindRow + strRowBegin.GetLength();
			if (nFindRow == nFindRow1) {
				nRowStart = nFindRow + strRowBegin1.GetLength();
			}
			CStringATL strRow = html.Mid(nRowStart, nFindRowEnd - nRowStart);
			CStlStrArray arrCols;
			Global::DepartString((LPCSTR)strRow, "\r\n", arrCols);
			CStringATL id, data;
			for (int i = 0; i < arrCols.size(); i++) {
				auto& col = arrCols[i];
				Global::TrimBlank(col);
				if (i == 0) {
					id = col.c_str();
					id.Replace(strColBegin, "");
					id.Replace(strColBegin2, "");
					id.Replace(strColEnd, "");
					if (id.Find(beginWeekDay) != 0) {
						id.Empty();
						break;
					}
					id.Replace(beginWeekDay, beginDay);
					id.Replace("-", "");

				}
				else if (i == 6 || i == 7) {
					CStringATL temp;
					temp = col.c_str();
					temp.Replace(strColBegin, "");
					temp.Replace(strColBegin1, "");
					temp.Replace(strColEnd, "");
					temp.Replace("-", ":");
					if (temp.GetLength() <= 1) {
						data.Empty();
						break;
					}
					if (data.IsEmpty()) {
						data = temp;
					}
					else {
						data = data + "|" + temp;
					}
				}
			}
			if (!id.IsEmpty() && !data.IsEmpty()) {
				mapBiFen[id] = data;
			}
			nFindRow = html.Find(strRowBegin, nFindRowEnd + 1);
			nFindRow1 = html.Find(strRowBegin1, nFindRowEnd + 1);
			if (nFindRow1 >= 0 && nFindRow > nFindRow1) {
				nFindRow = nFindRow1;
			}
		}
		for (const auto& bf : mapBiFen) {
			UpdateItemResultToDB((LPCSTR)bf.first, (LPCSTR)bf.second);
		}
	}
	return;
}












//http://www.okooo.com/jingcai/kaijiang/?LotteryType=SportteryWDL&StartDate=2018-03-05&EndDate=2018-03-06



