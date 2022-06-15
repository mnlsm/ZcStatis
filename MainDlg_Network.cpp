// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "Engine.h"
#include "Global.h"
#include "ZlibStream.h"
#include "BasicExcel.hpp"
#include "MiscHelper.h"


/*
#include "DialogDB.h"
#include "DialogGambel.h"
#include "JQC/JQCDialog.h"
#include "okooo/OkoooDialog.h"
#include "beidan/BeiDanDialog.h"
#include "zucai/ZuCaiDialog.h"
*/

#include <SQLiteCpp/SQLiteCpp.h>

LRESULT CMainDlg::OnAsyncDispatch(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DispathOneAsyncFunc();
	return 1L;
}

bool CMainDlg::AddOneAsyncFunc(talk_base::IAsyncFuncCall* pAsyncFunc) {
	BOOL bRet = CAsyncFuncDispatcher::AddOneAsyncFunc(pAsyncFunc);
	if (bRet) {
		bRet = PostMessage(CMainDlg::WM_ASYNC_DISPATCH, 0, 0);
	}
	return (bRet == TRUE);
}

void CMainDlg::OnHttpReturnGlobal(const CHttpRequestPtr& request,
	const CHttpResponseDataPtr& response) {
	if (sInst.get() != nullptr) {
		sInst->AddOneAsyncFunc(talk_base::BindAsyncFunc(&CMainDlg::OnHttpReturn, sInst.get(), request, response));
	}
}

CHttpRequestPtr CMainDlg::CreatePostRequest(const std::string& url, const std::string& idprefix,
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
	ptr->callback = &CMainDlg::OnHttpReturnGlobal;
	//httpMgr_->DoHttpCommandRequest(ptr);
	return ptr;
}

CHttpRequestPtr CMainDlg::CreateGetRequest(const std::string& url, const std::string& idprefix) {
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
	ptr->callback = &CMainDlg::OnHttpReturnGlobal;
	return ptr;
	//httpMgr_->DoHttpCommandRequest(ptr);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const char* ODDSMAIN_REQ_PREFIX = "oddsmain_req_prefix";
static const char* ODDSITEM_REQ_PREFIX = "oddsitem_req_prefix";

static const char* KAIJIANG14_REQ_PREFIX = "kiaijiang14_req_prefix";
static const char* KAIJIANG9_REQ_PREFIX = "kiaijiang9_req_prefix";


static const char* JCMATCHLIST_REQ_PREFIX = "jcmatchlist_req_prefix";
static const char* ZCPL_REQ_PREFIX = "zcpl_req_prefix";

static const char* BDDICT_REQ_PREFIX = "bddict_req_prefix";
static const char* BDDICT_DOWNLOAD_REQ_PREFIX = "bddict_download_req_prefix";



void CMainDlg::OnHttpReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	
	if (request->request_id.find(ODDSMAIN_REQ_PREFIX) == 0) {
		OnOddsMainReturn(request, response);
	} else if (request->request_id.find(ODDSITEM_REQ_PREFIX) == 0) {
		OnOddsItemReturn(request, response);
	} else if (request->request_id.find(KAIJIANG14_REQ_PREFIX) == 0) {
		OnKaiJiang14Return(request, response);
	} else if (request->request_id.find(KAIJIANG9_REQ_PREFIX) == 0) {
		OnKaiJiang9Return(request, response);
	} else if (request->request_id.find(BDDICT_REQ_PREFIX) == 0) {
		OnBdDictReturn(request, response);
	} else if (request->request_id.find(BDDICT_DOWNLOAD_REQ_PREFIX) == 0) {
		OnBdDictDownloadReturn(request, response);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnBdDictReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
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
		int index = 1;
		CStringA temp = CW2A(CA2W(raw_response.c_str(), acp_code).m_psz).m_psz;
		int nFindBegin = 0, nFindEnd = -1;
		CStringA section_begin = "class=\"wordsliblistname\"><a href=\"";
		CStringA section_end = "\"";
		do{
			nFindBegin = temp.Find(section_begin, nFindBegin);
			if (nFindBegin == -1) {
				break;
			}
			nFindBegin += section_begin.GetLength();
			nFindEnd = temp.Find(section_end, nFindBegin);
			if (nFindEnd == -1) {
				break;
			}
			CStringA download_uri = temp.Mid(nFindBegin, nFindEnd - nFindBegin );
			nFindBegin = nFindEnd;
			CStringATL url;
			url.Format(_T("https://mime.baidu.com%s"), download_uri);
			CHttpRequestPtr req = CreateGetRequest((LPCSTR)url, BDDICT_DOWNLOAD_REQ_PREFIX);
			req->cmd = std::to_string(atoi(request->cmd.c_str()) * 1000 + index) + ".bcd";
			httpMgr_->DoHttpCommandRequest(req);
			index++;
		}while (true);
	}
	return;
}

