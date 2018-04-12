#include "stdafx.h"
#include "DanLueDialog.h"
#include "Global.h"

BOOL DanLueDialog::OnIdle() {
	int async_count = GetAsyncFuncCount();
	if (async_count > 0) {
		PostMessage(DanLueDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	return FALSE;
}

LRESULT DanLueDialog::OnAsyncDispatch(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	DispathOneAsyncFunc();
	return 1L;
}

bool DanLueDialog::AddOneAsyncFunc(talk_base::IAsyncFuncCall *pAsyncFunc) {
	BOOL bRet = CAsyncFuncDispatcher::AddOneAsyncFunc(pAsyncFunc);
	if (bRet) {
		bRet = PostMessage(DanLueDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	return (bRet == TRUE);
}

void DanLueDialog::OnHttpReturnGlobal(const CHttpRequestPtr& request, 
		const CHttpResponseDataPtr& response) {
	sInst.AddOneAsyncFunc(talk_base::BindAsyncFunc(&DanLueDialog::OnHttpReturn, &sInst, request, response));
}

CHttpRequestPtr DanLueDialog::CreatePostRequest(const std::string& url, const std::string& idprefix, 
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
	ptr->callback = &DanLueDialog::OnHttpReturnGlobal;
	//httpMgr_->DoHttpCommandRequest(ptr);
	return ptr;
}

CHttpRequestPtr DanLueDialog::CreateGetRequest(const std::string& url, const std::string& idprefix) {
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
	ptr->callback = &DanLueDialog::OnHttpReturnGlobal;
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

void DanLueDialog::OnHttpReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (request->request_id.find(LOGIN_REQ_PREFIX) == 0) {
		OnLoginReturn(request, response);
	}
	else if (request->request_id.find(LOGOFF_REQ_PREFIX) == 0) {
		OnLogOffReturn(request, response);
	}
	else if (request->request_id.find(INFO_REQ_PREFIX) == 0) {
		OnInfoReturn(request, response);
	}
	else if (request->request_id.find(RCTOKEN_REQ_PREFIX) == 0) {
		OnRcTokenReturn(request, response);
	}
	else if (request->request_id.find(FRIENDLIST_REQ_PREFIX) == 0) {
		OnFriendListReturn(request, response);
	}
	else if (request->request_id.find(LOTTERYCATEGORIES_REQ_PREFIX) == 0) {
		OnLotteryCategoriesReturn(request, response);
	}
	else if (request->request_id.find(JCMATCHLIST_REQ_PREFIX) == 0) {
		OnJcMatchListReturn(request, response);
	}
	else if (request->request_id.find(HEMAI_REQ_PREFIX) == 0) {
		OnHeMaiReturn(request, response);
	}
	else if (request->request_id.find(BIFEN_REQ_PREFIX) == 0) {
		OnBiFenReturn(request, response);
	}

}

int DanLueDialog::doLogin() {
	if (m_LoginToken.empty()) {
		CStringATL strLoginFile = m_strRootDir + "\\danlue_login.dat";
		std::string data;
		if (!Global::ReadFileData((LPCTSTR)strLoginFile, data) || data.empty()) {
			return -1;
		}
		std::string url = "http://newapp.87.cn/account/login";
		CHttpRequestPtr request = CreatePostRequest(url, LOGIN_REQ_PREFIX, data);
		//request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
		request->request_headers.insert(std::make_pair("Content-Type", "application/json"));
		
		httpMgr_->DoHttpCommandRequest(request);
	}
	m_buLogin.EnableWindow(FALSE);
	m_buLogoff.EnableWindow(FALSE);
	return 0;
}


void DanLueDialog::OnLoginReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue, dataValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			if (header.status != 0) {
				MessageBoxA(Global::fromUTF8(header.message).c_str(), "提示", MB_OK);
				m_buLogin.EnableWindow(TRUE);
				m_buLogoff.EnableWindow(FALSE);
			}
			int64 userID = 0;

			GetValueFromJsonObject(rootValue, "data", &dataValue);
			GetStringFromJsonObject(dataValue, "data", &m_LoginToken);
			GetInt64FromJsonObject(dataValue, "userId", &userID);
			GetStringFromJsonObject(dataValue, "niname", &m_NickName);
			GetStringFromJsonObject(dataValue, "rcuserid", &m_RcUserID);
			GetInt64FromJsonObject(dataValue, "slwId", &m_slwId);
			char buf[128] = { '\0' };
			_i64toa(userID, buf, 10);
			m_UserID = buf;
			m_buLogoff.EnableWindow(TRUE);
			doInfo();
			doRcToken();
			doFriendList();
			doLotteryCategories();
		}
	} else {
		MessageBoxA("登录失败，网络错误!", "提示", MB_OK);
		m_buLogin.EnableWindow(TRUE);
		m_buLogoff.EnableWindow(FALSE);
	}
}

int DanLueDialog::doLogOff() {
	if (m_UserID.empty()) {
		return 0;
	}
	Json::Value root;
	Json::FastWriter writer;
	root["sid"] = m_UserID;
	root["token"] = "";
	root["userId"] = 0;
	std::string json = writer.write(root);
	std::string url = "http://appserver.87.cn/user/information";
	CHttpRequestPtr request = CreatePostRequest(url, LOGOFF_REQ_PREFIX, json);
	request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	httpMgr_->DoHttpCommandRequest(request);
	return 0l;
}

void DanLueDialog::OnLogOffReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			//if (header.status == 0) {
				m_buLogin.EnableWindow(TRUE);
				m_buLogoff.EnableWindow(FALSE);
			//}
		}
	}
}

