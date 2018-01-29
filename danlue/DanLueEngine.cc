#include "stdafx.h"
#include "DanLueEngine.h"

DanLueEngine::DanLueEngine(const CStlString& script) {
	m_strScript = script;
}

BOOL DanLueEngine::CalculateAllResult(CStlString& failed_reason) {
	return TRUE;
}

void DanLueEngine::GetResultString(CStlString& result) {
	return;
}