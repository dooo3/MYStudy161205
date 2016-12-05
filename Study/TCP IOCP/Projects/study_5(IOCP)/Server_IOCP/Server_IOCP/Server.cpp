#include "InItAll.h"
 

 using namespace std;

//~~~~스레드 함수들
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

	//1. Completion Port 생성// IOCP Kernel Object 를 생성
	// CreateCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 쓰레드숫자); 인자가 이렇게 들어가야 IOCP객체를 생성하는것이다.
	//연결은
	//CreateCompletionPort(sock, hIocp, 컴플리션 키, 0 );

	//이렇게 연결되서 나서 리턴값이 hIocp위치에 있는 값이 그대로 리턴됩니다. 만약 같지 않다면 에러
    HANDLE hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, // handle to file
																										 NULL, // handle to I/O completion port
																										 0,		  // completion key
																										 8		  // number of threads to execute concurrently //0으로 해주면 디폴트로 CPU숫자를 지정
																										 );
	 /*
     CreateIoCompletionPort 의 첫번째에 해당하며
     Completion Port 를 만들어주는 부분이다.
     은행이야기를 예로 들자면 은행을 만들어 주는 부분이 된다.
     */
 
    SYSTEM_INFO SystemInfo;
	 /*
     시스템 정보가 전달됨 쓰레드를 생성할때 CPU 의 개수에 따라
     쓰레드를 만들어 줘야 하기 때문에 정보를 얻어옴
     */

    GetSystemInfo(&SystemInfo);
	 /*
	 시스템 정보를 얻어온다.
	 이는 앞서 말했지만 CPU의 개수만큼 쓰레드를 만들기 위해서 이다
	 이렇게 얻어온 시스템 정보는 맴버변수 dwNumberOfProcessors 에CPU의 개수가들어간다.
	 */

	 //2. Completion Port 에서 입출력 완료를 대기하는 쓰레드를 CPU 개수만큼 생성. 쓰레드풀생성

	HANDLE RecvThread;
	RecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread_Func, (LPVOID)hCompletionPort , 0, NULL);