void CMainDlg::OnBdDictDownloadReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		CStlString strFilePath = Global::GetAppPath() + request->cmd;
		Global::SaveFileData(strFilePath, response->response_content, FALSE);
	}
	if (httpMgr_->GetRequestCount() <= 0) {
		Sleep(20);
	}
	return;
}

void CMainDlg::doRequestNetData() {
	CStringATL url;
	/*
	CStlString root_path = Global::GetAppPath() + "temp";
	std::vector<CStringATL> dirs, files;
	CMiscHelper::ListFiles(root_path.c_str(), dirs, files);
	for (auto& f : files) {
		CStlString file, file_data, utf8_data;
		file = root_path + "\\" + (LPCSTR)f;
		Global::ReadFileData(file, file_data);
		utf8_data = CW2A(CA2W(file_data.c_str(), CP_ACP).m_psz, CP_UTF8);
		Global::SaveFileData(file, utf8_data, FALSE);
	}
	return;
	for (size_t i = 1; i <= 7; i++) {
		url.Format(_T("https://mime.baidu.com/web/iw/c/null/download_number/page:%u"), i);
		CHttpRequestPtr request = CreateGetRequest((LPCSTR)url, BDDICT_REQ_PREFIX);
		request->cmd = std::to_string(i);
		httpMgr_->DoHttpCommandRequest(request);
	}
	return;
	CStlString exe = Global::GetAppPath() + "dict_cvt.exe";
	CStlString input_path = Global::GetAppPath() + "bcd";
	CStlString output_path = Global::GetAppPath() + "temp";
	for (size_t i = 1; i <= 7; i++) {
		for (size_t j = 1; j <= 20; j++) {
			std::string fi = input_path + "\\" + std::to_string(i * 1000 + j) + ".bcd";
			std::string fo = output_path + "\\" + std::to_string(i * 1000 + j) + ".txt";
			if (CMiscHelper::IsFileExist(fi.c_str())) {
				CStringATL cmd, result;
				cmd.Append("\"");
				cmd.Append(exe.c_str());
				cmd.Append("\" ");
				cmd.Append("-i:bcd");
				cmd.Append(" ");
				cmd.Append(fi.c_str());
				cmd.Append(" ");
				cmd.Append("-o:sgpy");
				cmd.Append(" ");
				cmd.Append(fo.c_str());
				CMiscHelper::ExecConsoleCmd(cmd, result, INFINITE);
			}
		}
	}
	return;
	*/
	url.Format(_T("https://odds.500.com/europe_sfc_%s.shtml"), m_strMinQH);
	CHttpRequestPtr request = CreateGetRequest((LPCSTR)url, ODDSMAIN_REQ_PREFIX);
	httpMgr_->DoHttpCommandRequest(request);
}

void CMainDlg::OnOddsMainReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
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
		int nFindBegin = -1, nFindEnd = -1;
		CStringA section_begin = "<select id=\"select-expect\">";
		CStringA section_end = "</select>";
		nFindBegin = temp.Find(section_begin, 0);
		if (nFindBegin == -1) {
			return;
		}
		nFindEnd = temp.Find(section_end, nFindBegin);
		if (nFindEnd == -1) {
			return;
		}
		CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
		if (xmlText.IsEmpty()) {
			//MessageBox("获取对阵列表失败 1！", "错误", MB_ICONERROR | MB_OK);
			return;
		}
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* tempElement = nullptr;
		if (doc.Parse(xmlText) != tinyxml2::XML_SUCCESS) {
			return;
		}
		tinyxml2::XMLElement* child = doc.FirstChildElement();
		if (child == nullptr) {
			return;
		}
		child = child->FirstChildElement("option");
		while (child != nullptr) {
			CStringATL id = GetElementText(child);
			if (atoi(id) > 0) {
				if (net_skip_ids_.find(id) == net_skip_ids_.end()) {
					net_req_ids_.insert(id);
				}
			}
			child = child->NextSiblingElement("option");
		}
		int index = 0;
		pending_request_count_ = net_req_ids_.size();
		for (auto& id : net_req_ids_) {
			std::ostringstream oss;
			oss << "type=europe&lot=sfc&cid=0&expect=" << id;
			CHttpRequestPtr request = CreatePostRequest("https://odds.500.com/excel.php",
				ODDSITEM_REQ_PREFIX, oss.str());
			request->cmd = id;
			request->postdata_content_type = "application/x-www-form-urlencoded";
			request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
			request->request_headers.insert(std::make_pair("Cache-Control", "max-age=0"));
			request->request_headers.insert(std::make_pair("Upgrade-Insecure-Requests", "1"));
			request->request_headers.insert(std::make_pair("Origin", "https://odds.500.com"));
			httpMgr_->DoHttpCommandRequest(request);
			if (((++index) % 10) == 0) {
				Sleep(200);
			}
		}
	}
}


