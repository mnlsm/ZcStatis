#include "stdafx.h"
#include "JQCDialog.h"
#include "Global.h"
#include "ZlibStream.h"
#include "MiscHelper.h"

BOOL JQCDialog::OnIdle() {
	int async_count = GetAsyncFuncCount();
	if (async_count > 0) {
		PostMessage(JQCDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	return FALSE;
}

bool JQCDialog::AddOneAsyncFunc(talk_base::IAsyncFuncCall* pAsyncFunc) {
	BOOL bRet = CAsyncFuncDispatcher::AddOneAsyncFunc(pAsyncFunc);
	if (bRet) {
		bRet = PostMessage(JQCDialog::WM_ASYNC_DISPATCH, 0, 0);
	}
	return (bRet == TRUE);
}

LRESULT JQCDialog::OnAsyncDispatch(UINT msg, WPARAM wParam,
		LPARAM lParam, BOOL& bHandled) {
	DispathOneAsyncFunc();
	return 1L;
}

void JQCDialog::OnHttpReturnGlobal(const CHttpRequestPtr& request,
	const CHttpResponseDataPtr& response) {
	
	sInst.AddOneAsyncFunc(talk_base::BindAsyncFunc(&JQCDialog::OnHttpReturn, &sInst, request, response));
}

CHttpRequestPtr JQCDialog::CreateGetRequest(const std::string& url, const std::string& idprefix) {
	std::string request_id = CHttpClientMgr::generateRequestId();
	request_id = idprefix + "_" + request_id;
	CHttpRequestPtr ptr(new (std::nothrow) THttpRequestData());
	ptr->agent = "Dalvik/2.1.0 (Linux; U; Android 6.0; M5s Build/MRA58K)";
	ptr->request_headers.insert(std::make_pair("Connection", "Keep-Alive"));
	ptr->request_headers.insert(std::make_pair("Accept-Encoding", "gzip"));
	ptr->request_headers.insert(std::make_pair("Cache-Control", "no-store"));
	ptr->request_url = url;
	ptr->request_id = request_id;
	ptr->request_type = HTTP_REQUEST_GET;
	//ptr->request_post_data = query->GetCloudPathsRequestData();
	ptr->response_type = RESPONSEMEMORY;
	ptr->request_time = 1000 * 60;
	ptr->callback = &JQCDialog::OnHttpReturnGlobal;
	return ptr;
}






///////////////////////////////////////////////////////////////////////////////////////////
static const char* REQUEST_DONETUPDATERESULTS = "1_req_doNetUpdateResults";
static const char* REQUEST_DONETUPDATERESULTS2 = "2_req_doNetUpdateResults2";


void JQCDialog::OnHttpReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (request->request_id.find(REQUEST_DONETUPDATERESULTS) == 0) {
		OnNetUpdateResultsFirst(request, response);
	} else if (request->request_id.find(REQUEST_DONETUPDATERESULTS2) == 0) {
		OnNetUpdateResultsSecond(request, response);
	}
/*
	else if (request->request_id.find(JCMATCHLIST_REQ_PREFIX) == 0) {
		OnJcMatchListReturn(request, response);
	}

	else if (request->request_id.find(BIFEN_REQ_PREFIX) == 0) {
		OnBiFenReturn(request, response);
	}
*/
}


void JQCDialog::doNetUpdateResults() {
	m_request_result_pos = 0;
	m_request_result_count = 0;
	std::string url = "https://www.310win.com/zucai/4changjinqiucai/kaijiang_zc_4.html";
	CHttpRequestPtr request = CreateGetRequest(url, REQUEST_DONETUPDATERESULTS);
	httpMgr_->DoHttpCommandRequest(request);
	return;
}

void JQCDialog::OnNetUpdateResultsFirst(const CHttpRequestPtr& request,
		const CHttpResponseDataPtr& response) {
	std::map<std::string, std::string> id_pairs;
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		std::string raw_response;
		if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
			CZlibStream zlib;
			zlib.DecompressGZip(response->response_content, raw_response);
		} else {
			raw_response = response->response_content;
		}
		CStringA section_begin = "<div class=\"ste23\">";
		CStringA section_end = "</div>";
		int acp_code = CP_ACP;
		if (raw_response.find("UTF-8") != std::string::npos
			|| raw_response.find("utf-8") != std::string::npos) {
			acp_code = CP_UTF8;
		}
		CStringA temp = CW2A(CA2W(raw_response.c_str(), acp_code).m_psz).m_psz;
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
			return;
		}
		child = child->FirstChildElement();
		while (child != nullptr) {
			if (strcmp("select", child->Name()) == 0) {
				child = child->FirstChildElement();
			}
			if (child != nullptr) {
				if (strcmp("option", child->Name()) == 0) {
					CStringATL sid = GetElementAttrValue(child, "value");
					CStringATL id = GetElementText(child);
					if (!sid.IsEmpty() && !id.IsEmpty()) {
						if (m_localids.find((LPCSTR)id) == m_localids.end()) {
							id_pairs[(LPCSTR)id] = sid;
						}
					}
				}
			}
			child = child->NextSiblingElement();
		}
	}
	m_request_result_pos = 0;
	m_request_result_count = id_pairs.size();
	for (auto& pair : id_pairs) {
		std::string url = "http://www.310win.com/Info/Result/Soccer.aspx?load=ajax&typeID=4&IssueID=";
		url = url + pair.second + "&randomT-_-=0.04642827346238332";
		CHttpRequestPtr request = CreateGetRequest(url, REQUEST_DONETUPDATERESULTS2);
		request->request_headers.insert(std::make_pair("Referer", 
			"https://www.310win.com/zucai/4changjinqiucai/kaijiang_zc_4.html"));
		httpMgr_->DoHttpCommandRequest(request);
	}
}

