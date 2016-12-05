#pragma once

#include "define.h"



class odbc_func
{
private:

public:
	odbc_func(void);
	~odbc_func(void);

	void DBDisConnect();
	bool DBExcuteSQL(ConnectInfo CI);
	int CheckODBC(SQLHANDLE hstmt );
	int proc_get_nId(SQLCHAR* szName,ConnectInfo CI);
	int proc_account_get_numOfID(SQLCHAR* szID,ConnectInfo CI);
	bool ConnectInfo_InIt(ConnectInfo ci ,SQLCHAR* DNSID,SQLCHAR* ID,SQLCHAR* PW);
};



