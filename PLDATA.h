// PLDATA.h : CPLDATA 的声明

#pragma once

// 代码生成在 2005年12月19日, 0:54

[
	#error Security Issue: The connection string may contain a password
// 此连接字符串中可能包含密码
// 下面的连接字符串中可能包含明文密码和/或
// 其他重要信息。请在查看完
// 此连接字符串并找到所有与安全有关的问题后移除 #error。可能需要
// 将此密码存储为其他格式或使用其他的用户身份验证。
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
// 该表/命令包含可以通过 ISequentialStream 接口访问
// 的列。然而，并非所有提供程序都支持此功能，即使
// 那些确实支持此功能的提供程序通常也仅限为每个行集
// 只有一个 ISequentialStream。
// 如果希望在此访问器中使用流，请使用以下代码的示例
// 行，并将 DBPROP_ISequentialStream 行集属性设置为
// “真”。然后就可以使用 Read() 方法读取数据。有关
// ISequentialStream 绑定的详细信息，请参见文档

	// 为解决某些提供程序的若干问题，以下代码可能以
	// 不同于提供程序所报告的顺序来绑定列

	[ db_column(1, status=m_dwBONUSStatus, length=m_dwBONUSLength) ] double m_BONUS;
	[ db_column(2, status=m_dwIDStatus, length=m_dwIDLength) ] TCHAR m_ID[201];
	// [ db_column(3, status=m_dwPLDATAStatus, length=m_dwPLDATALength) ] ISequentialStream* m_PLDATA;
	[ db_column(3, status=m_dwPLDATAStatus, length=m_dwPLDATALength) ] TCHAR m_PLDATA[8000];
	[ db_column(4, status=m_dwRESULTStatus, length=m_dwRESULTLength) ] TCHAR m_RESULT[201];

	// 以下向导生成的数据成员包含相应字段的状态值。
	// 可以使用这些值保存数据库返回的 NULL 值或在编译器返回
	// 错误时保存错误信息。有关如何使用这些字段的详细信息，
	// 请参见 Visual C++ 文档中的“向导生成的访问器中的字段状态数据成员”。
	// 注意：在设置/插入数据前必须初始化这些字段！

	DBSTATUS m_dwBONUSStatus;
	DBSTATUS m_dwIDStatus;
	DBSTATUS m_dwPLDATAStatus;
	DBSTATUS m_dwRESULTStatus;

	// 以下向导生成的数据成员包含相应字段的长度值。
	// 注意：对于变长列，在设置/插入数据前必须初始化这些字段！

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