void CMainDlg::OnOddsItemReturn(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	pending_request_count_--;
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		CStlString fn = Global::GetAppPath() + _T("odds\\") + request->cmd + _T(".xls");
		DeleteFileA(fn.c_str());
		Global::SaveFileData(fn, response->response_content, FALSE);
	}
	if (pending_request_count_ <= 0) {
		pending_request_count_ = net_req_ids_.size();
		for (auto& id : net_req_ids_) {
			std::ostringstream oss;
			oss << "gameIndex=401&issueNo=" << id;
			CHttpRequestPtr request = CreatePostRequest("https://kaijiang.ttyingqiu.com/open/historyIssueBySFC.do",
				KAIJIANG14_REQ_PREFIX, oss.str());
			request->cmd = id;
			request->postdata_content_type = "application/x-www-form-urlencoded";
			request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
			request->request_headers.insert(std::make_pair("Cache-Control", "max-age=0"));
			request->request_headers.insert(std::make_pair("Origin", "https://kaijiang.ttyingqiu.com"));
			httpMgr_->DoHttpCommandRequest(request);
		}
	}
}

void CMainDlg::OnKaiJiang14Return(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	pending_request_count_--;
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		CStlString fn = Global::GetAppPath() + _T("odds\\") + request->cmd + _T(".k14");
		DeleteFileA(fn.c_str());
		std::string raw_response;
		if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
			CZlibStream zlib;
			zlib.DecompressGZip(response->response_content, raw_response);
		}
		else {
			raw_response = response->response_content;
		}
		int acp_code = CP_UTF8;
		CStringA temp, jsonText = CW2A(CA2W(raw_response.c_str(), acp_code).m_psz).m_psz;
		CStringATL codes, sales, prize;
		Json::Value rootValue;
		if (ParseJsonString((LPCSTR)jsonText, rootValue) && rootValue.isObject()) {
			std::string vtemp;
			GetStringFromJsonObject(rootValue, "openResult", &vtemp);
			temp = vtemp.c_str();temp.Trim();
			temp.Replace("<i>", "");
			temp.Replace("</i>", "");
			if (temp.GetLength() != 14) {
				return;
			}
			codes = temp;
			GetStringFromJsonObject(rootValue, "saleValue", &vtemp);
			temp = vtemp.c_str();temp.Trim();
			temp.Replace(",", "");
			if (atoi(temp) < 0) {
				temp = "1";
			}
			sales = temp;
			GetStringFromJsonObject(rootValue, "levelInfo", &vtemp);
			temp = vtemp.c_str();temp.Trim();
			int nFindBegin = -1, nFindEnd = -1;
			CStringA section_begin = "<i class='red'>";
			CStringA section_end = "</i>";
			nFindBegin = temp.Find(section_begin, 0);
			if (nFindBegin == -1) {
				return;
			}
			nFindEnd = temp.Find(section_end, nFindBegin);
			if (nFindEnd == -1) {
				return;
			}
			CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
			xmlText.Replace(section_begin, "");
			xmlText.Replace(section_end, "");
			xmlText.Replace(",", "");
			if (atoi(xmlText) < 0) {
				return;
			}
			prize = xmlText;
			if (codes.GetLength() > 0 && sales.GetLength() > 0 && prize.GetLength() > 0) {
				std::string file_content = (codes + "," + sales + "," + prize + "\n");
				Global::SaveFileData(fn, file_content, FALSE);
			}
		}
	}
	if (pending_request_count_ <= 0) {
		pending_request_count_ = net_req_ids_.size();
		for (auto& id : net_req_ids_) {
			std::ostringstream oss;
			oss << "gameIndex=402&issueNo=" << id;
			CHttpRequestPtr request = CreatePostRequest("https://kaijiang.ttyingqiu.com/open/historyIssueBySFC.do",
				KAIJIANG9_REQ_PREFIX, oss.str());
			request->cmd = id;
			request->postdata_content_type = "application/x-www-form-urlencoded";
			request->request_headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
			request->request_headers.insert(std::make_pair("Cache-Control", "max-age=0"));
			request->request_headers.insert(std::make_pair("Origin", "https://kaijiang.ttyingqiu.com"));
			httpMgr_->DoHttpCommandRequest(request);
		}
	}
}

