#include "stdafx.h"
#include "OkoooDialog.h"
#include "Global.h"
#include "ZlibStream.h"
#include "MiscHelper.h"


BOOL OkoooDialog::OnIdle() {
	int async_count = GetAsyncFuncCount();
	if (async_count > 0) {
		PostMessage(OkoooDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	return FALSE;
}

LRESULT OkoooDialog::OnAsyncDispatch(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DispathOneAsyncFunc();
	return 1L;
}

bool OkoooDialog::AddOneAsyncFunc(talk_base::IAsyncFuncCall* pAsyncFunc) {
	BOOL bRet = CAsyncFuncDispatcher::AddOneAsyncFunc(pAsyncFunc);
	if (bRet) {
		bRet = PostMessage(OkoooDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	return (bRet == TRUE);
}

void OkoooDialog::OnHttpReturnGlobal(const CHttpRequestPtr& request,
	const CHttpResponseDataPtr& response) {
	sInst.AddOneAsyncFunc(talk_base::BindAsyncFunc(&OkoooDialog::OnHttpReturn, &sInst, request, response));
}

CHttpRequestPtr OkoooDialog::CreatePostRequest(const std::string& url, const std::string& idprefix,
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
	ptr->callback = &OkoooDialog::OnHttpReturnGlobal;
	//httpMgr_->DoHttpCommandRequest(ptr);
	return ptr;
}

CHttpRequestPtr OkoooDialog::CreateGetRequest(const std::string& url, const std::string& idprefix) {
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
	ptr->callback = &OkoooDialog::OnHttpReturnGlobal;
	return ptr;
	//httpMgr_->DoHttpCommandRequest(ptr);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
static const std::string LOGIN_REQ_PREFIX = "login_req_prefix";
static const std::string LOGOFF_REQ_PREFIX = "logoff_req_prefix";
static const std::string INFO_REQ_PREFIX = "info_req_prefix";
static const std::string RCTOKEN_REQ_PREFIX = "rctoken_req_prefix";
static const std::string FRIENDLIST_REQ_PREFIX = "friendlist_req_prefix";
static const std::string LOTTERYCATEGORIES_REQ_PREFIX = "lotterycategories_req_prefix";
static const std::string JCMATCHLIST_REQ_PREFIX = "jcmatchlist_req_prefix";
*/

#define LOGIN_REQ_PREFIX  "login_req_prefix"
#define  LOGOFF_REQ_PREFIX  "logoff_req_prefix"
#define  INFO_REQ_PREFIX  "info_req_prefix"
#define  RCTOKEN_REQ_PREFIX  "rctoken_req_prefix"
#define  FRIENDLIST_REQ_PREFIX  "friendlist_req_prefix"
#define  LOTTERYCATEGORIES_REQ_PREFIX  "lotterycategories_req_prefix"
#define  JCMATCHLIST_REQ_PREFIX "jcmatchlist_req_prefix"
#define  HEMAI_REQ_PREFIX "hemai_req_prefix"
#define  HEMAI_REQ_PREFIX_FINISH "hemai_req_prefix_finish"
#define  BIFEN_REQ_PREFIX "bifen_req_prefix"

struct ResHeader {
	ResHeader() {
		status = -1;
		timestamp = 0;
		message = "unknown";
	}
	void parse(const Json::Value& rootValue) {
		std::string str_error;
		GetStringFromJsonObject(rootValue, "error", &str_error);
		if (!str_error.empty()) {
			str_error = Global::fromUTF8(str_error);
		}
		GetIntFromJsonObject(rootValue, "status", &status);
		GetStringFromJsonObject(rootValue, "message", &message);
		if (!message.empty()) {
			message = Global::fromUTF8(message);
		}
		GetInt64FromJsonObject(rootValue, "timestamp", &timestamp);
	}
	int status;
	std::string message;
	__int64 timestamp;
};

void OkoooDialog::OnHttpReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (request->request_id.find(LOGIN_REQ_PREFIX) == 0) {
		OnLoginReturn(request, response);
	}

	else if (request->request_id.find(JCMATCHLIST_REQ_PREFIX) == 0) {
		OnJcMatchListReturn(request, response);
	}

	else if (request->request_id.find(BIFEN_REQ_PREFIX) == 0) {
		OnBiFenReturn(request, response);
	}

}

int OkoooDialog::doLogin() {
	if (TRUE) {
		std::string url = "https://m.okooo.com/weixin/jing/d.php";
		CHttpRequestPtr request = CreateGetRequest(url, LOGIN_REQ_PREFIX);
		httpMgr_->DoHttpCommandRequest(request);
	}
	m_buLogin.EnableWindow(FALSE);
	m_buLogoff.EnableWindow(FALSE);
	return 0;
}


void OkoooDialog::OnLoginReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		doJcMatchList();
	} else {
		MessageBoxA("µÇÂ¼Ê§°Ü£¬ÍøÂç´íÎó!", "ÌáÊ¾", MB_OK);
		m_buLogin.EnableWindow(TRUE);
		m_buLogoff.EnableWindow(FALSE);
	}
}

int OkoooDialog::doLogOff() {
	m_buLogin.EnableWindow(TRUE);
	m_buLogoff.EnableWindow(FALSE);
	return 0l;
}


int OkoooDialog::doJcMatchList() {
	std::string url = "https://m.okooo.com/weixin/jing/d.php";
	CHttpRequestPtr request = CreateGetRequest(url, JCMATCHLIST_REQ_PREFIX);
	httpMgr_->DoHttpCommandRequest(request);
	return 0l;
}

CStringA CreateMatchDescription(const CStringA& ahost, const CStringA& aaway) {
	const char* vs("  VS  ");
	const char* buf = "               ";
	CStringA result;
	CStringA host(buf), away(buf);
	memcpy((char*)(LPCSTR)host + host.GetLength() - ahost.GetLength(), (LPCSTR)ahost, ahost.GetLength());
	memcpy((char*)(LPCSTR)away, (LPCSTR)aaway, aaway.GetLength());
	result = host + vs + away;
	return result;
}

void OkoooDialog::OnJcMatchListReturn(const CHttpRequestPtr& request,
	const CHttpResponseDataPtr& response) {
	std::multimap<std::string, std::shared_ptr<JCMatchItem>> items;
	std::map<std::string, std::shared_ptr<JCMatchItem>> order_items;
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		CZlibStream zlib;
		std::string raw_response;
		zlib.DecompressGZip(response->response_content, raw_response);
		CStringA section_begin = "<section id=\"match_list\" class=\"listbox listbox_jingcai\"";
		CStringA section_end = "</section>";
		CStringA temp = CT2A(CA2T(raw_response.c_str(), CP_UTF8).m_psz).m_psz;
		int nFindBegin = temp.Find(section_begin);
		if (nFindBegin == -1) {
			return;
		}
		int nFindEnd = temp.Find(section_end, nFindBegin);
		CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin  + section_end.GetLength());
		std::string date;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* tempElement = nullptr;
		if (doc.Parse(xmlText) != tinyxml2::XML_SUCCESS) {
			return;
		}
		tinyxml2::XMLElement* child = doc.FirstChildElement();
		if (child == nullptr) {
			return;
		}
		child = child->FirstChildElement();
		while (child != nullptr) {
			//child->FindAttribute
			CStringA class_value = GetElementClassAttrValue(child);
			if (class_value == "clearfix listtop ctrl_daybar") {
				tempElement = FindElementByClassAttr(child, "listtoptxt fl font12");
				std::vector<CStringA> temp_vec;
				CMiscHelper::SplitCStringA(GetElementText(tempElement), "&nbsp;&nbsp;", temp_vec);
				if (temp_vec.size() == 3) {
					date = temp_vec[1];
				}
				if (date.empty()) {
					return;
				}
			}
			else if (class_value == "listItemjczq") {
				tinyxml2::XMLElement* son = child->FirstChildElement();
				while (son != nullptr) {
					//""<div class = "width320">
					CStringA son_class_value = GetElementClassAttrValue(son);
					if (son_class_value == "clearfix center listItem ctrl_eachmatch") {
						tinyxml2::XMLElement* son_child = son->FirstChildElement();
						if (son_child != nullptr) {
							std::shared_ptr<JCMatchItem> ji(new JCMatchItem());
							ji->id = date + (LPCSTR)GetElementText(FindElementByClassAttr(son_child, "xuhao"));
							CMiscHelper::string_replace(ji->id, "-", "");
							ji->match_category = GetElementText(FindElementByClassAttr(son_child, "liansai"));
							ji->start_time = ji->last_buy_time = date + std::string(" ") + (LPCSTR)GetElementText(FindElementByClassAttr(son_child, "timetxt"));
							tempElement = FindElementByClassAttr(son_child, "fr listmore ctrl_addmore");
							ji->descrition = CreateMatchDescription(GetElementAttrValue(tempElement, "hn"), GetElementAttrValue(tempElement, "an"));
							ji->orderid = GetElementAttrValue(tempElement, "orderid");
							items.insert(std::make_pair(date, ji));
							order_items.insert(std::make_pair(ji->orderid, ji));
						}
					}
					son = son->NextSiblingElement();
				}
			}
			child = child->NextSiblingElement();
		}
		section_begin = "var oddsData = ";
		section_end = "};";
		nFindBegin = temp.Find(section_begin);
		if (nFindBegin == -1) {
			return;
		}
		nFindEnd = temp.Find(section_end, nFindBegin);
		CStringA jsText = temp.Mid(nFindBegin + section_begin.GetLength(), 
			nFindEnd - nFindBegin + section_end.GetLength() - section_begin.GetLength());
		Json::Value rootValue, itemValue, dataValue, tempValue;
		//std::map<std::string, std::map<int, double>> odds;
		if (ParseJsonString((LPCSTR)jsText, rootValue) && rootValue.isObject()) {
			bool first = false;
#ifdef _DEBUG
			first = true;
#endif
			for (auto& item : order_items) {
				GetValueFromJsonObject(rootValue, item.first, &itemValue);
				if (itemValue.isObject()) {
					//std::map<int, double> odd;
					double dTemp = 0.0;
					GetValueFromJsonObject(itemValue, "Boundary", &tempValue);
					if (tempValue.isObject()) {
						if (GetDoubleFromJsonObject(tempValue, "SportteryWDL", &dTemp)) {
							item.second->hand = (int)dTemp;
						}
					}
					GetValueFromJsonObject(itemValue, "OddsList", &tempValue);
					if (tempValue.isObject()) {
						GetValueFromJsonObject(tempValue, "SportteryWDL", &dataValue);
						for (auto& iter = dataValue.begin(); iter != dataValue.end(); ++iter) {
							GetDoubleFromJsonObject(dataValue, iter.memberName(), &dTemp);
							int k = atoi(iter.memberName());
							JCMatchItem::Subject sub;
							sub.tid = 1;
							sub.odds = dTemp;
							sub.betCode = k - 10;
							sub.checked = false;
							sub.calcTip(item.second->hand);
							item.second->subjects.push_back(sub);
						}
						GetValueFromJsonObject(tempValue, "SportteryNWDL", &dataValue);
						for (auto& iter = dataValue.begin(); iter != dataValue.end(); ++iter) {
							GetDoubleFromJsonObject(dataValue, iter.memberName(), &dTemp);
							int k = atoi(iter.memberName());
							JCMatchItem::Subject sub;
							sub.tid = 6;
							sub.odds = dTemp;
							sub.betCode = 3;
							if (k == 15) sub.betCode = 1;
							else if (k == 14) sub.betCode = 0;
							sub.checked = false;
							sub.calcTip(item.second->hand);
							item.second->subjects.push_back(sub);
						}

						GetValueFromJsonObject(tempValue, "SportteryHalfFull", &dataValue);
						for (auto& iter = dataValue.begin(); iter != dataValue.end(); ++iter) {
							GetDoubleFromJsonObject(dataValue, iter.memberName(), &dTemp);
							int k = atoi(iter.memberName());
							JCMatchItem::Subject sub;
							sub.tid = 4;
							sub.odds = dTemp;
							sub.betCode = k - 20;
							sub.checked = false;
							sub.calcTip(item.second->hand);
							item.second->subjects.push_back(sub);
						}

						GetValueFromJsonObject(tempValue, "SportteryScore", &dataValue);
						for (auto& iter = dataValue.begin(); iter != dataValue.end(); ++iter) {
							GetDoubleFromJsonObject(dataValue, iter.memberName(), &dTemp);
							int k = atoi(iter.memberName());
							JCMatchItem::Subject sub;
							sub.tid = 3;
							sub.odds = dTemp;
							sub.betCode = k - 30;
							sub.checked = false;
							sub.calcTip(item.second->hand);
							item.second->subjects.push_back(sub);
						}
						GetValueFromJsonObject(tempValue, "SportteryTotalGoals", &dataValue);
						for (auto& iter = dataValue.begin(); iter != dataValue.end(); ++iter) {
							GetDoubleFromJsonObject(dataValue, iter.memberName(), &dTemp);
							int k = atoi(iter.memberName());
							JCMatchItem::Subject sub;
							sub.tid = 2;
							sub.odds = dTemp;
							sub.betCode = k - 0;
							sub.checked = false;
							sub.calcTip(item.second->hand);
							item.second->subjects.push_back(sub);
						}
					}
				}
				if (first) {
					first = false;
					std::string lines;
					for (const auto& sub : item.second->subjects) {
						lines += sub.lineStr();
					}
					Global::SaveFileData("e:\\zcjc.txt", lines, FALSE);
				}
			}
		}

	}
	if (!items.empty()) {
		for (auto& iter : items) {
			JCMatchItem item;
			if (GetItemFromDB(*iter.second, item)) {
				*iter.second = item;
			} else {
				InsertItemToDB(*iter.second);
			}
		}
		m_JCMatchItems.swap(items);
	}
	doBiFen();
	ReloadMatchListData();
	m_buLogin.EnableWindow(FALSE);
	m_buLogoff.EnableWindow(TRUE);
}

int OkoooDialog::doBiFen() {
	CStringATL beginDay, endDay, beginWeekDay;
	Global::getBiFenDateInfo(beginDay, endDay, beginWeekDay);
	CStringATL url;
	url.Format("http://www.okooo.com/jingcai/kaijiang/?LotteryType=SportteryWDL&StartDate=%s&EndDate=%s",
		beginDay, endDay);
	std::string request_id = BIFEN_REQ_PREFIX;
	CHttpRequestPtr request = CreateGetRequest((LPCSTR)url, request_id);
	httpMgr_->DoHttpCommandRequest(request);
	return 0;
}

void OkoooDialog::OnBiFenReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
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



