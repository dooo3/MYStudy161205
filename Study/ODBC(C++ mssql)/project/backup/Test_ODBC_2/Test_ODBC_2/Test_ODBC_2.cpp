// Test_ODBC_2.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

/*
DB������Ʈ �뵵 : 
Ŭ���̾�Ʈor���Ӽ����� Ư�������Ϳ�û�� ����, ��� DB���ν��� ȣ���ؼ� ������� Ŭ���̾�ƮOR���Ӽ��� ����.

-- 1���� ���ȶ����� ������Ʈ�� �ΰ�.
-- 2���� ���Ǽ��� ���̱�����..
DB - ����DB������Ʈ - ���Ӽ���
DB - WEB�������Ʈ - Ŭ���̾�Ʈ

1. recv() ȣ��.
 ��Ŷ�а�
 ��Ŷ���� : ģ������Ʈ �ҷ����� / ������ �ҷ�����
 �������ҷ����� ��Ŷ : DB_GET_USERINFO | NID
 proc_get_user_info( NID ) 
 ������ �ҷ����� ���ν��� ȣ��.
*/

#include "stdafx.h"
#include "windows.h"
#include "iostream"
#include "sql.h"
#include "sqlext.h"
//////////////////////////
#include <process.h> //beginthreadex() �Լ� ���� �ʿ� �������
//////////////////////////


using namespace std;


//////////////////////////
// SQL ODBC Conn
//////////////////////////



//////////////////////////
//////////////////////////
//2016-11-15

#define WORK_MAX 8
#define THREAD_MAX 10

#define DEFAULT_BUFLEN 500



typedef struct _ConnectInfo
{
	SQLCHAR* DNSID; //DNSID
	SQLCHAR* ID;//ID
	SQLCHAR* PW;//PW

	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;

	SQLRETURN retVal;
	//threadID�� �ʿ��Ѱ�?00
}ConnectInfo;

typedef struct _WorkerThread
{
	HANDLE hThread;
	DWORD idThread;
	BOOL bWork;
}WorkerThread;

typedef struct _ThreadPool
{
	WorkerThread workerThreadList[THREAD_MAX];
	HANDLE workerEventList[THREAD_MAX];

	ConnectInfo workList[WORK_MAX];

	DWORD idxOfCurrentWork;
	DWORD idxOfLastAddedWork;

	//���������� �۾������� �������� ���� ������ �ε���
	DWORD threadIdx;
}ThreadPool;



DWORD AddWorkToPool(ConnectInfo work);	//�۾� ���
ConnectInfo GetWorkFromPool(void);				//�۾� ��������

DWORD MakeThreadToPool();				//������Ǯ ����
void WorkerThreadFunction(LPVOID pParam);//������ �Լ�

void Test_Connection(ConnectInfo* connectInfo); // �����忡�� ������ �Լ�(���� �����۾�)

void CloseHandle();					//���ҽ� ����

ThreadPool gThreadPool;			//������Ǯ ����
static HANDLE mutex = NULL;	//���ؽ� ����

void DBDisConnect();
bool DBExcuteSQL(ConnectInfo CI);

bool ConnectInfo_InIt(ConnectInfo ci ,SQLCHAR* DNSID,SQLCHAR* ID,SQLCHAR* PW);

void InitMutex(void)					//���ؽ� ����
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

void DeInitMutex(void)				//���ؽ� ����
{
	BOOL ret = CloseHandle(mutex);
}

void AcquireMutex(void)			//���ؽ� ����ȭ
{
	DWORD ret = WaitForSingleObject(mutex,INFINITE);

	if(ret ==WAIT_FAILED)
		printf("Error AcquireMutex! \n");
}

void ReleaseMutex(void)			//���ؽ� ��ȯ
{
	BOOL ret = ReleaseMutex(mutex);

	if(ret == 0)
		printf("Error ReleaseMutex! \n");
}

DWORD AddWorkToPool(ConnectInfo work) //�۾� ���
{
	BOOL bRun = FALSE;
	while(!bRun)				//�۾��� �����忡 ����ɶ����� �ݺ�
	{
		AcquireMutex();	//���ؽ� ����ȭ

		//�۾��� �ִ��۾��� �ʰ��� �����޼��� ��� �� ����
		if(gThreadPool.idxOfLastAddedWork >= WORK_MAX)
		{
			printf("AddWorkToPool failed! \n");
			return NULL;
		}

		//�۾� ���
		gThreadPool.workList[gThreadPool.idxOfLastAddedWork] = work;

		//�۾������� ������ �˻�
		for(DWORD i = gThreadPool.threadIdx;i<THREAD_MAX; i++)
		{
			if(gThreadPool.workerThreadList[i].bWork == FALSE)
			{
				//���� �۾����� �۾��ε��� ����
				gThreadPool.idxOfLastAddedWork++;

				//���� ��������� �ھ����ɿ��θ� �˻��ϵ���
				//���� �������� ���� ������ �ε��� ����
				gThreadPool.threadIdx = (i+1<THREAD_MAX) ? i+1 : 0 ;

				//���� �������� �̺�Ʈ Ȱ��ȭ
				SetEvent(gThreadPool.workerEventList[i]);
				//���� �����带 �۾������� ����
				gThreadPool.workerThreadList[i].bWork = TRUE;
				printf("--SetEvent thread: %d--\n\n", i);

				//�۾��� �����忡 ����Ǿ����Ƿ� �ݺ� ����
				bRun = TRUE;
				break;
			}

		}
		ReleaseMutex();
	}
	return 1;
}