void CMainDlg::OnKaiJiang9Return(const CHttpRequestPtr& request, const CHttpResponseDataPtr& response) {
	pending_request_count_--;
	if (response->httperror == talk_base::HE_NONE && response->response_content.size() > 0) {
		CStlString fn = Global::GetAppPath() + _T("odds\\") + request->cmd + _T(".k9");
		DeleteFileA(fn.c_str());
		std::string raw_response;
		if (response->response_headers.Find("Content-Encoding: gzip") != -1) {
			CZlibStream zlib;
			zlib.DecompressGZip(response->response_content, raw_response);
		}
		else {
			raw_response = response->response_content;
		}
		int acp_code = CP_UTF8;
		CStringA temp, jsonText = CW2A(CA2W(raw_response.c_str(), acp_code).m_psz).m_psz;
		CStringATL codes, sales, prize;
		Json::Value rootValue;
		if (ParseJsonString((LPCSTR)jsonText, rootValue) && rootValue.isObject()) {
			std::string vtemp;
			GetStringFromJsonObject(rootValue, "openResult", &vtemp);
			temp = vtemp.c_str();temp.Trim();
			temp.Replace("<i>", "");
			temp.Replace("</i>", "");
			if (temp.GetLength() != 14) {
				return;
			}
			codes = temp;
			GetStringFromJsonObject(rootValue, "saleValue", &vtemp);
			temp = vtemp.c_str();temp.Trim();
			temp.Replace(",", "");
			if (atoi(temp) < 0) {
				temp = "1";
			}
			sales = temp;
			GetStringFromJsonObject(rootValue, "levelInfo", &vtemp);
			temp = vtemp.c_str();temp.Trim();
			int nFindBegin = -1, nFindEnd = -1;
			CStringA section_begin = "<i class='red'>";
			CStringA section_end = "</i>";
			nFindBegin = temp.Find(section_begin, 0);
			if (nFindBegin == -1) {
				return;
			}
			nFindEnd = temp.Find(section_end, nFindBegin);
			if (nFindEnd == -1) {
				return;
			}
			CStringA xmlText = temp.Mid(nFindBegin, nFindEnd - nFindBegin + section_end.GetLength());
			xmlText.Replace(section_begin, "");
			xmlText.Replace(section_end, "");
			xmlText.Replace(",", "");
			if (atoi(xmlText) < 0) {
				return;
			}
			prize = xmlText;
			if (codes.GetLength() > 0 && sales.GetLength() > 0 && prize.GetLength() > 0) {
				std::string file_content = (codes + "," + sales + "," + prize + "\n");
				Global::SaveFileData(fn, file_content, FALSE);
			}
		}
	}
	if (pending_request_count_ <= 0) {
		for (auto& id : net_req_ids_) {
			CStringATL strMathchs, strOdds, codes;
			long sales = 0;
			double prize_14 = 0.0, prize_9 = 0.0;
			CStlString xls_fn = Global::GetAppPath() + _T("odds\\") + (LPCSTR)id + _T(".xls");
			ReadOddsDataFromFile(xls_fn, strMathchs, strOdds);
			CStlString k14_fn = Global::GetAppPath() + _T("odds\\") + (LPCSTR)id + _T(".k14");
			ReadKaiJiang14FromFile(k14_fn, codes, sales, prize_14);
			CStlString k9_fn = Global::GetAppPath() + _T("odds\\") + (LPCSTR)id + _T(".k9");
			ReadKaiJiang9FromFile(k9_fn, codes, prize_9);
			if (codes.IsEmpty()) {
				codes = "00000000000000";
			}
			if (strMathchs.GetLength() > 0 && strOdds.GetLength() > 0) {
				DeleteFileA(xls_fn.c_str());
				DeleteFileA(k14_fn.c_str());
				DeleteFileA(k9_fn.c_str());

				CStringATL strSQL = _T("DELETE FROM PLDATA WHERE ID = ?");
				SQLite::Statement sm(*m_pDatabase, strSQL);
				sm.bindNoCopy(1, id);
				sm.exec();

				strSQL = _T("INSERT INTO PLDATA (ID, BONUS,RESULT,PLDATA,SALES,MATCHS,BONUS9) values(?,?,?,?,?,?,?)");
				SQLite::Statement sm1(*m_pDatabase, strSQL);
				sm1.bindNoCopy(1, id);
				sm1.bind(2, prize_14);
				sm1.bindNoCopy(3, codes);
				sm1.bindNoCopy(4, strOdds);
				sm1.bind(5, sales);
				sm1.bindNoCopy(6, strMathchs);
				sm1.bind(7, prize_9);
				sm1.exec();
			}
		}
		ReloadStatisData(FALSE);
	}
}

