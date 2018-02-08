#include "stdafx.h"
#include "DanLueDialog.h"
#include "Global.h"

LRESULT DanLueDialog::OnAsyncDispatch(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	DispathOneAsyncFunc();
	return 1L;
}

bool DanLueDialog::AddOneAsyncFunc(talk_base::IAsyncFuncCall *pAsyncFunc) {
	bool bRet = PostMessage(DanLueDialog::WM_ASYNC_DISPATCH, 0, 0);
	if (bRet) {
		bRet = CAsyncFuncDispatcher::AddOneAsyncFunc(pAsyncFunc);
	}
	return bRet;
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
static const std::string LOGIN_REQ_PREFIX = "login_req_prefix";
static const std::string LOGOFF_REQ_PREFIX = "logoff_req_prefix";
static const std::string INFO_REQ_PREFIX = "info_req_prefix";
static const std::string RCTOKEN_REQ_PREFIX = "rctoken_req_prefix";
static const std::string FRIENDLIST_REQ_PREFIX = "friendlist_req_prefix";
static const std::string LOTTERYCATEGORIES_REQ_PREFIX = "lotterycategories_req_prefix";
static const std::string JCMATCHLIST_REQ_PREFIX = "jcmatchlist_req_prefix";



struct ResHeader {
	ResHeader() {
		status = -1;
		timestamp = 0;
		message = "unknown";
	}
	void parse(const Json::Value& rootValue) {
		GetIntFromJsonObject(rootValue, "status", &status);
		GetStringFromJsonObject(rootValue, "message", &message);
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

	



}

int DanLueDialog::doLogin() {
	if (m_LoginToken.empty()) {
		CStringATL strLoginFile = m_strRootDir + "\\danlue_login.dat";
		std::string data;
		if (!Global::ReadFileData((LPCTSTR)strLoginFile, data) || data.empty()) {
			return -1;
		}
		std::string url = "http://appserver.87.cn/account/login";
		CHttpRequestPtr request = CreatePostRequest(url, LOGIN_REQ_PREFIX, data);
		request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
		httpMgr_->DoHttpCommandRequest(request);
	}
	m_buLogin.EnableWindow(FALSE);
	m_buLogoff.EnableWindow(FALSE);
	return 0;
}


void DanLueDialog::OnLoginReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE) {
		Json::Value rootValue;
		if (ParseJsonString(response->response_content, rootValue) && rootValue.isObject()) {
			ResHeader header;
			header.parse(rootValue);
			if (header.status != 0) {
				MessageBoxA(Global::fromUTF8(header.message).c_str(), "ÌáÊ¾", MB_OK);
				m_buLogin.EnableWindow(TRUE);
				m_buLogoff.EnableWindow(FALSE);
			}
			int64 userID = 0;
			GetStringFromJsonObject(rootValue, "data", &m_LoginToken);
			GetInt64FromJsonObject(rootValue, "userId", &userID);
			GetStringFromJsonObject(rootValue, "niname", &m_NickName);
			GetStringFromJsonObject(rootValue, "rcuserid", &m_RcUserID);
			GetInt64FromJsonObject(rootValue, "slwId", &m_slwId);
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
		MessageBoxA("µÇÂ¼Ê§°Ü£¬ÍøÂç´íÎó!", "ÌáÊ¾", MB_OK);
		m_buLogin.EnableWindow(TRUE);
		m_buLogoff.EnableWindow(FALSE);
	}
}

int DanLueDialog::doLogOff() {
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
			if (header.status == 0) {
				m_buLogin.EnableWindow(TRUE);
				m_buLogoff.EnableWindow(FALSE);
			}
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
	CHttpRequestPtr request = CreatePostRequest(url, FRIENDLIST_REQ_PREFIX, json);
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
			if (header.status == 0) {
				GetValueFromJsonObject(rootValue, "data", &dataValue);
				if (dataValue.isArray()) {
					for (int i = 0; i < dataValue.size(); i++) {
						Json::Value itemValue;
						GetValueFromJsonArray(dataValue, i, &itemValue);
						if (itemValue.isObject()) {
							LotteryCategories lc;
							int64 cID;
							GetStringFromJsonObject(itemValue, "description", &lc.description);
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
										Json::Value itemsValue;
										GetValueFromJsonArray(listValue, j, &itemsValue);
										if (itemsValue.isArray()) {
											for (int k = 0; k < itemsValue.size(); k++) {
												JCMatchItem ji;

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
														ji.descrition = v1 + "   VS   " + v2;
														GetStringFromJsonObject(playValue, "dt", &ji.start_time);
														GetStringFromJsonObject(playValue, "ot", &ji.last_buy_time);
														GetInt64FromJsonObject(playValue, "id", &cID);
														char buf[128] = { '\0' };
														_i64toa(cID, buf, 10);
														ji.id = buf;
														GetStringFromJsonObject(playValue, "gameType", &v1);
														


													}


												}
												m_JCMatchItems.insert(std::make_pair(date, ji));

											
											}
										
										}

									}
								
								}
							}
						}
					}
				}
			}
		}
	}

}