int DanLueDialog::doInfo() {
	Json::Value root;
	Json::FastWriter writer;
	root["sid"] = m_UserID;
	root["token"] = m_LoginToken;
	root["userId"] = 0;
	std::string json = writer.write(root);
	std::string url = "http://appserver.87.cn/user/information";
	CHttpRequestPtr request = CreatePostRequest(url, INFO_REQ_PREFIX, json);
	request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	httpMgr_->DoHttpCommandRequest(request);
	return 0l;
}

void DanLueDialog::OnInfoReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue, dataValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			if (header.status == 0) {
				GetValueFromJsonObject(rootValue, "data", &dataValue);
				if (dataValue.isObject()) {
					//todo
				}
			}
		}
	}
}

int DanLueDialog::doRcToken() {
	Json::Value root;
	Json::FastWriter writer;
	root["rcuserid"] = m_RcUserID;
	root["sid"] = m_UserID;
	root["token"] = m_LoginToken;
	root["userId"] = 0;
	std::string json = writer.write(root);
	std::string url = "http://appserver.87.cn/rongcloud/token";
	CHttpRequestPtr request = CreatePostRequest(url, RCTOKEN_REQ_PREFIX, json);
	request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	httpMgr_->DoHttpCommandRequest(request);
	return 0l;
}

void DanLueDialog::OnRcTokenReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue, dataValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			if (header.status == 0) {
				GetValueFromJsonObject(rootValue, "data", &dataValue);
				if (dataValue.isObject()) {
					GetStringFromJsonObject(dataValue, "rctoken", &m_RcUserToken);
				}
			}
		}
	}
}

int DanLueDialog::doFriendList() {
	Json::Value root;
	Json::FastWriter writer;
	root["rcuserid"] = m_RcUserID;
	root["sid"] = m_UserID;
	root["slwid"] = m_slwId;
	root["token"] = m_LoginToken;
	root["uptime"] = 0;
	root["userId"] = 0;
	std::string json = writer.write(root);
	std::string url = "http://appserver.87.cn/chat/friendlist";
	CHttpRequestPtr request = CreatePostRequest(url, FRIENDLIST_REQ_PREFIX, json);
	request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	httpMgr_->DoHttpCommandRequest(request);
	return 0l;
}

void DanLueDialog::OnFriendListReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue, dataValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			if (header.status == 0) {
				GetValueFromJsonObject(rootValue, "data", &dataValue);
				if (dataValue.isObject()) {
					//todo
				}
			}
		}
	}
}

