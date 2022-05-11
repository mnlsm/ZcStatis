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
	//ptr->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	ptr->request_headers.insert(std::make_pair("Connection", "Keep-Alive"));
	ptr->request_headers.insert(std::make_pair("Accept-Encoding", "gzip"));
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
	ptr->request_headers.insert(std::make_pair("Connection", "Keep-Alive"));
	ptr->request_headers.insert(std::make_pair("Accept-Encoding", "gzip"));
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
		CStringA temp = CT2A(CA2T(raw_response.c_str(), CP_ACP).m_psz).m_psz;

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
			m_order_items.insert(std::make_pair(ji->orderid, ji));
		}
		
		for (const auto& item : m_order_items) {
			CStringA url;
			url.Format("https://m.okooo.com/match/change.php?mid=%s&pid=24&Type=Odds&c=1" ,item.second->orderid.c_str());
			auto req = CreateGetRequest(std::string((LPCSTR)url), BEIDANWDL_REQ_PREFIX);
			req->cmd = item.second->orderid;
			m_pending_request++;
			httpMgr_->DoHttpCommandRequest(req);
		}
		
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
			CStringA temp = CT2A(CA2T(raw_response.c_str(), CP_ACP).m_psz).m_psz;
			CStringA section_begin = "<table width=";
			CStringA section_end = "</table>";

			int nFindBegin = temp.Find(section_begin);
			if (nFindBegin == -1) {
				return;
			}
			int nFindEnd = temp.Find(section_end, nFindBegin);
			CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
			std::string date;
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLElement* tempElement = nullptr;
			if (doc.Parse(xmlText) != tinyxml2::XML_SUCCESS) {
				return;
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
				items.insert(std::make_pair(iter.second->start_time, iter.second));
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



