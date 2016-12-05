#include "InItAll.h"
 

 using namespace std;

//~~~~������ �Լ���
DWORD WINAPI RecvThread_Func(LPVOID pComPort);
DWORD WINAPI SendThread_Func();
DWORD WINAPI WorkerThread_Func();

char* EXPORT_DATA(char buf[], int nTotLen); 
//2016-10-17


//2016-10-20

queue<REAL_DATA> g_Queue_recv_buf;

 
#pragma comment(lib, "ws2_32.lib")



int main(int argc, char** argv)
{
	InItAll Initall;

	//1. Completion Port ����// IOCP Kernel Object �� ����
	// CreateCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, ���������); ���ڰ� �̷��� ���� IOCP��ü�� �����ϴ°��̴�.
	//������
	//CreateCompletionPort(sock, hIocp, ���ø��� Ű, 0 );

	//�̷��� ����Ǽ� ���� ���ϰ��� hIocp��ġ�� �ִ� ���� �״�� ���ϵ˴ϴ�. ���� ���� �ʴٸ� ����
    HANDLE hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, // handle to file
																										 NULL, // handle to I/O completion port
																										 0,		  // completion key
																										 8		  // number of threads to execute concurrently //0���� ���ָ� ����Ʈ�� CPU���ڸ� ����
																										 );
	 /*
     CreateIoCompletionPort �� ù��°�� �ش��ϸ�
     Completion Port �� ������ִ� �κ��̴�.
     �����̾߱⸦ ���� ���ڸ� ������ ����� �ִ� �κ��� �ȴ�.
     */
 
    SYSTEM_INFO SystemInfo;
	 /*
     �ý��� ������ ���޵� �����带 �����Ҷ� CPU �� ������ ����
     �����带 ����� ��� �ϱ� ������ ������ ����
     */

    GetSystemInfo(&SystemInfo);
	 /*
	 �ý��� ������ ���´�.
	 �̴� �ռ� �������� CPU�� ������ŭ �����带 ����� ���ؼ� �̴�
	 �̷��� ���� �ý��� ������ �ɹ����� dwNumberOfProcessors ��CPU�� ����������.
	 */

	 //2. Completion Port ���� ����� �ϷḦ ����ϴ� �����带 CPU ������ŭ ����. ������Ǯ����

	HANDLE RecvThread;
	RecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread_Func, (LPVOID)hCompletionPort , 0, NULL);

