// Test_ODBC_2.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

/*
DB에이전트 용도 : 
클라이언트or게임서버가 특정데이터요청이 오면, 대신 DB프로시저 호출해서 결과값을 클라이언트OR게임서버 리턴.

-- 1순위 보안때문에 에이전트를 두고.
-- 2순위 세션수를 줄이기위해..
DB - 게임DB에이전트 - 게임서버
DB - WEB디비에이전트 - 클라이언트

1. recv() 호출.
 패킷읽고
 패킷종류 : 친구리스트 불러오기 / 내정보 불러오기
 내정버불러오기 패킷 : DB_GET_USERINFO | NID
 proc_get_user_info( NID ) 
 내정보 불러오기 프로시저 호출.
*/

#include "stdafx.h"
#include "windows.h"
#include "iostream"
#include "sql.h"
#include "sqlext.h"
//////////////////////////
#include <process.h> //beginthreadex() 함수 사용시 필요 헤더파일
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
	//threadID가 필요한가?00
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

	//마지막으로 작업시작한 쓰레드의 다음 쓰레드 인덱스
	DWORD threadIdx;
}ThreadPool;



DWORD AddWorkToPool(ConnectInfo work);	//작업 등록
ConnectInfo GetWorkFromPool(void);				//작업 가져오기

DWORD MakeThreadToPool();				//쓰레드풀 생성
void WorkerThreadFunction(LPVOID pParam);//쓰레드 함수

void Test_Connection(ConnectInfo* connectInfo); // 쓰레드에서 실행할 함수(실제 연결작업)

void CloseHandle();					//리소스 해제

ThreadPool gThreadPool;			//쓰레드풀 선언
static HANDLE mutex = NULL;	//뮤텍스 선언

void DBDisConnect();
bool DBExcuteSQL(ConnectInfo CI);

bool ConnectInfo_InIt(ConnectInfo ci ,SQLCHAR* DNSID,SQLCHAR* ID,SQLCHAR* PW);

void InitMutex(void)					//뮤텍스 생성
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

void DeInitMutex(void)				//뮤텍스 해제
{
	BOOL ret = CloseHandle(mutex);
}

void AcquireMutex(void)			//뮤텍스 동기화
{
	DWORD ret = WaitForSingleObject(mutex,INFINITE);

	if(ret ==WAIT_FAILED)
		printf("Error AcquireMutex! \n");
}

void ReleaseMutex(void)			//뮤텍스 반환
{
	BOOL ret = ReleaseMutex(mutex);

	if(ret == 0)
		printf("Error ReleaseMutex! \n");
}

DWORD AddWorkToPool(ConnectInfo work) //작업 등록
{
	BOOL bRun = FALSE;
	while(!bRun)				//작업이 쓰레드에 실행될때까지 반복
	{
		AcquireMutex();	//뮤텍스 동기화

		//작업이 최대작업수 초과시 에러메세지 출력 및 중지
		if(gThreadPool.idxOfLastAddedWork >= WORK_MAX)
		{
			printf("AddWorkToPool failed! \n");
			return NULL;
		}

		//작업 등록
		gThreadPool.workList[gThreadPool.idxOfLastAddedWork] = work;

		//작업가능한 쓰레드 검색
		for(DWORD i = gThreadPool.threadIdx;i<THREAD_MAX; i++)
		{
			if(gThreadPool.workerThreadList[i].bWork == FALSE)
			{
				//다음 작업으로 작업인덱스 증가
				gThreadPool.idxOfLastAddedWork++;

				//다음 쓰레드부터 자업가능여부를 검색하도록
				//현재 쓰레드의 다음 쓰레드 인덱스 저장
				gThreadPool.threadIdx = (i+1<THREAD_MAX) ? i+1 : 0 ;

				//현제 쓰레드의 이벤트 활성화
				SetEvent(gThreadPool.workerEventList[i]);
				//현제 쓰레드를 작업중으로 변경
				gThreadPool.workerThreadList[i].bWork = TRUE;
				printf("--SetEvent thread: %d--\n\n", i);

				//작업이 쓰레드에 실행되었으므로 반복 중지
				bRun = TRUE;
				break;
			}

		}
		ReleaseMutex();
	}
	return 1;
}

ConnectInfo GetWorkFromPool() //작업 가져오기
{
	ConnectInfo work;
	memset(&work,0,sizeof(ConnectInfo));
	AcquireMutex(); // 뮤텍스 동기화

	//현제 작업인덱스가 등록된 작업 인덱스 범위안에 있지 않을 경우
	//작업이 없는것으로 간주
	if( !(gThreadPool.idxOfCurrentWork<gThreadPool.idxOfLastAddedWork) )
	{
		ReleaseMutex(); //뮤텍스 반환;
		return work;
	}

	//작업 가져오기
	work = gThreadPool.workList[gThreadPool.idxOfCurrentWork++];

	ReleaseMutex();//뮤텍스 반환;
	return work; //가져온 작업 반환
}