/*
	for(int i=0; i<SystemInfo.dwNumberOfProcessors; ++i)
	{
		 _beginthreadex(NULL, 0, (unsigned int (__stdcall*)(void*))CompletionThread, (LPVOID)hCompletionPort, 0, NULL);
		 g_Num_Of_Thread++;
	}
*/
	

       
		

	// 소켓을 만들때는 꼭 중첩 입출력 형식으로 만들어 주어야 한다. WSA_FLAG_OVERLAPPED
    SOCKET hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    SOCKADDR_IN servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//둘다 가능
    //servAddr.sin_port = htons(atoi("2738"));
    servAddr.sin_port = htons(9000);
 
    bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr));
    listen(hServSock, 0); //Socket s,int backlog : 대기열에 들어갈 수 있는 연결 혹은 부분적인 연결의 최대 개수를 뜻한다
 
	
	

	
	//소켓의 핸들과 주소
    SOCKET_DATA* PerHandleSockData;
	//소켓의 버퍼정보
    IO_DATA* PerIoData;
	
 
    int RecvBytes =0;
    int Flags =0;
	int retval = 0;

    while(1)
    {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(clientAddr);
 
        SOCKET hClientSock = accept(hServSock, (SOCKADDR*)&clientAddr, &addrLen);
 
		 //연결된클라이언트의소켓핸들정보와주소정보를설정.
        PerHandleSockData = (PPER_SOCK_HANDLE_DATA)malloc(sizeof(SOCKET_DATA));
        PerHandleSockData->hClientSock = hClientSock;
        memcpy(&(PerHandleSockData->clientAddr), &clientAddr, addrLen);
		//PerHandleSockData에 연결된 클라이언트의 정보들을 저장한다.
 
		//3. Overlapped 소켓과 CompletionPort의 연결.
        CreateIoCompletionPort((HANDLE)hClientSock, //FileHandle,            
											hCompletionPort, // ExistingCompletionPort, // handle to I/O completion port 기존의 CompletionPort 객체와 IOCP와 연결하는 작업
											(DWORD)PerHandleSockData, //FileHandle에 넣어준 이 핸들을 구분해주는 역할
											0	//0으로 해주면 디폴트로 CPU숫자를 지정
											);

		printf("[IOCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", 
			inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		/*
		  CreateIoCompletionPort 의 두번째 기능에 해당 한다.
		  현재 연결된 클라이언트와 만들어둔 CompletionPort 오브젝트를 연결 해준다.             
		  세번째 인자로 위에서 클라이언트 정보를 저장했던 PerHandleData를 넘겨 준다.
          이를 통해서 입출력이 완료된 클라이언트의 정보를 얻는것이 가능 하다.
		*/
		
		 // 연결된 클라이언트를 위한 버퍼를 설정하고 OVERLAPPED 구조체 변수 초기화.
        PerIoData = (PPER_IO_HANDLE_DATA)malloc(sizeof(IO_DATA));
        memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
        PerIoData->wsaBuf.len = BUFSIZE;
        PerIoData->wsaBuf.buf = PerIoData->buffer;

		// PerIoData 를 만들어 주고 초기화 합니다.

        Flags = 0;
 
		//4. 유저접속시 무조건 recv or send를 한번 해줘야 recv 가능
       retval = WSARecv(PerHandleSockData->hClientSock,  //데이터 입력소켓.
						&(PerIoData->wsaBuf),		//데이터 입력 버퍼포인터.
						1,									//데이터 입력 버퍼의 수
						(LPDWORD)&RecvBytes,	
						(LPDWORD)&Flags, 
						&(PerIoData->overlapped), //OVERLAPPED 구조체 포인터. 
						NULL
						);

		if(retval == 0)
		{
			//에러없음 에러가 발생하지않고 데이터 입력이 완료 되었다면 0을 반환한다. 커널에 등록된거.
		}else if(retval ==SOCKET_ERROR)
		{
			
			if(WSAGetLastError()!=ERROR_IO_PENDING)
			{
				//IO진행중 에러라기보다는 하나의 정보
			}
			//에러발생
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
											&BytesTransferred, //전송된 바이트수
											(LPDWORD)&PerHandleSockData, 
											(LPOVERLAPPED*)&PerIoData, //OVERLAPPED 구조체 포인터  LPPER_IO_DATA 구조체의 시작주소 와 overlapped 구조체의 시작주소는 같기 때문에 가능하다.
											INFINITE
											);

		if(b_Retval)
		{

		}else
		{
			GetLastError();
		}
		
		if(BytesTransferred == 0)	//EOF 전송시.
        {
			//클라이언트의 연결 종료시 처리를 해준다.
			printf("[IOCP 서버] 클라이언트 접속을해제 합니다\n");
            closesocket(PerHandleSockData->hClientSock);

            free(PerHandleSockData);
            free(PerIoData);
            continue;
        }
		
		
		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
		PerIoData->wsaBuf.len = BUFSIZE;
		PerIoData->wsaBuf.buf = PerIoData->buffer;
 
		flags = 0;
		
		//연결 소켓(:12)으로 부터 데이터를 읽는다.
		retval = WSARecv(PerHandleSockData->hClientSock, //연결 소켓을 가리키는 소켓 지정 번호 clientSock
					  &(PerIoData->wsaBuf), //WSABUF(:4300) 구조체 배열을 가리키는 포인터 //WSABUF 구조체는 데이터 버퍼를 가리키는 포인터와 버퍼의 크기 정보를 가지고 있다
					  1, //lpBuffers 배열에 있는 WSABUF 구조체의 개수.
					  NULL, 
					  &flags, //데이터 입력이 완료된 경우, 읽은 데이터의 바이트 크기를 넘긴다.
					  &(PerIoData->overlapped), //함수의 호출 방식을 지정한다.
					  NULL//WSAOVERLAPPED(:4300) 구조체의 포인터로 만약 non overlapped(:12) 소켓이라면 무시한다.
					  );
			if(retval == 0)
			{
				//에러없음 에러가 발생하지않고 데이터 입력이 완료 되었다면 0을 반환한다
			}else if(retval ==SOCKET_ERROR)
			{
				//에러발생
				GetLastError();
			}
		
		   // PerIoData->wsaBuf.buf[BytesTransferred] = '\0';

	
			
			g_Queue_recv_buf.push();
			
			printf("Recv[%s]\n",EXPORT_DATA(PerIoData->buffer,BytesTransferred));

			//EXPORT_DATA(PerIoData->buffer,BytesTransferred);

			//printf("Recv[%s]\n", PerIoData->wsaBuf.buf);
			
			//printf("Recv[%s]\n", (EXPORT_DATA(PerIoData->buffer,BytesTransferred))+1 );
			
			//printf("Recv[%s]\n", EXPORT_DATA(PerIoData->buffer,BytesTransferred));

			//여기서 그냥 큐에 넣어주고 queue에 데이터 있으면 그냥 출력해주는 Thread를 workerThread로 만들어줘야하나??????????



			//g_Queue_recv_buf.push(PerIoData); //필요없음


			
	 
			
			

				
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
	
	// 1. h1 파씽 
	memcpy(&nHeader1, buf+index,sizeof(int));
	index+=sizeof(int);
	// 2. h2 파씽
	memcpy(&nHeader2, buf+index,sizeof(int));
	index+=sizeof(int);

	// 2. 길이 가져오고
	memcpy(&nDatagramLen, buf+index,sizeof(int));
	index += sizeof(int);

	if( nHeader1 != PACKET_HEADER_START1 || nHeader2 != PACKET_HEADER_START2 )
	{
		// 소켓 끊기.. 나중에 추가.
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