/*
	for(int i=0; i<SystemInfo.dwNumberOfProcessors; ++i)
	{
		 _beginthreadex(NULL, 0, (unsigned int (__stdcall*)(void*))CompletionThread, (LPVOID)hCompletionPort, 0, NULL);
		 g_Num_Of_Thread++;
	}
*/
	

       
		

	// ������ ���鶧�� �� ��ø ����� �������� ����� �־�� �Ѵ�. WSA_FLAG_OVERLAPPED
    SOCKET hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    SOCKADDR_IN servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//�Ѵ� ����
    //servAddr.sin_port = htons(atoi("2738"));
    servAddr.sin_port = htons(9000);
 
    bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr));
    listen(hServSock, 0); //Socket s,int backlog : ��⿭�� �� �� �ִ� ���� Ȥ�� �κ����� ������ �ִ� ������ ���Ѵ�
 
	
	

	
	//������ �ڵ�� �ּ�
    SOCKET_DATA* PerHandleSockData;
	//������ ��������
    IO_DATA* PerIoData;
	
 
    int RecvBytes =0;
    int Flags =0;
	int retval = 0;

    while(1)
    {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(clientAddr);
 
        SOCKET hClientSock = accept(hServSock, (SOCKADDR*)&clientAddr, &addrLen);
 
		 //�����Ŭ���̾�Ʈ�Ǽ����ڵ��������ּ�����������.
        PerHandleSockData = (PPER_SOCK_HANDLE_DATA)malloc(sizeof(SOCKET_DATA));
        PerHandleSockData->hClientSock = hClientSock;
        memcpy(&(PerHandleSockData->clientAddr), &clientAddr, addrLen);
		//PerHandleSockData�� ����� Ŭ���̾�Ʈ�� �������� �����Ѵ�.
 
		//3. Overlapped ���ϰ� CompletionPort�� ����.
        CreateIoCompletionPort((HANDLE)hClientSock, //FileHandle,            
											hCompletionPort, // ExistingCompletionPort, // handle to I/O completion port ������ CompletionPort ��ü�� IOCP�� �����ϴ� �۾�
											(DWORD)PerHandleSockData, //FileHandle�� �־��� �� �ڵ��� �������ִ� ����
											0	//0���� ���ָ� ����Ʈ�� CPU���ڸ� ����
											);

		printf("[IOCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
			inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		/*
		  CreateIoCompletionPort �� �ι�° ��ɿ� �ش� �Ѵ�.
		  ���� ����� Ŭ���̾�Ʈ�� ������ CompletionPort ������Ʈ�� ���� ���ش�.             
		  ����° ���ڷ� ������ Ŭ���̾�Ʈ ������ �����ߴ� PerHandleData�� �Ѱ� �ش�.
          �̸� ���ؼ� ������� �Ϸ�� Ŭ���̾�Ʈ�� ������ ��°��� ���� �ϴ�.
		*/
		
		 // ����� Ŭ���̾�Ʈ�� ���� ���۸� �����ϰ� OVERLAPPED ����ü ���� �ʱ�ȭ.
        PerIoData = (PPER_IO_HANDLE_DATA)malloc(sizeof(IO_DATA));
        memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
        PerIoData->wsaBuf.len = BUFSIZE;
        PerIoData->wsaBuf.buf = PerIoData->buffer;

		// PerIoData �� ����� �ְ� �ʱ�ȭ �մϴ�.

        Flags = 0;
 
		//4. �������ӽ� ������ recv or send�� �ѹ� ����� recv ����
       retval = WSARecv(PerHandleSockData->hClientSock,  //������ �Է¼���.
						&(PerIoData->wsaBuf),		//������ �Է� ����������.
						1,									//������ �Է� ������ ��
						(LPDWORD)&RecvBytes,	
						(LPDWORD)&Flags, 
						&(PerIoData->overlapped), //OVERLAPPED ����ü ������. 
						NULL
						);

		if(retval == 0)
		{
			//�������� ������ �߻������ʰ� ������ �Է��� �Ϸ� �Ǿ��ٸ� 0�� ��ȯ�Ѵ�. Ŀ�ο� ��ϵȰ�.
		}else if(retval ==SOCKET_ERROR)
		{
			
			if(WSAGetLastError()!=ERROR_IO_PENDING)
			{
				//IO������ ������⺸�ٴ� �ϳ��� ����
			}
			//�����߻�
		}
    }
 
    return 0;
}
 