ConnectInfo GetWorkFromPool() //�۾� ��������
{
	ConnectInfo work;
	memset(&work,0,sizeof(ConnectInfo));
	AcquireMutex(); // ���ؽ� ����ȭ

	//���� �۾��ε����� ��ϵ� �۾� �ε��� �����ȿ� ���� ���� ���
	//�۾��� ���°����� ����
	if( !(gThreadPool.idxOfCurrentWork<gThreadPool.idxOfLastAddedWork) )
	{
		ReleaseMutex(); //���ؽ� ��ȯ;
		return work;
	}

	//�۾� ��������
	work = gThreadPool.workList[gThreadPool.idxOfCurrentWork++];

	ReleaseMutex();//���ؽ� ��ȯ;
	return work; //������ �۾� ��ȯ
}

DWORD MakeThreadToPool()
{
	InitMutex();				//���ؽ� ����
	DWORD i;
	//�ִ� ������ ����ŭ ������ �� �̺�Ʈ ����
	for(i = 0; i<THREAD_MAX ; i++)
	{
		DWORD idThread;
		HANDLE hThread;

		//�̺�Ʈ ���� �� �ڵ�����
		gThreadPool.workerEventList[i] = CreateEvent(NULL,FALSE,FALSE,NULL);

		//������ ����
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WorkerThreadFunction,
			(LPVOID)i,0,&idThread);
		printf("--make thread: %d--\n\n", i);

		//������ �ڵ� ����
		gThreadPool.workerThreadList[i].hThread = hThread;
		//������ ���̵� ����
		gThreadPool.workerThreadList[i].idThread = idThread;
		//�۾����� �ƴ����� �ʱ�ȭ
		gThreadPool.workerThreadList[i].bWork = FALSE;
	}

	return i;
}

void WorkerThreadFunction(LPVOID pParam)		//������ �Լ�
{
	ConnectInfo* work;
	//���� �������� �̺�Ʈ �ڵ� ��������
	HANDLE event = gThreadPool.workerEventList[(DWORD)pParam];

	while(1) //�� �� �� �� 
	{
		printf("--wait thread: %d--\n\n",(int)pParam);
		//�̺�Ʈ�� Ȱ��ȭ �ɶ����� ���
		WaitForSingleObject(event,INFINITE);

		work = &(GetWorkFromPool());				//�۾� ��������

		if(work->DNSID != NULL)							//�۾��� ���������� ����������
		{
			printf("--run thread: %d \n\n",(int)pParam);
			Test_Connection(work);

			//���� �����带 �۾����� �ƴ����� ����
			gThreadPool.workerThreadList[(DWORD)pParam].bWork = FALSE;
		}
	}
}

void Test_Connection(ConnectInfo* connectInfo)
{
	//���⼭ �����ϴ� ���� �־��ְ�
	
	 connectInfo->retVal = SQLConnect(connectInfo->hDbc, (SQLCHAR*)connectInfo->DNSID, SQL_NTS //DNS NAME
											,(SQLCHAR*)connectInfo->ID // ID
											,SQL_NTS,(SQLCHAR*)connectInfo->PW,SQL_NTS); //PW

	   if(connectInfo->retVal == SQL_SUCCESS || connectInfo->retVal == SQL_SUCCESS_WITH_INFO)
	  {
 			printf("ODBC ������ ���̽����� ���� ����.\n");
	  }else
	 {
		 SQLINTEGER NativeError;
		 SQLSMALLINT MsgLen;
		 SQLCHAR SqlState[6];
		 SQLCHAR Msg[255];
		 TCHAR Mes[1024];

		 for ( int iDiag = 1;; iDiag++ ){
			 connectInfo->retVal = SQLGetDiagRec( SQL_HANDLE_DBC, connectInfo->hDbc, iDiag , SqlState,
									&NativeError,Msg,sizeof(Msg),&MsgLen);

			 if(connectInfo->retVal == SQL_NO_DATA)
				 break;

			 wsprintf(Mes , "SQLSTATE:%s, NativeError:%d, ��������:%s",(LPCTSTR)SqlState , NativeError, (LPCTSTR)Msg);
			 printf("%s\n",Mes);
		 }
	  }


    connectInfo->retVal = SQLAllocHandle(SQL_HANDLE_STMT, connectInfo->hDbc, &(connectInfo->hStmt));

    if(connectInfo->retVal == SQL_SUCCESS || connectInfo->retVal == SQL_SUCCESS_WITH_INFO)
          printf("ODBC SQL���� ���� �޸� �Ҵ� ����.\n");
     else
          printf("ODBC SQL���� ���� �޸� �Ҵ� ����.\n");


	if(DBExcuteSQL(*connectInfo) ==false)
	{
		printf("Excute Error \n");
	}
	

}


