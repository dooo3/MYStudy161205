// Test_ODBC_2.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

/*
DB������Ʈ �뵵 
Ŭ���̾�Ʈor���Ӽ����� Ư�������Ϳ�û�� ����, ��� DB���ν��� ȣ���ؼ� ������� Ŭ���̾�ƮOR���Ӽ��� ����.

--1���� ���ȶ����� ������Ʈ�� �ΰ� . .
--2���� ���Ǽ��� ���̱����� . .

DB - ����DB������Ʈ - ���Ӽ���
DB - WEB�������Ʈ - Ŭ���̾�Ʈ

1. recv() ȣ��.

 ��Ŷ�а�
 ��Ŷ���� : ģ������Ʈ �ҷ����� / �� ���� �ҷ�����
 �� ���� �ҷ����� ��Ŷ : DB_GET_USERINFO | NID
 proc_get_user_info( NID ) 
 ������ �ҷ����� ���ν��� ȣ��.

*/


#include "define.h"
#include "odbc_func.h"
#include "thread_pool.h"



static odbc_func g_odbc_Func;
static thread_pool g_thread_Pool;

void WorkerThreadFunction(LPVOID pParam)		//������ �Լ�
{
	ConnectInfo* work;
	//���� �������� �̺�Ʈ �ڵ� ��������
	HANDLE event = g_ThreadPool.workerEventList[(DWORD)pParam];

	while(1) //�� �� �� �� 
	{

		//printf("START	%d~~~~~~~\n",(int)pParam);

		printf("--wait thread: %d--\n",(int)pParam);
		//�̺�Ʈ�� Ȱ��ȭ �ɶ����� ���
		WaitForSingleObject(event,INFINITE);
		
		work = &(g_thread_Pool.GetWorkFromPool());				//�۾� ��������
		
		if(work->DNSID != NULL)							//�۾��� ���������� ����������
		{
			printf("--run thread: %d \n",(int)pParam);
			g_thread_Pool.Test_Connection(work);

			//���� �����带 �۾����� �ƴ����� ����
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

int main()
{
	int nRetCode = 0;
	char a;
	g_thread_Pool.MakeThreadToPool();

	ConnectInfo Ci1;//�۾�1
	ConnectInfo Ci2;//�۾�2
	ConnectInfo Ci3;//�۾�3
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



