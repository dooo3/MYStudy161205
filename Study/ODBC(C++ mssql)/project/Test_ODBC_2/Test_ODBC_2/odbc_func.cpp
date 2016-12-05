#pragma once
#include "odbc_func.h"
#include "thread_pool.h"

thread_pool thread_Pool;

odbc_func::odbc_func(void)
{
}

odbc_func::~odbc_func(void)
{
}

/*
void DBDisConnect();


int CheckODBC(SQLHANDLE hstmt );
int proc_get_nId(SQLCHAR* szName,ConnectInfo CI);
int proc_account_get_numOfID(SQLCHAR* szID,ConnectInfo CI);
bool DBExcuteSQL(ConnectInfo CI);
bool ConnectInfo_InIt(ConnectInfo ci ,SQLCHAR* DNSID,SQLCHAR* ID,SQLCHAR* PW);
*/

int odbc_func::CheckODBC(SQLHANDLE hstmt)
{
	SQLCHAR		SqlState[6], Msg[512]; 
	SQLINTEGER	NativeError; 
	SQLSMALLINT	i=1, MsgLen; 
	SQLRETURN	rc2; 
 
	while(( rc2 = SQLGetDiagRec( SQL_HANDLE_STMT, hstmt, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen )) != SQL_NO_DATA ) 
	{ 
		if( rc2 == SQL_INVALID_HANDLE) { 
			printf("no alloc handle\n");
			return 0; 
		}
 
		printf("SQL ERROR State: %s, Native: %d, Msg:%s\r\n", SqlState, NativeError, Msg ); 
		 
		if( strcmp( (char*)SqlState, "08S01") == 0 ) 
		{ 
			printf( "SQL ERROR State: %s, Native: %d, Msg:%s\r\n", SqlState, NativeError, Msg ); 
			return 1; 
		} 
		 
		i++; 
	} 
	return 0; 
}

int odbc_func::proc_get_nId(SQLCHAR* szName,ConnectInfo CI)
{
	int nId;
	SDWORD cbParam = SQL_NTS;
	SQLRETURN retval;
	
	SQLBindParameter(CI.hStmt, 1, SQL_PARAM_INPUT		, SQL_C_CHAR, SQL_CHAR			, sizeof( szName ) + 1	, 0	, szName	, sizeof(szName)	, &cbParam);//input szname
	SQLBindParameter(CI.hStmt, 2, SQL_PARAM_OUTPUT	, SQL_C_LONG, SQL_INTEGER		, 0								, 0	, &nId	, 0						, &cbParam);  //output nId
	retval = SQLExecDirect(CI.hStmt, (SQLCHAR *)"{ CALL proc_test_op (?, ?) }", SQL_NTS);

	CheckODBC(CI.hStmt);

	printf("retval->%d \n",retval);
	printf("nId->%d \n",nId);

	return nId;
}

int odbc_func::proc_account_get_numOfID(SQLCHAR* szID,ConnectInfo CI)
{
	int nNumofAccount =0;
	SDWORD cbParam = SQL_NTS;
	SQLRETURN retval;
	
	SQLBindParameter(CI.hStmt, 1, SQL_PARAM_INPUT		, SQL_C_CHAR, SQL_CHAR			, sizeof( szID ) + 1	, 0	, szID	, sizeof(szID)	, &cbParam);//input szname
	SQLBindParameter(CI.hStmt, 2, SQL_PARAM_OUTPUT	, SQL_C_LONG, SQL_INTEGER		, 0								, 0	, &nNumofAccount	, 0						, &cbParam);  //output nId
	retval = SQLExecDirect(CI.hStmt, (SQLCHAR *)"{ CALL proc_account_get_numOfID (?, ?) }", SQL_NTS);

	CheckODBC(CI.hStmt);

	printf("retval->%d \n",retval);
	printf("nNumofAccount->%d \n",nNumofAccount);

	return nNumofAccount;
}
bool odbc_func::DBExcuteSQL(ConnectInfo CI)
{
	if(CI.DNSID == (SQLCHAR*)"Test_Rma")
	{
		proc_get_nId((SQLCHAR*)"미노",CI);
			return true;
	}else if(CI.DNSID == (SQLCHAR*)"Test_Rma2")
	{
		proc_get_nId((SQLCHAR*)"이름",CI); 
			return true;
	}else if(CI.DNSID ==(SQLCHAR*)"Test_Rma3")
	{
		proc_account_get_numOfID((SQLCHAR*)"abcd",CI);
			return true;
	}

	return false;
}

bool odbc_func::ConnectInfo_InIt(ConnectInfo ci, SQLCHAR* DNSID ,SQLCHAR* ID ,SQLCHAR* PW)
{

	if(ci.DNSID != NULL)//처음으로 추가하는 것일때만 AddWorkToPool; 
	{
		if(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &(ci.hEnv)) != SQL_SUCCESS)
			return false; 
		if(SQLSetEnvAttr(ci.hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
			return false;
		if(SQLAllocHandle(SQL_HANDLE_DBC, ci.hEnv, &(ci.hDbc)) != SQL_SUCCESS)
			return false;

		//return true;//false 거나 
		
	}else //새로운 작업만들기
	{
		ci.DNSID = DNSID;
		ci.ID = ID;
		ci.PW = PW;

		if(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &(ci.hEnv)) != SQL_SUCCESS)
			return false; 
		if(SQLSetEnvAttr(ci.hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
			return false;
		if(SQLAllocHandle(SQL_HANDLE_DBC, ci.hEnv, &(ci.hDbc)) != SQL_SUCCESS)
			return false;

		thread_Pool.AddWorkToPool(ci);
	}

	return true;
}