void JQCDialog::OnNetUpdateResultsSecond(const CHttpRequestPtr& request,
		const CHttpResponseDataPtr& response) {
	m_request_result_pos++;
	CStringATL progress;
	progress.Format("请求结果进度：%d/%d", m_request_result_pos, m_request_result_count);
	m_stInfo.SetWindowText(progress);

	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		std::string raw_response;
		if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
			CZlibStream zlib;
			zlib.DecompressGZip(response->response_content, raw_response);
		} else {
			raw_response = response->response_content;
		}
		CStringA temp = CW2A(CA2W(raw_response.c_str(), CP_UTF8).m_psz, CP_ACP).m_psz;
		std::string qid, codes, bonus, bottom;
		Json::Value jroot, jbonus, jtmp;
		if (!parse_json_string((LPCSTR)temp, jroot)) {
			return;
		}
		json_get_string(jroot, "IssueNum", &qid);
		json_get_string(jroot, "Result", &codes);
		json_get_object(jroot, "Bonus", &jbonus);
		json_get_string(jroot, "Bottom", &bottom);
		if (qid.empty() || codes.empty()) {
			return;
		}
		if (!jbonus.isArray()) {
			return;
		}
		jsonarray_get_object(jbonus, 0, &jtmp);
		if (!jtmp.isObject()) {
			return;
		}
		json_get_string(jtmp, "BasicBonus", &bonus);
		CMiscHelper::string_replace(bonus, ",", "");
		CStringW wtmp , wnum;
		wtmp = CA2W(bonus.c_str(), CP_ACP);
		for (int i = 0; i < wtmp.GetLength(); i++) {
			if (wtmp[i] >= L'0' && wtmp[i] <= L'9') {
				wnum.AppendChar(wtmp[i]);
			}
		}
		float fbonux = _wtoi(wnum) / 10000.0f;
		wtmp = CA2W(bottom.c_str(), CP_ACP);
		wnum.Empty();
		for (int i = 0; i < wtmp.GetLength(); i++) {
			if (wtmp[i] >= L'0' && wtmp[i] <= L'9') {
				wnum.AppendChar(wtmp[i]);
			} else if (wtmp[i] == L'元') {
				break;
			}
		}
		int isale = _wtoi(wnum) / 10000;
		CStringATL line;
		line.Format("%s,%s,%.2f,%d", qid.c_str(), codes.c_str(), fbonux, isale);
		m_localids[qid] = line;
	}
	if (m_request_result_pos >= m_request_result_count) {
		m_stInfo.SetWindowText("");
		CStlString strTextFile = Global::GetAppPath() + _T("jqc.txt");
		DeleteFileA(strTextFile.c_str());
		std::string lines;
		for (auto& item : m_localids) {
			lines.append(item.second).append("\n");
		}
		Global::SaveFileData(strTextFile, lines, FALSE);
		ReloadStatisDataFromLocal(1);
	}

}