int DanLueDialog::doLotteryCategories() {
	Json::Value root;
	Json::FastWriter writer;
	root["sid"] = m_UserID;
	root["userId"] = 0;
	std::string json = writer.write(root);
	std::string url = "http://appserver.87.cn/lottery/lottery_categories";
	CHttpRequestPtr request = CreatePostRequest(url, LOTTERYCATEGORIES_REQ_PREFIX, json);
	request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	httpMgr_->DoHttpCommandRequest(request);
	return 0l;
}

void DanLueDialog::OnLotteryCategoriesReturn(const CHttpRequestPtr& request, 
		const CHttpResponseDataPtr& response)	{
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue, dataValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			GetValueFromJsonObject(rootValue, "data", &dataValue);
			if (header.status == 0 || dataValue.isArray()) {
				//GetValueFromJsonObject(rootValue, "data", &dataValue);
				if (dataValue.isArray()) {
					for (int i = 0; i < dataValue.size(); i++) {
						Json::Value itemValue;
						GetValueFromJsonArray(dataValue, i, &itemValue);
						if (itemValue.isObject()) {
							LotteryCategories lc;
							int64 cID;
							GetStringFromJsonObject(itemValue, "description", &lc.description);
							lc.description = Global::fromUTF8(lc.description);
							GetInt64FromJsonObject(itemValue, "id", &cID);
							GetStringFromJsonObject(itemValue, "label", &lc.label);
							GetStringFromJsonObject(itemValue, "path", &lc.path);
							char buf[128] = { '\0' };
							_i64toa(cID, buf, 10);
							lc.id = buf;
							m_LotteryCategories.push_back(lc);
						}
					}
				}
			}
		}
	}
	for (const auto& lc : m_LotteryCategories) {
		if (lc.id == "227") {
			doJcMatchList();
			break;
		}
	}

}

int DanLueDialog::doJcMatchList() {
	Json::Value root;
	Json::FastWriter writer;
	root["licenseId"] = "227";
	root["sid"] = m_UserID;
	root["token"] = m_LoginToken;
	root["type"] = "hhgg";
	root["userId"] = 0;
	std::string json = writer.write(root);
	std::string url = "http://appserver.87.cn/jc/match";
	CHttpRequestPtr request = CreatePostRequest(url, JCMATCHLIST_REQ_PREFIX, json);
	request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	httpMgr_->DoHttpCommandRequest(request);
	return 0l;
}

