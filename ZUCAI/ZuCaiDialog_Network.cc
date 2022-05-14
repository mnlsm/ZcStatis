#include "stdafx.h"
#include "ZuCaiDialog.h"
#include "Global.h"
#include "ZlibStream.h"
#include "MiscHelper.h"


BOOL ZuCaiDialog::OnIdle() {
	int async_count = GetAsyncFuncCount();
	if (async_count > 0) {
		PostMessage(ZuCaiDialog::WM_ASYNC_DISPATCH, 0, 0);
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

LRESULT ZuCaiDialog::OnAsyncDispatch(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DispathOneAsyncFunc();
	return 1L;
}

bool ZuCaiDialog::AddOneAsyncFunc(talk_base::IAsyncFuncCall* pAsyncFunc) {
	BOOL bRet = CAsyncFuncDispatcher::AddOneAsyncFunc(pAsyncFunc);
	if (bRet) {
		bRet = PostMessage(ZuCaiDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	return (bRet == TRUE);
}

void ZuCaiDialog::OnHttpReturnGlobal(const CHttpRequestPtr& request,
	const CHttpResponseDataPtr& response) {
	sInst.AddOneAsyncFunc(talk_base::BindAsyncFunc(&ZuCaiDialog::OnHttpReturn, &sInst, request, response));
}

CHttpRequestPtr ZuCaiDialog::CreatePostRequest(const std::string& url, const std::string& idprefix,
	const std::string& data) {
	std::string request_id = CHttpClientMgr::generateRequestId();
	request_id = idprefix + "_" + request_id;
	CHttpRequestPtr ptr(new (std::nothrow) THttpRequestData());
	//ptr->agent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36";
	ptr->agent = "Dalvik/2.1.0 (Linux; U; Android 6.0; M5s Build/MRA58K)";
	//ptr->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	ptr->request_headers.insert(std::make_pair("Connection", "Keep-Alive"));
	ptr->request_headers.insert(std::make_pair("Accept-Encoding", "gzip"));
	ptr->request_url = url;
	ptr->request_id = request_id;
	ptr->request_type = HTTP_REQUEST_POST;
	ptr->request_post_data = data;
	ptr->response_type = RESPONSEMEMORY;
	ptr->request_time = 1000 * 60;
	ptr->callback = &ZuCaiDialog::OnHttpReturnGlobal;
	//httpMgr_->DoHttpCommandRequest(ptr);
	return ptr;
}

CHttpRequestPtr ZuCaiDialog::CreateGetRequest(const std::string& url, const std::string& idprefix) {
	std::string request_id = CHttpClientMgr::generateRequestId();
	request_id = idprefix + "_" + request_id;
	CHttpRequestPtr ptr(new (std::nothrow) THttpRequestData());
	//ptr->agent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36";
	ptr->agent = "Dalvik/2.1.0 (Linux; U; Android 6.0; M5s Build/MRA58K)";
	ptr->request_headers.insert(std::make_pair("Connection", "Keep-Alive"));
	ptr->request_headers.insert(std::make_pair("Accept-Encoding", "gzip"));
	ptr->request_url = url;
	ptr->request_id = request_id;
	ptr->request_type = HTTP_REQUEST_GET;
	//ptr->request_post_data = query->GetCloudPathsRequestData();
	ptr->response_type = RESPONSEMEMORY;
	ptr->request_time = 1000 * 60;
	ptr->callback = &ZuCaiDialog::OnHttpReturnGlobal;
	return ptr;
	//httpMgr_->DoHttpCommandRequest(ptr);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const char* LOGIN_REQ_PREFIX = "login_req_prefix";
static const char* JCMATCHLIST_REQ_PREFIX = "jcmatchlist_req_prefix";
static const char* ZCPL_REQ_PREFIX = "zcpl_req_prefix";


void ZuCaiDialog::OnHttpReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (request->request_id.find(LOGIN_REQ_PREFIX) == 0) {
		OnLoginReturn(request, response);
	} else if (request->request_id.find(JCMATCHLIST_REQ_PREFIX) == 0) {
		OnJcMatchListReturn(request, response);
	} else if (request->request_id.find(ZCPL_REQ_PREFIX) == 0) {
		OnZcPlReturn(request, response);
	}
}

int ZuCaiDialog::doLogin() {
	if (TRUE) {
		std::string url = "https://m.okooo.com/weixin/jing/d.php";
		CHttpRequestPtr request = CreateGetRequest(url, LOGIN_REQ_PREFIX);
		httpMgr_->DoHttpCommandRequest(request);
	}
	m_buLogin.EnableWindow(FALSE);
	m_buLogoff.EnableWindow(FALSE);
	return 0;
}


void ZuCaiDialog::OnLoginReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		doJcMatchList();
	} else {
		MessageBoxA("登录失败，网络错误!", "提示", MB_OK);
		m_buLogin.EnableWindow(TRUE);
		m_buLogoff.EnableWindow(FALSE);
	}
}

int ZuCaiDialog::doLogOff() {
	m_buLogin.EnableWindow(TRUE);
	m_buLogoff.EnableWindow(FALSE);
	return 0l;
}


int ZuCaiDialog::doJcMatchList() {
	std::string url = "https://www.okooo.com/zucai/ren9/";
	CHttpRequestPtr request = CreateGetRequest(url, JCMATCHLIST_REQ_PREFIX);
	request->request_headers.insert(std::make_pair("cookie", "LastUrl=; __utmz=56961525.1650162259.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); FirstURL=kj.okooo.com / ; FirstOKURL=https % 3A//www.okooo.com/danchang/kaijiang/; First_Source=kj.okooo.com; _ga=GA1.2.1953148046.1650162259; PHPSESSID=f10e97ef44b47fd188dce251c03e1d4816228600; __utma=56961525.1953148046.1650162259.1651827501.1652296390.3; __utmc=56961525; pm=; LStatus=N; LoginStr=%7B%22welcome%22%3A%22%u60A8%u597D%uFF0C%u6B22%u8FCE%u60A8%22%2C%22login%22%3A%22%u767B%u5F55%22%2C%22register%22%3A%22%u6CE8%u518C%22%2C%22TrustLoginArr%22%3A%7B%22alipay%22%3A%7B%22LoginCn%22%3A%22%u652F%u4ED8%u5B9D%22%7D%2C%22tenpay%22%3A%7B%22LoginCn%22%3A%22%u8D22%u4ED8%u901A%22%7D%2C%22weibo%22%3A%7B%22LoginCn%22%3A%22%u65B0%u6D6A%u5FAE%u535A%22%7D%2C%22renren%22%3A%7B%22LoginCn%22%3A%22%u4EBA%u4EBA%u7F51%22%7D%2C%22baidu%22%3A%7B%22LoginCn%22%3A%22%u767E%u5EA6%22%7D%2C%22snda%22%3A%7B%22LoginCn%22%3A%22%u76DB%u5927%u767B%u5F55%22%7D%7D%2C%22userlevel%22%3A%22%22%2C%22flog%22%3A%22hidden%22%2C%22UserInfo%22%3A%22%22%2C%22loginSession%22%3A%22___GlobalSession%22%7D; acw_tc=2f624a7c16522984571547396e7b12d938bc88db7e47bf16f3047017ed7a19; acw_sc__v3=627c15baccdabfeed9a6a907689e5c1f59c6a40c; Hm_lvt_213d524a1d07274f17dfa17b79db318f=1650162260,1651827501,1652299222; Hm_lpvt_213d524a1d07274f17dfa17b79db318f=1652299222; __utmb=56961525.18.8.1652299221829"));
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
		return "00";
	}
	else if (len == 1) {
		return CStringA("0") + temp;
	}
	return temp;
}

void ZuCaiDialog::OnJcMatchListReturn(const CHttpRequestPtr& request,
	const CHttpResponseDataPtr& response) {
	m_pending_request = 0;
	m_order_items.clear();

	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		std::string raw_response;
		if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
			CZlibStream zlib;
			zlib.DecompressGZip(response->response_content, raw_response);
		}
		else {
			raw_response = response->response_content;
		}
		CStringA temp = CT2A(CA2T(raw_response.c_str(), CP_UTF8).m_psz).m_psz;

		int nFindBegin = -1, nFindEnd = -1;
		CStringA section_begin = "<tr id=\"tr";
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
			auto node = FindElementByClassAttr(child, "xh td1");
			if (node == nullptr) {
				MessageBox("获取对阵列表失败 4！", "错误", MB_ICONERROR | MB_OK);
				break;
			}
			CStringA xuhao = GetElementText(node);
			CStringA match_category = GetElementText(FindElementByClassAttr(child, "ls  jsLeagueName"));
			CStringA start_time = GetElementAttrValue(FindElementByClassAttr(
				child, "switchtime timetd feedbackObj td3"), "title");
			start_time.Replace("比赛时间:", ""); start_time.Trim();

			CStringA home = GetElementText(FindElementByClassAttr(
				child, "homenameobj homename"));
			CStringA away = GetElementText(FindElementByClassAttr(
				child, "awaynameobj awayname"));
			CStringA href = GetElementAttrValue(FindElementByClassAttr(
				child, "zc-link"), "href");
			href.Replace("odds", "history");
			href.Replace("node", "history");
			CStringA orderid = href;
			orderid.Replace("/soccer/match/", "");
			orderid.Replace("/history/", "");

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
				sub.tid = 6;
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
				sub.tid = 6;
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
				sub.tid = 6;
				sub.odds = atof(pl);
				sub.betCode = 0;
				sub.checked = false;
				sub.calcTip(ji->hand);
				ji->subjects.push_back(sub);
			}
			m_order_items.insert(std::make_pair(ji->orderid, ji));
		}
	}
	if (m_order_items.empty()) {
		MessageBox("获取对阵列表失败 5, 没有比赛场次！", "错误", MB_ICONERROR | MB_OK);
		m_buLogin.EnableWindow(FALSE);
		m_buLogoff.EnableWindow(TRUE);
	} else {
		for (const auto& item : m_order_items) {
			CStringA url;
			url.Format("https://www.okooo.com/soccer/match/%s/odds/stat/14/?type=start&range=all", item.second->orderid.c_str());
			auto req = CreateGetRequest(std::string((LPCSTR)url), ZCPL_REQ_PREFIX);
			req->cmd = item.second->orderid;
			req->request_headers.insert(std::make_pair("cookie", "LastUrl=; __utmz=56961525.1650162259.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); FirstURL=kj.okooo.com / ; FirstOKURL=https % 3A//www.okooo.com/danchang/kaijiang/; First_Source=kj.okooo.com; _ga=GA1.2.1953148046.1650162259; PHPSESSID=f10e97ef44b47fd188dce251c03e1d4816228600; __utma=56961525.1953148046.1650162259.1651827501.1652296390.3; __utmc=56961525; pm=; LStatus=N; LoginStr=%7B%22welcome%22%3A%22%u60A8%u597D%uFF0C%u6B22%u8FCE%u60A8%22%2C%22login%22%3A%22%u767B%u5F55%22%2C%22register%22%3A%22%u6CE8%u518C%22%2C%22TrustLoginArr%22%3A%7B%22alipay%22%3A%7B%22LoginCn%22%3A%22%u652F%u4ED8%u5B9D%22%7D%2C%22tenpay%22%3A%7B%22LoginCn%22%3A%22%u8D22%u4ED8%u901A%22%7D%2C%22weibo%22%3A%7B%22LoginCn%22%3A%22%u65B0%u6D6A%u5FAE%u535A%22%7D%2C%22renren%22%3A%7B%22LoginCn%22%3A%22%u4EBA%u4EBA%u7F51%22%7D%2C%22baidu%22%3A%7B%22LoginCn%22%3A%22%u767E%u5EA6%22%7D%2C%22snda%22%3A%7B%22LoginCn%22%3A%22%u76DB%u5927%u767B%u5F55%22%7D%7D%2C%22userlevel%22%3A%22%22%2C%22flog%22%3A%22hidden%22%2C%22UserInfo%22%3A%22%22%2C%22loginSession%22%3A%22___GlobalSession%22%7D; acw_tc=2f624a7c16522984571547396e7b12d938bc88db7e47bf16f3047017ed7a19; acw_sc__v3=627c15baccdabfeed9a6a907689e5c1f59c6a40c; Hm_lvt_213d524a1d07274f17dfa17b79db318f=1650162260,1651827501,1652299222; Hm_lpvt_213d524a1d07274f17dfa17b79db318f=1652299222; __utmb=56961525.18.8.1652299221829"));
			m_pending_request++;
			httpMgr_->DoHttpCommandRequest(req);
		}
	}
}

