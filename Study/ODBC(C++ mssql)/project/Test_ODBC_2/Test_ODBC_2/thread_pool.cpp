
#include "thread_pool.h"
#include "odbc_func.h"
#include "define.h"


static odbc_func g_odbc_Func;

using namespace std;

thread_pool::thread_pool(void)
{

}

thread_pool::~thread_pool(void)
{

}




/*
void InitMutex(void);
void DeInitMutex(void)	;
void AcquireMutex(void);
void ReleaseMutex(void);
DWORD AddWorkToPool(ConnectInfo work);
ConnectInfo GetWorkFromPool();
DWORD MakeThreadToPool();
void WorkerThreadFunction(LPVOID pParam);
void Test_Connection(ConnectInfo* connectInfo);
void CloseHandle();
*/


void thread_pool::InitMutex(void)					//뮤텍스 생성
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

void thread_pool::DeInitMutex(void)				//뮤텍스 해제
{
	BOOL ret = ::CloseHandle(mutex);
}

void thread_pool::AcquireMutex(void)			//뮤텍스 동기화
{
	DWORD ret = WaitForSingleObject(mutex,INFINITE);

	if(ret ==WAIT_FAILED)
		printf("Error AcquireMutex! \n");
}

void thread_pool::ReleaseMutex(void)			//뮤텍스 반환
{
	BOOL ret = ::ReleaseMutex(mutex);
	if(ret == 0)
		printf("Error ReleaseMutex! \n");
}


DWORD thread_pool::AddWorkToPool(ConnectInfo work) //작업 등록
{
	BOOL bRun = FALSE;
	while(!bRun)				//작업이 쓰레드에 실행될때까지 반복
	{
		AcquireMutex();	//뮤텍스 동기화

		//작업이 최대작업수 초과시 에러메세지 출력 및 중지
		if(g_ThreadPool.idxOfLastAddedWork >= WORK_MAX)
		{
			printf("AddWorkToPool failed! \n");
			return NULL;
		}

		//작업 등록
		g_ThreadPool.workList[g_ThreadPool.idxOfLastAddedWork] = work;

		//작업가능한 쓰레드 검색
		for(DWORD i = g_ThreadPool.threadIdx;i<THREAD_MAX; i++)
		{
			if(g_ThreadPool.workerThreadList[i].bWork == FALSE)
			{
				//다음 작업으로 작업인덱스 증가
				g_ThreadPool.idxOfLastAddedWork++;

				//다음 쓰레드부터 자업가능여부를 검색하도록
				//현재 쓰레드의 다음 쓰레드 인덱스 저장
				g_ThreadPool.threadIdx = (i+1<THREAD_MAX) ? i+1 : 0 ;

				//현제 쓰레드의 이벤트 활성화
				SetEvent(g_ThreadPool.workerEventList[i]);

				//현제 쓰레드를 작업중으로 변경
				g_ThreadPool.workerThreadList[i].bWork = TRUE;
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

ConnectInfo thread_pool::GetWorkFromPool() //작업 가져오기
{
	ConnectInfo work;
	memset(&work,0,sizeof(ConnectInfo));
	AcquireMutex(); // 뮤텍스 동기화

	//현제 작업인덱스가 등록된 작업 인덱스 범위안에 있지 않을 경우
	//작업이 없는것으로 간주
	if( !(g_ThreadPool.idxOfCurrentWork<g_ThreadPool.idxOfLastAddedWork) )
	{
		ReleaseMutex(); //뮤텍스 반환;
		return work;
	}

	//작업 가져오기
	work = g_ThreadPool.workList[g_ThreadPool.idxOfCurrentWork++];
	ReleaseMutex();//뮤텍스 반환;
	return work; //가져온 작업 반환
}

DWORD thread_pool::MakeThreadToPool()
{
	InitMutex();				//뮤텍스 생성
	DWORD i;
	//최대 쓰레드 수만큼 쓰레드 및 이벤트 생성
	for(i = 0; i<THREAD_MAX ; i++)
	{
		DWORD idThread;
		HANDLE hThread;

		//이벤트 생성 및 핸들저장
		g_ThreadPool.workerEventList[i] = CreateEvent(NULL,FALSE,FALSE,NULL);
		//static_cast x
		//const_cast x
		//dynamic_cast x
		//reinterpret_cast x

		//쓰레드 생성
		hThread = CreateThread( NULL,0, (LPTHREAD_START_ROUTINE)WorkerThreadFunction,
			(LPVOID)i,0,&idThread);
		printf("--make thread: %d--\n\n", i);

		//쓰레드 핸들 저장
		g_ThreadPool.workerThreadList[i].hThread = hThread;
		//쓰레드 아이디 저장
		g_ThreadPool.workerThreadList[i].idThread = idThread;
		//작업중이 아님으로 초기화
		g_ThreadPool.workerThreadList[i].bWork = FALSE;
	}

	return i;
}



void thread_pool::Test_Connection(ConnectInfo* connectInfo)
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

	//수정부분
	
	if(g_odbc_Func.DBExcuteSQL(*connectInfo) ==false)
	{
		printf("Excute Error \n");
	}
	

}




void thread_pool::CloseHandle()	//리소스 해제
{
	BOOL bComplete = FALSE;
	//모든 쓰레드가 작업중이 아닌 경우 작업완료 시점으로 간주
	while(!bComplete)
	{
		for(int i = 0;i<THREAD_MAX;i++)
		{
			if(g_ThreadPool.workerThreadList[i].bWork == 1) //TRUE 1
			{
				bComplete = FALSE;
				break;
			}
			bComplete = TRUE;

		}

		
	}

	for(int i = 0;i<THREAD_MAX ; i++)
	{
		//쓰레드 핸들 해제
		::CloseHandle(g_ThreadPool.workerThreadList[i].hThread);
		//이벤트 핸들 해제
		::CloseHandle(g_ThreadPool.workerEventList[i]);
	}

	//1122 수정 (주석처리)
	thread_pool::DeInitMutex();	//뮤텍스 해제
}



