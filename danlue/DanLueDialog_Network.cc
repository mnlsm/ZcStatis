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
												std::shared_ptr<JCMatchItem> ji(new JCMatchItem());
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
														ji->descrition = v1 + "   VS   " + v2;
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
			}
		}
	}

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
	sprintf(cb, "%d-%d", tid, betCode);
	ret = cb;
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





















/*
POST http://appserver.87.cn/lottery/hemai HTTP/1.1
Content-Type: application/x-www-form-urlencoded
User-Agent: Dalvik/2.1.0 (Linux; U; Android 6.0; M5s Build/MRA58K)
Host: appserver.87.cn
Connection: Keep-Alive
Accept-Encoding: gzip
Content-Length: 410

{"baodinumber":0,"brokerage":0,"buynumber":1,"detail":"20180208001:4-8:0|20180208002:4-0,4-3:0","eventId":227,"hemaidesc":"","hemaipaydesc":"","hemaisuccessdesc":"","hemaitype":"5","isshow":1,"licenseId":"227","money":4,"mult":1,"odds":"","passtype":"2c1","programDesc":"227","sharenumber":4,"sid":"1866628","title":"竞彩足球 合买","token":"3C37257AAB2E6A2144390DD83E83C266","uploadstate":"0","userId":0}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HTTP/1.1 200
Server: nginx/1.7.2
Date: Thu, 08 Feb 2018 14:46:04 GMT
Content-Type: application/json;charset=UTF-8
Connection: keep-alive
Content-Length: 84

{"status":"0","message":"合发起成功","data":"966877","timestamp":1518101164012}

*/

/*
6 - 3, 6 - 1, 6 - 0 	(胜平负代码[6] 结果: 3, 1, 0)
1 - 3, 1 - 1, 1 - 0 	(让球胜平负代码[1] 结果: 3, 1, 0)
2 - 0, 2 - 1, 2 - 2 ... (进球总数代码[2] 结果: 0...7)
4 - 0, 4 - 1, 4 - 2 ... (半全场代码[4] 结果[位置] 0...8)
3 - 0, 3 - 1, 3 - 2 ... (比分代码[3] 结果[位置] 0....30) 注意 "*其它"在对应胜平负结果的最低索引
*/