DWORD WINAPI RecvThread_Func(LPVOID pComPort)
{
	HANDLE hCompletionPort = (HANDLE)pComPort;

	 DWORD BytesTransferred;

    //LPPER_HANDLE_DATA PerHandleData;
    //LPPER_IO_DATA PerIoData;
	
	SOCKET_DATA* PerHandleSockData;
	IO_DATA* PerIoData;
	REAL_DATA realData;

    DWORD flags;
	
	int retval = 0;
	bool b_Retval =FALSE;

	 while(1)		
	 {	

		b_Retval = GetQueuedCompletionStatus(hCompletionPort, //Completion Port
											&BytesTransferred, //���۵� ����Ʈ��
											(LPDWORD)&PerHandleSockData, 
											(LPOVERLAPPED*)&PerIoData, //OVERLAPPED ����ü ������  LPPER_IO_DATA ����ü�� �����ּ� �� overlapped ����ü�� �����ּҴ� ���� ������ �����ϴ�.
											INFINITE
											);

		if(b_Retval)
		{

		}else
		{
			GetLastError();
		}
		
		if(BytesTransferred == 0)	//EOF ���۽�.
        {
			//Ŭ���̾�Ʈ�� ���� ����� ó���� ���ش�.
			printf("[IOCP ����] Ŭ���̾�Ʈ ���������� �մϴ�\n");
            closesocket(PerHandleSockData->hClientSock);

            free(PerHandleSockData);
            free(PerIoData);
            continue;
        }
		
		
		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
		PerIoData->wsaBuf.len = BUFSIZE;
		PerIoData->wsaBuf.buf = PerIoData->buffer;
 
		flags = 0;
		
		//���� ����(:12)���� ���� �����͸� �д´�.
		retval = WSARecv(PerHandleSockData->hClientSock, //���� ������ ����Ű�� ���� ���� ��ȣ clientSock
					  &(PerIoData->wsaBuf), //WSABUF(:4300) ����ü �迭�� ����Ű�� ������ //WSABUF ����ü�� ������ ���۸� ����Ű�� �����Ϳ� ������ ũ�� ������ ������ �ִ�
					  1, //lpBuffers �迭�� �ִ� WSABUF ����ü�� ����.
					  NULL, 
					  &flags, //������ �Է��� �Ϸ�� ���, ���� �������� ����Ʈ ũ�⸦ �ѱ��.
					  &(PerIoData->overlapped), //�Լ��� ȣ�� ����� �����Ѵ�.
					  NULL//WSAOVERLAPPED(:4300) ����ü�� �����ͷ� ���� non overlapped(:12) �����̶�� �����Ѵ�.
					  );
			if(retval == 0)
			{
				//�������� ������ �߻������ʰ� ������ �Է��� �Ϸ� �Ǿ��ٸ� 0�� ��ȯ�Ѵ�
			}else if(retval ==SOCKET_ERROR)
			{
				//�����߻�
				GetLastError();
			}
		
		   // PerIoData->wsaBuf.buf[BytesTransferred] = '\0';

	
			
			g_Queue_recv_buf.push();
			
			printf("Recv[%s]\n",EXPORT_DATA(PerIoData->buffer,BytesTransferred));

			//EXPORT_DATA(PerIoData->buffer,BytesTransferred);

			//printf("Recv[%s]\n", PerIoData->wsaBuf.buf);
			
			//printf("Recv[%s]\n", (EXPORT_DATA(PerIoData->buffer,BytesTransferred))+1 );
			
			//printf("Recv[%s]\n", EXPORT_DATA(PerIoData->buffer,BytesTransferred));

			//���⼭ �׳� ť�� �־��ְ� queue�� ������ ������ �׳� ������ִ� Thread�� workerThread�� ���������ϳ�??????????



			//g_Queue_recv_buf.push(PerIoData); //�ʿ����


			
	 
			
			

				
	 }

}












char* EXPORT_DATA(char buf[], int nTotLen)//,char type)
{
	
	char szMsg[MAX_MSG+1] = {0x0,};
//	char szMsg = {0x0};
	
	int nType =0 ;

	int index = 0;
	
	int nDatagramLen = 0;

	int nHeader1 = 0;
	int nHeader2 = 0;
	
	//memset( &stDatagram, 0x0, sizeof( SOCKET_DATA ) );
	
	// 1. h1 �ľ� 
	memcpy(&nHeader1, buf+index,sizeof(int));
	index+=sizeof(int);
	// 2. h2 �ľ�
	memcpy(&nHeader2, buf+index,sizeof(int));
	index+=sizeof(int);

	// 2. ���� ��������
	memcpy(&nDatagramLen, buf+index,sizeof(int));
	index += sizeof(int);

	if( nHeader1 != PACKET_HEADER_START1 || nHeader2 != PACKET_HEADER_START2 )
	{
		// ���� ����.. ���߿� �߰�.
		//return stDatagram;
	}else if( nHeader1 == PACKET_HEADER_START1 && nHeader2 == PACKET_HEADER_START2) 
	{
		nType= buf[index++];

		switch(nType)
		{
			case CS_CHAT_MSG_NTY:
			
			memcpy(&szMsg, buf+index, MAX_MSG );
	
			//index += MAX_MSG;
			break;
		}
	}

	return szMsg;
}
