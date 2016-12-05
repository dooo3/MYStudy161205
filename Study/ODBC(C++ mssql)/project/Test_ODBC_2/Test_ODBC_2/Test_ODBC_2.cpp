// Test_ODBC_2.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

/*
DB에이전트 용도 
클라이언트or게임서버가 특정데이터요청이 오면, 대신 DB프로시저 호출해서 결과값을 클라이언트OR게임서버 리턴.

--1순위 보안때문에 에이전트를 두고 . .
--2순위 세션수를 줄이기위해 . .

DB - 게임DB에이전트 - 게임서버
DB - WEB디비에이전트 - 클라이언트

1. recv() 호출.

 패킷읽고
 패킷종류 : 친구리스트 불러오기 / 내 정보 불러오기
 내 정보 불러오기 패킷 : DB_GET_USERINFO | NID
 proc_get_user_info( NID ) 
 내정보 불러오기 프로시저 호출.

*/


#include "define.h"
#include "odbc_func.h"
#include "thread_pool.h"



static odbc_func g_odbc_Func;
static thread_pool g_thread_Pool;

void WorkerThreadFunction(LPVOID pParam)		//쓰레드 함수
{
	ConnectInfo* work;
	//현재 쓰레드의 이벤트 핸들 가져오기
	HANDLE event = g_ThreadPool.workerEventList[(DWORD)pParam];

	while(1) //무 한 반 복 
	{

		//printf("START	%d~~~~~~~\n",(int)pParam);

		printf("--wait thread: %d--\n",(int)pParam);
		//이벤트가 활성화 될때까지 대기
		WaitForSingleObject(event,INFINITE);
		
		work = &(g_thread_Pool.GetWorkFromPool());				//작업 가져오기
		
		if(work->DNSID != NULL)							//작업을 성공적으로 가져왔으면
		{
			printf("--run thread: %d \n",(int)pParam);
			g_thread_Pool.Test_Connection(work);

			//현제 쓰레드를 작업중이 아님으로 변경
			g_ThreadPool.workerThreadList[(DWORD)pParam].bWork = FALSE;
		}

		Sleep(3000);

		//printf("END		%d~~~~~~~\n",(int)pParam);
		
	}


}


//////////////////////////


//////////////////////////




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

int main()
{
	int nRetCode = 0;
	char a;
	g_thread_Pool.MakeThreadToPool();

	ConnectInfo Ci1;//작업1
	ConnectInfo Ci2;//작업2
	ConnectInfo Ci3;//작업3
	memset(&Ci1,0,sizeof(ConnectInfo));
	memset(&Ci2,0,sizeof(ConnectInfo));
	memset(&Ci3,0,sizeof(ConnectInfo)); 

	while(1)
	{
		scanf_s("%c",&a);
		
		if( a == 'a')
		{
			g_odbc_Func.ConnectInfo_InIt(Ci1,(SQLCHAR*)"Test_Rma",(SQLCHAR*)"sa",(SQLCHAR*)"tjfgusWk@8949!");
			//g_odbc_Func.ConnectInfo_InIt(Ci2,(SQLCHAR*)"Test_Rma2",(SQLCHAR*)"sa",(SQLCHAR*)"tjfgusWk@8949!");
			//g_odbc_Func.ConnectInfo_InIt(Ci3,(SQLCHAR*)"Test_Rma3",(SQLCHAR*)"sa",(SQLCHAR*)"tjfgusWk@8949!");
			//g_thread_Pool.CloseHandle();
		}
		
		//g_thread_Pool.CloseHandle();
	}
	
	
	

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