void ZuCaiDialog::OnZcPlReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	m_pending_request--;
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		do {
			auto& item = m_order_items.find(request->cmd);
			if (item == m_order_items.end()) {
				break;
			}
			std::string raw_response;
			if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
				CZlibStream zlib;
				zlib.DecompressGZip(response->response_content, raw_response);
			}
			else {
				raw_response = response->response_content;
			}
			CStringA temp = CT2A(CA2T(raw_response.c_str(), CP_ACP).m_psz).m_psz;
			int nFirstFind = temp.Find("初指");
			if (nFirstFind == -1) {
				break;
			}
			nFirstFind = nFirstFind - 100;
			CStringA section_begin = "<tr";
			CStringA section_end = "</tr>";
			int nFindBegin = temp.Find(section_begin, nFirstFind);
			if (nFindBegin == -1) {
				break;
			}
			int nFindEnd = temp.Find(section_end, nFindBegin);
			CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
			xmlText.Replace("</td></td>", "</td>");
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
			child = child->FirstChildElement("td");
			int index = -1;
			while (child != nullptr) {
				if (index == 0) {
					auto sub = item->second->get_subject(6, 3);
					if (sub != nullptr) {
						sub->odds = atof(GetElementText(child));
					}
				} else if (index == 1) {
					auto sub = item->second->get_subject(6, 1);
					if (sub != nullptr) {
						sub->odds = atof(GetElementText(child));
					}
				} else if (index == 2) {
					auto sub = item->second->get_subject(6, 0);
					if (sub != nullptr) {
						sub->odds = atof(GetElementText(child));
					}
				} else if (index > 2) {
					break;
				}
				child = child->NextSiblingElement("td");
				index++;
			}

		} while (false);
	}

	if (m_pending_request <= 0) {
		std::multimap<std::string, std::shared_ptr<JCMatchItem>> items;
		if (!m_order_items.empty()) {
			for (auto& iter : m_order_items) {
				JCMatchItem item;
				if (GetItemFromDB(*iter.second, item)) {
					*iter.second = item;
				}
				else {
					InsertItemToDB(*iter.second);
				}
				items.insert(std::make_pair(iter.second->id, iter.second));
			}
			m_JCMatchItems.swap(items);
		}
		ReloadMatchListData();
		m_buLogin.EnableWindow(FALSE);
		m_buLogoff.EnableWindow(TRUE);
	}
}