void CMainDlg::ReadOddsDataFromFile(const CStlString& fn, CStringATL& strMathchs, CStringATL& strOdds) {
	strOdds.Empty();
	strMathchs.Empty();
	const std::string dmVS = "    VS    ";
	CStlString rateStr, matchStr;
	using namespace YExcel;
	BasicExcel BE;
	if (!BE.Load(fn.c_str())) {
		return ;
	}
	Worksheet* ws = BE.GetRawWorksheet(0);
	if (ws == NULL) {
		return ;
	}
	const LONGINT maxRowIndex = ws->getMaxRowIndex();
	for (LONGINT row = 2; row < maxRowIndex; row = row + 2) {
		CStlString strMatchPrefix(16, ' ');
		int matchNameState = 0;
		TCHAR szR[20] = { _T('\0') };
		LONGINT matchNo = -1;
		std::string text;

		ws->getLongInt(row, 0, matchNo);
		if (matchNo != -1) {
			_stprintf(szR, _T("%I64d."), matchNo);
			TCHAR* dataPos = (TCHAR*)strMatchPrefix.data();
			memcpy(dataPos, szR, _tcslen(szR) * sizeof(TCHAR));
			matchNameState++;
		}
		else {
			continue;
		}
		ws->getLabel(row, 4, text);
		if (!text.empty()) {
			int teamNameLen = text.size();
			if (teamNameLen < strMatchPrefix.size() - 3) {
				matchNameState++;
				TCHAR* dataPos = (TCHAR*)strMatchPrefix.data() + strMatchPrefix.size() - teamNameLen;
				memcpy(dataPos, text.c_str(), teamNameLen * sizeof(TCHAR));
			}
		}
		ws->getLabel(row, 6, text);
		if (!text.empty()) {
			if (matchNameState == 2) {
				strMatchPrefix = strMatchPrefix + dmVS + text;
			}
			else {
				strMatchPrefix.clear();
			}
			if (!strMatchPrefix.empty()) {
				if (matchStr.empty()) {
					matchStr = strMatchPrefix.c_str();
				}
				else {
					matchStr = matchStr + _T("\n") + strMatchPrefix.c_str();
				}
			}
		}
		for (int j = 7; j <= 9; j++) {
			double rate = 0.0;
			ws->getDouble(row, j, rate);
			if (rate > 0) {
				_stprintf(szR, _T("%.2f"), rate);
				if (rateStr.empty()) {
					rateStr = szR;
				}
				else {
					rateStr = rateStr + _T('#') + szR;
				}
			}
		}
	}
	CStlStrArray arrTemp;
	Global::DepartString(rateStr, _T("#"), arrTemp);
	if (arrTemp.size() == TOTO_COUNT * 3) {
		strOdds = rateStr.c_str();
	}
	Global::DepartString(matchStr, _T("\n"), arrTemp);
	if (arrTemp.size() == TOTO_COUNT) {
		strMathchs = matchStr.c_str();
	}
}


void CMainDlg::ReadKaiJiang14FromFile(const CStlString& fn, CStringATL& codes, long& sales, double& prize) {
	codes.Empty();
	sales = 0;
	prize = 0.0;
	std::string filedate;
	Global::ReadFileData(fn, filedate);
	Global::TrimBlank(filedate);
	std::vector<CStlString> arrParts;
	Global::DepartString(filedate, _T(","), arrParts);
	if (arrParts.size() == 3) {
		codes = arrParts[0].c_str();
		if (codes.GetLength() != 14) {
			codes.Empty();
			return;
		}
		sales = atol(arrParts[1].c_str());
		prize = atof(arrParts[2].c_str()) / 10000;
	}
}

void CMainDlg::ReadKaiJiang9FromFile(const CStlString& fn, const CStringATL& codes, double& prize) {
	prize = 0.0;
	if (codes.IsEmpty()) {
		return;
	}
	std::string filedate;
	Global::ReadFileData(fn, filedate);
	Global::TrimBlank(filedate);
	std::vector<CStlString> arrParts;
	Global::DepartString(filedate, _T(","), arrParts);
	if (arrParts.size() == 3) {
		CStringATL scodes = arrParts[0].c_str();
		if (scodes == codes) {
			prize = atof(arrParts[2].c_str());
		}
	}
}