DWORD MakeThreadToPool()
{
	InitMutex();				//뮤텍스 생성
	DWORD i;
	//최대 쓰레드 수만큼 쓰레드 및 이벤트 생성
	for(i = 0; i<THREAD_MAX ; i++)
	{
		DWORD idThread;
		HANDLE hThread;

		//이벤트 생성 및 핸들저장
		gThreadPool.workerEventList[i] = CreateEvent(NULL,FALSE,FALSE,NULL);

		//쓰레드 생성
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WorkerThreadFunction,
			(LPVOID)i,0,&idThread);
		printf("--make thread: %d--\n\n", i);

		//쓰레드 핸들 저장
		gThreadPool.workerThreadList[i].hThread = hThread;
		//쓰레드 아이디 저장
		gThreadPool.workerThreadList[i].idThread = idThread;
		//작업중이 아님으로 초기화
		gThreadPool.workerThreadList[i].bWork = FALSE;
	}

	return i;
}

void WorkerThreadFunction(LPVOID pParam)		//쓰레드 함수
{
	ConnectInfo* work;
	//현재 쓰레드의 이벤트 핸들 가져오기
	HANDLE event = gThreadPool.workerEventList[(DWORD)pParam];

	while(1) //무 한 반 복 
	{
		printf("--wait thread: %d--\n\n",(int)pParam);
		//이벤트가 활성화 될때까지 대기
		WaitForSingleObject(event,INFINITE);

		work = &(GetWorkFromPool());				//작업 가져오기

		if(work->DNSID != NULL)							//작업을 성공적으로 가져왔으면
		{
			printf("--run thread: %d \n\n",(int)pParam);
			Test_Connection(work);

			//현제 쓰레드를 작업중이 아님으로 변경
			gThreadPool.workerThreadList[(DWORD)pParam].bWork = FALSE;
		}
	}
}

void Test_Connection(ConnectInfo* connectInfo)
{
	//여기서 연결하는 과정 넣어주고
	
	 connectInfo->retVal = SQLConnect(connectInfo->hDbc, (SQLCHAR*)connectInfo->DNSID, SQL_NTS //DNS NAME
											,(SQLCHAR*)connectInfo->ID // ID
											,SQL_NTS,(SQLCHAR*)connectInfo->PW,SQL_NTS); //PW

	   if(connectInfo->retVal == SQL_SUCCESS || connectInfo->retVal == SQL_SUCCESS_WITH_INFO)
	  {
 			printf("ODBC 데이터 베이스와의 연결 성공.\n");
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

			 wsprintf(Mes , "SQLSTATE:%s, NativeError:%d, 진단정보:%s",(LPCTSTR)SqlState , NativeError, (LPCTSTR)Msg);
			 printf("%s\n",Mes);
		 }
	  }


    connectInfo->retVal = SQLAllocHandle(SQL_HANDLE_STMT, connectInfo->hDbc, &(connectInfo->hStmt));

    if(connectInfo->retVal == SQL_SUCCESS || connectInfo->retVal == SQL_SUCCESS_WITH_INFO)
          printf("ODBC SQL문을 위한 메모리 할당 성공.\n");
     else
          printf("ODBC SQL문을 위한 메모리 할당 실패.\n");


	if(DBExcuteSQL(*connectInfo) ==false)
	{
		printf("Excute Error \n");
	}
	

}


void CloseHandle()	//리소스 해제
{
	BOOL bComplete = FALSE;
	//모든 쓰레드가 작업중이 아닌 경우 작업완료 시점으로 간주
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
		//쓰레드 핸들 해제
		CloseHandle(gThreadPool.workerThreadList[i].hThread);
		//이벤트 핸들 해제
		CloseHandle(gThreadPool.workerEventList[i]);
	}
	DeInitMutex();	//뮤텍스 해제
}

//////////////////////////


//////////////////////////


int CheckODBC(SQLHANDLE hstmt );
int proc_get_nId(SQLCHAR* szName,ConnectInfo CI);



// recv() 
/*
1. select / iocp 모델을 안쓰고 쌩으로 recv( 소켓 )  호출할경우.
 : 해당소켓 송신버퍼에 데이터가 올때까지 블럭.

 2. 소켓에 넌블럭킹 설정
 : 계속 recv()호출

 3. 소켓에 넌블럭킹 설정 + select()
 : for( ) 문 돌려서 전체 확인 후 있는거에 대한 처리~ 
  { 
    select( readset,  )  
	recv()
  }

  4. 소켓에 넌블럭킹 설정 + iocp
*/

int _tmain(int argc, _TCHAR* argv[])
{
	int nRetCode = 0;
	

	//데이터 받기 전에 이거 먼저...
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


