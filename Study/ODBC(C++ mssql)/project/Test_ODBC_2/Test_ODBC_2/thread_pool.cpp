
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


void thread_pool::InitMutex(void)					//���ؽ� ����
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

void thread_pool::DeInitMutex(void)				//���ؽ� ����
{
	BOOL ret = ::CloseHandle(mutex);
}

void thread_pool::AcquireMutex(void)			//���ؽ� ����ȭ
{
	DWORD ret = WaitForSingleObject(mutex,INFINITE);

	if(ret ==WAIT_FAILED)
		printf("Error AcquireMutex! \n");
}

void thread_pool::ReleaseMutex(void)			//���ؽ� ��ȯ
{
	BOOL ret = ::ReleaseMutex(mutex);
	if(ret == 0)
		printf("Error ReleaseMutex! \n");
}


DWORD thread_pool::AddWorkToPool(ConnectInfo work) //�۾� ���
{
	BOOL bRun = FALSE;
	while(!bRun)				//�۾��� �����忡 ����ɶ����� �ݺ�
	{
		AcquireMutex();	//���ؽ� ����ȭ

		//�۾��� �ִ��۾��� �ʰ��� �����޼��� ��� �� ����
		if(g_ThreadPool.idxOfLastAddedWork >= WORK_MAX)
		{
			printf("AddWorkToPool failed! \n");
			return NULL;
		}

		//�۾� ���
		g_ThreadPool.workList[g_ThreadPool.idxOfLastAddedWork] = work;

		//�۾������� ������ �˻�
		for(DWORD i = g_ThreadPool.threadIdx;i<THREAD_MAX; i++)
		{
			if(g_ThreadPool.workerThreadList[i].bWork == FALSE)
			{
				//���� �۾����� �۾��ε��� ����
				g_ThreadPool.idxOfLastAddedWork++;

				//���� ��������� �ھ����ɿ��θ� �˻��ϵ���
				//���� �������� ���� ������ �ε��� ����
				g_ThreadPool.threadIdx = (i+1<THREAD_MAX) ? i+1 : 0 ;

				//���� �������� �̺�Ʈ Ȱ��ȭ
				SetEvent(g_ThreadPool.workerEventList[i]);

				//���� �����带 �۾������� ����
				g_ThreadPool.workerThreadList[i].bWork = TRUE;
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

ConnectInfo thread_pool::GetWorkFromPool() //�۾� ��������
{
	ConnectInfo work;
	memset(&work,0,sizeof(ConnectInfo));
	AcquireMutex(); // ���ؽ� ����ȭ

	//���� �۾��ε����� ��ϵ� �۾� �ε��� �����ȿ� ���� ���� ���
	//�۾��� ���°����� ����
	if( !(g_ThreadPool.idxOfCurrentWork<g_ThreadPool.idxOfLastAddedWork) )
	{
		ReleaseMutex(); //���ؽ� ��ȯ;
		return work;
	}

	//�۾� ��������
	work = g_ThreadPool.workList[g_ThreadPool.idxOfCurrentWork++];
	ReleaseMutex();//���ؽ� ��ȯ;
	return work; //������ �۾� ��ȯ
}

DWORD thread_pool::MakeThreadToPool()
{
	InitMutex();				//���ؽ� ����
	DWORD i;
	//�ִ� ������ ����ŭ ������ �� �̺�Ʈ ����
	for(i = 0; i<THREAD_MAX ; i++)
	{
		DWORD idThread;
		HANDLE hThread;

		//�̺�Ʈ ���� �� �ڵ�����
		g_ThreadPool.workerEventList[i] = CreateEvent(NULL,FALSE,FALSE,NULL);
		//static_cast x
		//const_cast x
		//dynamic_cast x
		//reinterpret_cast x

		//������ ����
		hThread = CreateThread( NULL,0, (LPTHREAD_START_ROUTINE)WorkerThreadFunction,
			(LPVOID)i,0,&idThread);
		printf("--make thread: %d--\n\n", i);

		//������ �ڵ� ����
		g_ThreadPool.workerThreadList[i].hThread = hThread;
		//������ ���̵� ����
		g_ThreadPool.workerThreadList[i].idThread = idThread;
		//�۾����� �ƴ����� �ʱ�ȭ
		g_ThreadPool.workerThreadList[i].bWork = FALSE;
	}

	return i;
}



void thread_pool::Test_Connection(ConnectInfo* connectInfo)
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

	//�����κ�
	
	if(g_odbc_Func.DBExcuteSQL(*connectInfo) ==false)
	{
		printf("Excute Error \n");
	}
	

}




void thread_pool::CloseHandle()	//���ҽ� ����
{
	BOOL bComplete = FALSE;
	//��� �����尡 �۾����� �ƴ� ��� �۾��Ϸ� �������� ����
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
		//������ �ڵ� ����
		::CloseHandle(g_ThreadPool.workerThreadList[i].hThread);
		//�̺�Ʈ �ڵ� ����
		::CloseHandle(g_ThreadPool.workerEventList[i]);
	}

	//1122 ���� (�ּ�ó��)
	thread_pool::DeInitMutex();	//���ؽ� ����
}



