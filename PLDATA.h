// PLDATA.h : CPLDATA ������

#pragma once

// ���������� 2005��12��19��, 0:54

[
	#error Security Issue: The connection string may contain a password
// �������ַ����п��ܰ�������
// ����������ַ����п��ܰ������������/��
// ������Ҫ��Ϣ�����ڲ鿴��
// �������ַ������ҵ������밲ȫ�йص�������Ƴ� #error��������Ҫ
// ��������洢Ϊ������ʽ��ʹ���������û������֤��
db_source(L"Provider=Microsoft.Jet.OLEDB.4.0;User ID=Admin;Data Source=C:\\Documents and Settings\\Administrator\\\x684c\x9762\\ZcStatis.mdb;Mode=Share Deny None;Extended Properties=\"\";Jet OLEDB:System database=\"\";Jet OLEDB:Registry Path=\"\";Jet OLEDB:Database Password=\"\";Jet OLEDB:Engine Type=5;Jet OLEDB:Database Locking Mode=1;Jet OLEDB:Global Partial Bulk Ops=2;Jet OLEDB:Global Bulk Transactions=1;Jet OLEDB:New Database Password=\"\";Jet OLEDB:Create System Database=False;Jet OLEDB:Encrypt Database=False;Jet OLEDB:Don't Copy Locale on Compact=False;Jet OLEDB:Compact Without Replica Repair=False;Jet OLEDB:SFP=False"),
	db_command(L" \
	SELECT \
		BONUS, \
		ID, \
		PLDATA, \
		RESULT \
		FROM PLDATA")
]
class CPLDATA
{
public:
// �ñ�/�����������ͨ�� ISequentialStream �ӿڷ���
// ���С�Ȼ�������������ṩ����֧�ִ˹��ܣ���ʹ
// ��Щȷʵ֧�ִ˹��ܵ��ṩ����ͨ��Ҳ����Ϊÿ���м�
// ֻ��һ�� ISequentialStream��
// ���ϣ���ڴ˷�������ʹ��������ʹ�����´����ʾ��
// �У����� DBPROP_ISequentialStream �м���������Ϊ
// ���桱��Ȼ��Ϳ���ʹ�� Read() ������ȡ���ݡ��й�
// ISequentialStream �󶨵���ϸ��Ϣ����μ��ĵ�

	// Ϊ���ĳЩ�ṩ������������⣬���´��������
	// ��ͬ���ṩ�����������˳��������

	[ db_column(1, status=m_dwBONUSStatus, length=m_dwBONUSLength) ] double m_BONUS;
	[ db_column(2, status=m_dwIDStatus, length=m_dwIDLength) ] TCHAR m_ID[201];
	// [ db_column(3, status=m_dwPLDATAStatus, length=m_dwPLDATALength) ] ISequentialStream* m_PLDATA;
	[ db_column(3, status=m_dwPLDATAStatus, length=m_dwPLDATALength) ] TCHAR m_PLDATA[8000];
	[ db_column(4, status=m_dwRESULTStatus, length=m_dwRESULTLength) ] TCHAR m_RESULT[201];

	// ���������ɵ����ݳ�Ա������Ӧ�ֶε�״ֵ̬��
	// ����ʹ����Щֵ�������ݿⷵ�ص� NULL ֵ���ڱ���������
	// ����ʱ���������Ϣ���й����ʹ����Щ�ֶε���ϸ��Ϣ��
	// ��μ� Visual C++ �ĵ��еġ������ɵķ������е��ֶ�״̬���ݳ�Ա����
	// ע�⣺������/��������ǰ�����ʼ����Щ�ֶΣ�

	DBSTATUS m_dwBONUSStatus;
	DBSTATUS m_dwIDStatus;
	DBSTATUS m_dwPLDATAStatus;
	DBSTATUS m_dwRESULTStatus;

	// ���������ɵ����ݳ�Ա������Ӧ�ֶεĳ���ֵ��
	// ע�⣺���ڱ䳤�У�������/��������ǰ�����ʼ����Щ�ֶΣ�

	DBLENGTH m_dwBONUSLength;
	DBLENGTH m_dwIDLength;
	DBLENGTH m_dwPLDATALength;
	DBLENGTH m_dwRESULTLength;


	void GetRowsetProperties(CDBPropSet* pPropSet)
	{
		pPropSet->AddProperty(DBPROP_CANFETCHBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
		pPropSet->AddProperty(DBPROP_CANSCROLLBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
		// pPropSet->AddProperty(DBPROP_ISequentialStream, true);
	}
};