void DanLueDialog::OnJcMatchListReturn(const CHttpRequestPtr& request, 
		const CHttpResponseDataPtr& response) {
	std::multimap<std::string, std::shared_ptr<JCMatchItem>> temp;
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue, dataValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			if (header.status == 0) {
				GetValueFromJsonObject(rootValue, "data", &dataValue);
				if (dataValue.isObject()) {
					int64 aheadtime;
					GetInt64FromJsonObject(dataValue, "aheadtime", &aheadtime);
					Json::Value schedulesValue;
					GetValueFromJsonObject(dataValue, "schedules", &schedulesValue);
					if (schedulesValue.isArray()) {
						for (int i = 0; i < schedulesValue.size(); i++) {
							Json::Value scheduleValue;
							GetValueFromJsonArray(schedulesValue, i, &scheduleValue);
							if (scheduleValue.isObject()) {
								std::string date;
								GetStringFromJsonObject(scheduleValue, "date", &date);
								Json::Value listValue;
								GetValueFromJsonObject(scheduleValue, "list", &listValue);
								if (listValue.isArray()) {
									for (int j = 0; j < listValue.size(); j++) {
										std::shared_ptr<JCMatchItem> ji(new JCMatchItem());
										Json::Value itemsValue;
										GetValueFromJsonArray(listValue, j, &itemsValue);
										if (itemsValue.isArray()) {
											for (int k = 0; k < itemsValue.size(); k++) {
												Json::Value itemValue;
												GetValueFromJsonArray(itemsValue, k, &itemValue);
												if (itemValue.isObject()) {
													int64 type = -1;
													Json::Value playValue;
													GetInt64FromJsonObject(itemValue, "type", &type);
													GetValueFromJsonObject(itemValue, "play", &playValue);
													if (playValue.isObject()) {
														std::string v1, v2;
														int64 cID = -1;
														GetStringFromJsonObject(playValue, "hn2", &v1);
														GetStringFromJsonObject(playValue, "an2", &v2);
														ji->descrition = Global::toFixedLengthString(v1, 18, true) + "   VS   " + Global::toFixedLengthString(v2, 18, false);
														ji->descrition = Global::fromUTF8(ji->descrition);
														GetStringFromJsonObject(playValue, "lid", &v1);
														ji->match_category = Global::fromUTF8(v1);
														GetStringFromJsonObject(playValue, "dt", &ji->start_time);
														GetStringFromJsonObject(playValue, "ot", &ji->last_buy_time);
														GetInt64FromJsonObject(playValue, "id", &cID);
														char buf[128] = { '\0' };
														_i64toa(cID, buf, 10);
														ji->id = buf;
														GetStringFromJsonObject(playValue, "gameType", &v1);
														GetInt64FromJsonObject(playValue, "hand", &ji->hand);
														Json::Value oddsValue;
														if (v1 == "dg") {
															GetValueFromJsonObject(playValue, "spTypeDg", &oddsValue);
														} else {
															GetValueFromJsonObject(playValue, "spTypeGg", &oddsValue);
														}
														if (oddsValue.isArray()) {
															std::vector<double> arrOdds;
															for (int m = 0; m < oddsValue.size(); m++) {
																double odds = 0.0;
																GetDoubleFromJsonArray(oddsValue, m, &odds);
																if (type == 4 && m == 12) {
																	arrOdds.insert(arrOdds.begin(), odds);
																} else if (type == 4 && m == 17) {
																	arrOdds.insert(arrOdds.begin() + 13, odds);
																} else if (type == 4 && m == 30) {
																	arrOdds.insert(arrOdds.begin() + 18, odds);
																} else {
																	arrOdds.push_back(odds);
																}
															}
															for (int m = 0; m < arrOdds.size(); m++) {
																JCMatchItem::Subject sub;
																if (type == 0) {
																	sub.tid = 6; //胜平负
																	sub.betCode = 3;
																	if (m == 1) {
																		sub.betCode = 1;
																	} else if (m == 2) {
																		sub.betCode = 0;
																	}
																}
																else if (type == 1) {
																	sub.tid = 1; //让球胜平负
																	sub.betCode = 3;
																	if (m == 1) {
																		sub.betCode = 1;
																	}
																	else if (m == 2) {
																		sub.betCode = 0;
																	}
																}
																else if (type == 2) {
																	sub.tid = 2; //进球总数
																	sub.betCode = m;
																}
																else if (type == 3) {
																	sub.tid = 4; //半全场
																	sub.betCode = m;
																} 
																else if (type == 4) {
																	sub.tid = 3; //比分
																	sub.betCode = m;
																}
																sub.odds = arrOdds[m];
																sub.calcTip(ji->hand);
																sub.checked = false;
																ji->subjects.push_back(sub);
															}
														}
													}
												}
											}
										}
										temp.insert(std::make_pair(date, ji));
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (!temp.empty()) {
		for (auto& iter : temp) {
			JCMatchItem item;
			if (GetItemFromDB(iter.second->id, item)) {
				*iter.second = item;
			} else {
				InsertItemToDB(*iter.second);
			}
		}
		m_JCMatchItems.swap(temp);
	}
	doBiFen();
	ReloadMatchListData();
}

int DanLueDialog::doHeMai() {
	if (m_Engine.get() == NULL) {
		MessageBoxA("请先加载Lua脚本", "错误", MB_OK | MB_ICONERROR);
		return -1;
	}
	bool last = false;
	CStlStrArray matchIDs;
	CStlStrxyArray records;
	int start = 0, max_count = 100;
	m_Engine->getResults(start, max_count, matchIDs, records, last);
	if (records.empty() || matchIDs.empty()) {
		MessageBoxA("请先计算结果", "错误", MB_OK | MB_ICONERROR);
		return -1;
	}
	CStlString saveFile = m_Engine->getScriptFile() + ".dat";
	DeleteFile(saveFile.c_str());

	const std::vector<JcBetItemSource>& vecFixedSources = m_Engine->GetFixedSources();
	do {
		last = false;
		records.clear();
		matchIDs.clear();
		m_Engine->getResults(start, max_count, matchIDs, records, last);
		if (vecFixedSources.empty()) {
			if (doHeMaiImpl(records, matchIDs, last) != 0) {
				return -1;
			}
		} else {
			if (doHeMaiImpl_FuShi(records, matchIDs, vecFixedSources, last) != 0) {
				return -1;
			}
		}
		start += records.size();
	} while (!last);
	return 0;
}


int DanLueDialog::doHeMaiImpl_FuShi(const CStlStrxyArray& records, const CStlStrArray& matchIDs, 
		const std::vector<JcBetItemSource>& vecFixedSources, bool last) {
	if (records.empty() || matchIDs.empty()) {
		return 0;
	}
	std::string title = m_Engine->getFanAnTitle();
	if (title.empty()) {
		title = Global::toUTF8("竞彩合买");
	}
	std::string desc = m_Engine->getFanAnDesc();
	if (desc.empty()) {
		desc = Global::toUTF8("信就有");
	}
	int nFixedRecordCount = 1;
	for (const auto& fixed : vecFixedSources) {
		nFixedRecordCount = nFixedRecordCount * fixed.bets.size();
	}
	//int nTotalRecordCount

	CStringATL temp, strRecords;
	Json::Value root;
	Json::FastWriter writer;
	root["title"] = title;
	root["hemaidesc"] = desc;
	root["baodinumber"] = 0;
	root["brokerage"] = 0;

	root["eventId"] = 227;
	root["hemaitype"] = "5";
	root["isshow"] = 1;
	root["licenseId"] = "227";
	root["mult"] = 1;
	root["odds"] = "";
	root["programDesc"] = "227";
	root["hemaisuccessdesc"] = ""; //todo
	root["hemaipaydesc"] = ""; //todo
	root["sid"] = m_UserID;
	root["token"] = m_LoginToken;
	root["uploadstate"] = "0";
	root["userId"] = 0;
	temp.Format("%dc1", matchIDs.size() + vecFixedSources.size());
	root["passtype"] = (LPCSTR)temp;//todo

	strRecords.Empty();
	for (const auto& record : records) {
		//const auto& record = records[i];
		CStlString item;
		for (int i = 0; i < record.size(); i++) {
			const auto& code = record[i];
			const auto& match = matchIDs[i];
			if (!item.empty()) {
				item += "|";
			}
			item = item + match + ":" + code + ":" + "0";
		}
		for (const auto& fixed : vecFixedSources) {
			CStlString itemFixed;
			for (const auto& code : fixed.bets) {
				if (itemFixed.empty()) {
					itemFixed = CStlString("|") + fixed.id + ":" + code.betCode();
				} else {
					itemFixed = itemFixed + "," + code.betCode();
				}
			}
			if (!itemFixed.empty()) {
				itemFixed = itemFixed + ":" + "0";
				item += itemFixed;
			}
		}
		int buynumber = (nFixedRecordCount * 2 / 100);
		buynumber = buynumber + 1;
		root["buynumber"] = buynumber; 
		root["detail"] = item.c_str();
		root["money"] = 2 * nFixedRecordCount;
		root["sharenumber"] = nFixedRecordCount; 
		std::string json = writer.write(root);
		std::string url = "http://appserver.87.cn/lottery/hemai";
		std::string request_id = HEMAI_REQ_PREFIX;
		if (last) {
			request_id = HEMAI_REQ_PREFIX_FINISH;
		}
		CHttpRequestPtr request = CreatePostRequest(url, request_id, json);
		request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
		httpMgr_->DoHttpCommandRequest(request);
		strRecords += item.c_str();
		strRecords += "\r\n";
	}
	//备份上传 结果
	strRecords.Replace("|", "\r\n");
	strRecords += "\r\n";
	CStlString saveFile = m_Engine->getScriptFile() + ".dat";
	Global::SaveFileData(saveFile, (LPCSTR)strRecords, TRUE);
	return 0L;
}


int DanLueDialog::doHeMaiImpl(const CStlStrxyArray& records, const CStlStrArray& matchIDs, bool last) {
	if (records.empty() || matchIDs.empty()) {
		return 0;
	}
	std::string title = m_Engine->getFanAnTitle();
	if (title.empty()) {
		title = Global::toUTF8("竞彩合买");
	}
	std::string desc = m_Engine->getFanAnDesc();
	if (desc.empty()) {
		desc = Global::toUTF8("信就有");
	}

	Json::Value root;
	Json::FastWriter writer;
	root["baodinumber"] = 0;
	root["brokerage"] = 0;
	int buynumber = (records.size() * 2 / 100);
	buynumber = buynumber + 1;
	root["buynumber"] = buynumber; //
	root["eventId"] = 227;
	root["hemaitype"] = "5";
	root["isshow"] = 1;
	root["licenseId"] = "227";
	root["mult"] = -1;
	root["odds"] = "";
	root["programDesc"] = "227";
	
	root["sid"] = m_UserID;
	root["token"] = m_LoginToken;
	root["uploadstate"] = "1";
	root["userId"] = 0;
	
	root["title"] = title;
	root["hemaidesc"] = desc;
	CStringATL temp, temp1, temp2;
	for (const auto& record : records) {
		if (!temp1.IsEmpty()) {
			temp1 += ",";
		}
		temp1 += "1";
		CStringATL line;
		for (const auto& code : record) {
			if (!line.IsEmpty()) {
				line += ",";
			}
			line += code.c_str();
		}
		if (!temp.IsEmpty()) {
			temp += "|";
		}
		temp += line;
	}
	for (const auto& match : matchIDs) {
		if (!temp2.IsEmpty()) {
			temp2 += ",";
		}
		temp2 += match.c_str();
	}
	CStringATL strRecords = temp;
	root["detail"] = (LPCSTR)temp; //todo
	root["hemaipaydesc"] = (LPCSTR)temp2; //todo
	root["hemaisuccessdesc"] = (LPCSTR)temp1; //todo
	root["money"] = 2 * records.size();// todo
	temp.Format("%dc1", matchIDs.size());
	root["passtype"] = (LPCSTR)temp;//todo
	root["sharenumber"] = records.size(); //todo

	std::string json = writer.write(root);

	std::string url = "http://appserver.87.cn/lottery/hemai";
	std::string request_id = HEMAI_REQ_PREFIX;
	if (last) {
		request_id = HEMAI_REQ_PREFIX_FINISH;
	}
	CHttpRequestPtr request = CreatePostRequest(url, request_id, json);
	request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	httpMgr_->DoHttpCommandRequest(request);

	//备份上传 结果
	strRecords.Replace("|", "\r\n");
	strRecords += "\r\n";
	CStlString saveFile = m_Engine->getScriptFile() + ".dat";
	Global::SaveFileData(saveFile, (LPCSTR)strRecords, TRUE);
	return 0L;
}

void DanLueDialog::OnHeMaiReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	CStringATL strMsg = "上传完成！";
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue, dataValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			strMsg.AppendFormat(" msg: [%s]", header.message.c_str());
			if (header.status == 0) {
				GetValueFromJsonObject(rootValue, "data", &dataValue);
				if (dataValue.isObject()) {
					//todo
				}
			}
		}
	}
	if (request->request_id.find(HEMAI_REQ_PREFIX_FINISH) == 0) {
		m_buUpload.EnableWindow(TRUE);
		MessageBox(strMsg, "提示", MB_ICONINFORMATION | MB_OK);
	}
}

int DanLueDialog::doBiFen() {
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

void DanLueDialog::OnBiFenReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
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
		CStringATL html = response->response_content.c_str();
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

				} else if (i == 6 || i == 7) {
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
					} else {
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

void DanLueDialog::JCMatchItem::Subject::calcTip(int hand) {
	CStringATL temp;
	if (tid == 6) {
		if (betCode == 3) {
			tip = "胜";
		} else if (betCode == 1) {
			tip = "平";
		} else if (betCode == 0) {
			tip = "负";
		}
	}
	else if (tid == 1) {
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
		} else {
			temp.Format("%d+球", betCode);
		}
		tip = temp;
	}
	else if (tid == 4) {
		if (betCode == 0) {
			tip = "胜胜";
		} 
		else if (betCode == 1) {
			tip = "胜平";
		}
		else if (betCode == 2) {
			tip = "胜负";
		}
		else if (betCode == 3) {
			tip = "平胜";
		}
		else if (betCode == 4) {
			tip = "平平";
		}
		else if (betCode == 5) {
			tip = "平负";
		}
		else if (betCode == 6) {
			tip = "负胜";
		}
		else if (betCode == 7) {
			tip = "负平";
		}
		else if (betCode == 8) {
			tip = "负负";
		}
	}
	else if (tid == 3) {
		if (betCode == 0) {
			tip = "胜其它";
		}
		else if (betCode == 1) {
			tip = "1:0";
		}
		else if (betCode == 2) {
			tip = "2:0";
		}
		else if (betCode == 3) {
			tip = "2:1";
		}
		else if (betCode == 4) {
			tip = "3:0";
		}
		else if (betCode == 5) {
			tip = "3:1";
		}
		else if (betCode == 6) {
			tip = "3:2";
		}
		else if (betCode == 7) {
			tip = "4:0";
		}
		else if (betCode == 8) {
			tip = "4:1";
		}
		else if (betCode == 9) {
			tip = "4:2";
		}
		else if (betCode == 10) {
			tip = "5:0";
		}
		else if (betCode == 11) {
			tip = "5:1";
		}
		else if (betCode == 12) {
			tip = "5:2";
		}

		else if (betCode == 13) {
			tip = "平其它";
		}
		else if (betCode == 14) {
			tip = "0:0";
		}
		else if (betCode == 15) {
			tip = "1:1";
		}
		else if (betCode == 16) {
			tip = "2:2";
		}
		else if (betCode == 17) {
			tip = "3:2";
		}


		else if (betCode == 18) {
			tip = "负其它";
		}
		else if (betCode == 19) {
			tip = "0:1";
		}
		else if (betCode == 20) {
			tip = "0:2";
		}
		else if (betCode == 21) {
			tip = "1:2";
		}
		else if (betCode == 22) {
			tip = "0:3";
		}
		else if (betCode == 23) {
			tip = "1:3";
		}
		else if (betCode == 24) {
			tip = "2:3";
		}
		else if (betCode == 25) {
			tip = "0:4";
		}
		else if (betCode == 26) {
			tip = "1:4";
		}
		else if (betCode == 27) {
			tip = "2:4";
		}
		else if (betCode == 28) {
			tip = "0:5";
		}
		else if (betCode == 29) {
			tip = "1:5";
		}
		else if (betCode == 30) {
			tip = "2:5";
		}

	}

}

std::string DanLueDialog::JCMatchItem::Subject::betStr() {
	std::string ret;
	char cb[20] = { '\0' };
	sprintf(cb, "%d-%d(%.2f)", (int)tid, (int)betCode, odds);
	ret = cb;
	return ret;
}

int DanLueDialog::JCMatchItem::Subject::getPan(int hand) const {
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

DanLueDialog::JCMatchItem::Subject* DanLueDialog::JCMatchItem::get_subject(int tid, int betCode) {
	DanLueDialog::JCMatchItem::Subject* result = NULL;
	for (auto& subject : subjects) {
		if (subject.tid == tid && subject.betCode == betCode) {
			result = &subject;
			break;
		}
	}
	return result;
}

DanLueDialog::JCMatchItem::Subject* DanLueDialog::get_subjects(const std::string& id, int tid, int code) {
	for (const auto& item : m_JCMatchItems) {
		if (item.second->id == id) {
			return item.second->get_subject(tid, code);
		}
	}
	return NULL;
}

DanLueDialog::JCMatchItem::Subject* DanLueDialog::JCMatchItem::get_subject(int tid, const char* tip) {
	DanLueDialog::JCMatchItem::Subject* result = NULL;
	for (auto& subject : subjects) {
		if (subject.tid == tid && subject.tip.compare(tip) == 0) {
			result = &subject;
			break;
		}
	}
	return result;
}










//http://www.okooo.com/jingcai/kaijiang/?LotteryType=SportteryWDL&StartDate=2018-03-05&EndDate=2018-03-06