void CloseHandle()	//���ҽ� ����
{
	BOOL bComplete = FALSE;
	//��� �����尡 �۾����� �ƴ� ��� �۾��Ϸ� �������� ����
	while(!bComplete)
	{
		for(int i = 0;i<THREAD_MAX;i++)
		{
			if(gThreadPool.workerThreadList[i].bWork == TRUE)
			{
				bComplete = FALSE;
				break;
			}
			bComplete = TRUE;
		}
		Sleep(1000);
	}

	for(int i = 0;i<THREAD_MAX ; i++)
	{
		//������ �ڵ� ����
		CloseHandle(gThreadPool.workerThreadList[i].hThread);
		//�̺�Ʈ �ڵ� ����
		CloseHandle(gThreadPool.workerEventList[i]);
	}
	DeInitMutex();	//���ؽ� ����
}

//////////////////////////


//////////////////////////


int CheckODBC(SQLHANDLE hstmt );
int proc_get_nId(SQLCHAR* szName,ConnectInfo CI);



// recv() 
/*
1. select / iocp ���� �Ⱦ��� ������ recv( ���� )  ȣ���Ұ��.
 : �ش���� �۽Ź��ۿ� �����Ͱ� �ö����� ��.

 2. ���Ͽ� �ͺ�ŷ ����
 : ��� recv()ȣ��

 3. ���Ͽ� �ͺ�ŷ ���� + select()
 : for( ) �� ������ ��ü Ȯ�� �� �ִ°ſ� ���� ó��~ 
  { 
    select( readset,  )  
	recv()
  }

  4. ���Ͽ� �ͺ�ŷ ���� + iocp
*/

int _tmain(int argc, _TCHAR* argv[])
{
	int nRetCode = 0;
	

	//������ �ޱ� ���� �̰� ����...
	MakeThreadToPool();
					/*
						for(int i =0;i<THREAD_MAX;i++)
						{
							if(i  == 0)
							{
								ConnectInfo Ci1;
								memset(&Ci1,0,sizeof(ConnectInfo));
								ConnectInfo_InIt(Ci1,(SQLCHAR*)"Test_Rma",(SQLCHAR*)"sa",(SQLCHAR*)"tjfgusWk@8949!");
							}
							if(i == 1)
							{
								ConnectInfo Ci2;
								memset(&Ci2,0,sizeof(ConnectInfo));
								ConnectInfo_InIt(Ci2,(SQLCHAR*)"Test_Rma2",(SQLCHAR*)"sa",(SQLCHAR*)"tjfgusWk@8949!");
							}
						}
					*/
	ConnectInfo Ci1;
	memset(&Ci1,0,sizeof(ConnectInfo));
	ConnectInfo_InIt(Ci1,(SQLCHAR*)"Test_Rma",(SQLCHAR*)"sa",(SQLCHAR*)"tjfgusWk@8949!");

	ConnectInfo Ci2;
	memset(&Ci2,0,sizeof(ConnectInfo));
	ConnectInfo_InIt(Ci2,(SQLCHAR*)"Test_Rma2",(SQLCHAR*)"sa",(SQLCHAR*)"tjfgusWk@8949!");

	ConnectInfo Ci3;
	memset(&Ci3,0,sizeof(ConnectInfo));
	ConnectInfo_InIt(Ci3,(SQLCHAR*)"Test_Rma3",(SQLCHAR*)"sa",(SQLCHAR*)"tjfgusWk@8949!");

	CloseHandle();

	return nRetCode;

}

/*
void DBDisConnect()
{
	if(hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	if(hDbc) SQLDisconnect(hDbc);
	if(hDbc) SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if(hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}
*/


int proc_get_nId(SQLCHAR* szName,ConnectInfo CI)
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



int proc_account_get_numOfID(SQLCHAR* szID,ConnectInfo CI)
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



int CheckODBC(  SQLHANDLE hstmt ) 
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



bool DBExcuteSQL(ConnectInfo CI)
{

	if(CI.DNSID == (SQLCHAR*)"Test_Rma")
	{
		proc_get_nId((SQLCHAR*)"�̳�",CI);
			return true;
	}else if(CI.DNSID == (SQLCHAR*)"Test_Rma2")
	{
		proc_get_nId((SQLCHAR*)"�̸�",CI); 
			return true;
	}else if(CI.DNSID ==(SQLCHAR*)"Test_Rma3")
	{
		proc_account_get_numOfID((SQLCHAR*)"abcd",CI);
			return true;
	}

	return false;
}




bool ConnectInfo_InIt(ConnectInfo ci, SQLCHAR* DNSID ,SQLCHAR* ID ,SQLCHAR* PW)
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

	AddWorkToPool(ci);

	return true;
}


