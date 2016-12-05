#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>

#include <io.h>



using namespace std;

#define BUFSIZE 210400
#define BUFSIZE_LOGIN 256

const static int MAX_ID		=	20;
const static int MAX_MSG	=	256;
const static int MAX_PASSWORD	=	128;


const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);

//실제 데이터의 첫 바이트에 들어가는 데이터 타입;;(헤더에 들어가는게 아님)
const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);
/*
	char szID[20];
	char szMsg[512];
*/

void EXPORT_DATA(char buf[], int nTotLen );
//데이터와 길이를 받아서 Queue에 저장하는 함수


//queue에 넣을 구조체
typedef struct packet_data
{
	char szPacket[BUFSIZE+1];
	SOCKET sock;
	//----------새로추가
	int recvBytes;
	int sendBytes;
	//----------새로추가
	packet_data()
	{
		memset( szPacket, 0x0, BUFSIZE );
	}
}PACKET_DATA;




//------------소켓 관리 변수
int g_nTotalSockets = 0;

PACKET_DATA* g_socket_Info[FD_SETSIZE];
FD_SET g_listen_rset;
FD_SET g_recv_set;
FD_SET g_writ_set;

//----------------------소켓 관리 함수
int AddSocketInfo(SOCKET clientsock);
void RemoveSocketInfo(int nIndex);

//----------------------소켓 관리 함수

//구조체 queue생성
queue<PACKET_DATA> Queue_recv_buf;

//queue<PACKET_DATA> Queue_send_buf;


//~~~~~~~~스레드 함수들
DWORD WINAPI Check_Data();
DWORD WINAPI Send_Process();
DWORD WINAPI Receive_Process();

/////////////////////20161010

/////////////////////20161011



// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}





int main(int argc, char* argv[])
{
	int retval;

	//fd_set readfds, allfds;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;
	// socket() 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");	
	
	
	//long port;
	//port = atol(argv[1]);

	// bind()
	//서버의 지역 ip주소와 지역포트 번호를 위한 구조체전달
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");
	
	// listen() - bind()에 의해 처리된 tcp포트의 상태를 LISTEN으로 바꾼다.
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");
	
	//넌블로킹 소켓으로 전환
	unsigned long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if(retval == SOCKET_ERROR)err_quit("ioctlsocket()");

	//SO_RESUSEADDR 소켓 옵션 설정(REUSEADDR)
	bool optReuse = true;
	retval = setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,
							(char*)&optReuse,sizeof(optReuse));
	if(retval == SOCKET_ERROR) err_quit("setsocketopt()");

	// 데이터 통신에 사용할 변수
	
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrlen;

	HANDLE data_chec_Thread;
	HANDLE recv_Proc_Thread;
	HANDLE send_Thread;

	//소켓 셋 초기화
	FD_ZERO(&g_listen_rset);  
	FD_ZERO(&g_recv_set);  
	FD_ZERO(&g_writ_set);

	//소켓 셋 지정
	FD_SET(listen_sock,&g_listen_rset);

	// 데이터체킹 스레드생성
	data_chec_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Check_Data,  NULL, 0, NULL);

	// recv쓰레드 생성
	recv_Proc_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Receive_Process, NULL , 0, NULL);

	//send_Thread Create
	send_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Send_Process, NULL , 0, NULL);


	
	if(data_chec_Thread == NULL)
		printf("[오류] 스레드 생성 실패!\n");
	else
		CloseHandle(data_chec_Thread);

	if(send_Thread == NULL)
		printf("[오류] 스레드 생성 실패!\n");
	else
		CloseHandle(send_Thread);

	if(recv_Proc_Thread == NULL)
		printf("[오류] 스레드 생성 실패!\n");
	else
		CloseHandle(recv_Proc_Thread);

	while(1){

		// 1. SELECT() 내에서 읽기셋 내에 상태가 변경된 소켓이 있으면 리턴.
		// 2. SELECT() 읽기셋에 소켓이 아애 없으면 그냥 바로리턴.
		// 3. 장점 : loop를 계속 안도니깐 자원소모가 없다. -> 계속적으로 소켓함수 호출하여 확인할 필요없음.
		// 4. 단점 : select 리턴값이 특정소켓을 리턴하는게 아니기에, 모든 소켓에 대해 FD_ISSET() 검사를 해야함.(큰단점은X)
		retval = select( 2 , &g_listen_rset , NULL, NULL , NULL);
		if( retval > 0 )
		{
			if( FD_ISSET(listen_sock,&g_listen_rset) )	
			{
				addrlen = sizeof(clientAddr);
				
				clientSocket = accept(listen_sock,(SOCKADDR*)&clientAddr,&addrlen);
				if(clientSocket == INVALID_SOCKET)
				{
					if((WSAGetLastError() != WSAEWOULDBLOCK))
					{
						err_quit("accept()");
					}
				}else
				{
					// 접속이 되면
					printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", 
						inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
					//Sleep(10);

					//소켓 정보 추가
					int n_sock_no = AddSocketInfo(clientSocket);
					if( n_sock_no == -1 )
					{
						printf("[TCP 서버] 클라이언트 접속을해제 합니다n");
						closesocket(clientSocket);	
					}
		
				}//else{//접속이 되면..
			} //(FD_ISSET(listen_sock,&g_recv_set))	
		}

	

	} //while end
	return 0;
}






///
// 클라이언트와 데이터 통신
DWORD WINAPI Send_Process()
{
	SOCKADDR_IN clientAddr;
	int addrlen;
	int retval;

	addrlen = sizeof(clientAddr);

	while(1)
	{
		//현제는 클라이언트1개만 접속해서 소켓의 개수가 1임 
		//여러개 의 클라이언트가 접속시...
		for(int i = 0; i < g_nTotalSockets; ++i)
		{
			PACKET_DATA* pInfo = g_socket_Info[i];
			if(FD_ISSET(pInfo->sock, &g_writ_set))
			{
				// 데이터보내기
				retval = send(pInfo->sock, pInfo->szPacket + pInfo->sendBytes,
					pInfo->recvBytes - pInfo->sendBytes, 0);


				if(retval == SOCKET_ERROR)
				{
					if(WSAGetLastError() != WSAEWOULDBLOCK)
					{
						err_display("send()");
						RemoveSocketInfo(i);
						continue;
					}
				}

				pInfo->sendBytes += retval;
				if(pInfo->recvBytes == pInfo->sendBytes)
					pInfo->recvBytes = pInfo->sendBytes = 0;
			}//if(FD_ISSET(pInfo->sock, &g_writ_set))
		}//for
		
		//Sleep(1);
		//delay()
	}

	return 0;
}


DWORD WINAPI Receive_Process()
{
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;

	addrlen = sizeof(clientaddr);

	while(1)
	{
		
		retval = select( g_nTotalSockets+1 ,&g_recv_set,NULL,NULL,NULL);
		if(retval>0) 
		{
			for(int i = 0; i < g_nTotalSockets; ++i)
			{
				PACKET_DATA* pInfo = g_socket_Info[i];

				if(FD_ISSET(pInfo->sock, &g_recv_set))
				{
					// 데이터받기
					retval = recv(pInfo->sock, pInfo->szPacket, BUFSIZE, 0);

					if(retval == SOCKET_ERROR)
					{
						if(WSAGetLastError() != WSAEWOULDBLOCK)
						{
							err_display("recv()");
							RemoveSocketInfo(i);
							continue;
						}
					}
					else if(retval == 0)
					{
						RemoveSocketInfo(i);
						continue;
					}
					else
					{
						pInfo->recvBytes = retval;

						// 받은 데이터 출력
						SOCKADDR_IN sockAddr;
						int nAddrLength = sizeof(sockAddr);
						// 클라이언트 정보 얻기
						getpeername(pInfo->sock, (SOCKADDR*)&sockAddr, &addrlen);
						//받은 패킷 분석
						EXPORT_DATA(pInfo->szPacket,retval);
						
						pInfo->szPacket[retval] = '\0';
						printf("[TCP/%s:%d] %s\n",
							inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port), pInfo->szPacket);
					}
				}
			}
		}


	}

		
	
	return 0;
}





void EXPORT_DATA(char buf[], int nTotLen)
{
	int index = 0;
	
	PACKET_DATA stDatagram;

	int nDatagramLen = 0;
	int nHeader1 = 0;
	int nHeader2 = 0;
	
	while(index < nTotLen)
	{
		nDatagramLen = 0;
		nHeader1 = 0;
		nHeader2 = 0;
		memset( &stDatagram, 0x0, sizeof( PACKET_DATA ) );

		// 1. h1 파씽 
		memcpy(&nHeader1, buf+index,sizeof(int));
		index+=sizeof(int);
		// 2. h2 파씽
		memcpy(&nHeader2, buf+index,sizeof(int));
		index+=sizeof(int);

		if( nHeader1 != PACKET_HEADER_START1 || nHeader2 != PACKET_HEADER_START2 )
		{
			// 소켓 끊기.. 나중에 추가.
			return;
		}

		// 2. 길이 가져오고
		memcpy(&nDatagramLen, buf+index,sizeof(int));
		index += sizeof(int);
	
		//3. 길이만큼 데이터 긁어오기
		memcpy( stDatagram.szPacket, buf+index, nDatagramLen );
		index += nDatagramLen;
		
		//소켓도 따로 넣어주고

		Queue_recv_buf.push(stDatagram);

		//Queue_send_buf.push(stDatagram);

		//g_nCnt++;
		//printf("g_nCnt:%d\n", g_nCnt);

	}
}


void MsgReq(char* szBuf, int index,int NumOfMessage )
{
	char szID[MAX_ID+1] = {0x0,};
	char szMsg[MAX_MSG+1] = {0x0,};

	
	//????????? Start1,Start2만큼 더 이동해야 하는거 아닌가?
	//아니다 queue에는 진짜 데이터만 넣었다
	//nType만큼만 이동해야하는데?...

	
	memcpy(szID,szBuf+index,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+index,MAX_MSG);
	index+=MAX_MSG;
	
	printf("메시지 수:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);

	//일단 되는것 
	/*
	index+=sizeof(int);

	memcpy(szID,szBuf+1,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+MAX_ID+1,MAX_MSG);
	index+=MAX_MSG;
	*/
	//printf("메시지 수:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);
	
}


//데이터를 채크하는 swicth문을 여기로 옮긴다
DWORD WINAPI Check_Data()
{
	int index = 0;
	int num_of_Queue = 1;
	int nPacketType = 0;
	PACKET_DATA real_Data;

	int data_Lenght = 0;

	while(1)
	{
		if(!Queue_recv_buf.empty())
		{

			real_Data = Queue_recv_buf.front();

			data_Lenght = real_Data.szPacket[sizeof(int) + sizeof(int) + sizeof(int)];
			real_Data.szPacket[data_Lenght+1];

			//여기는 RealData만 들어가니까 사이즈를 HeaderLenght만큼 넣어야 하는거 아닌가?

		
			
			//real_Data.szPacket[BUFSIZE+1];  //방금 주석처리 한 것
			//memset(real_Data.szPacket, 0x0, sizeof(BUFSIZE) );

			index = 0;
			
			nPacketType = real_Data.szPacket[index++];

			switch(nPacketType)
			{
				case CS_CHAT_MSG_NTY:
					MsgReq(real_Data.szPacket, index,num_of_Queue);
					
					//printf("큐개수:%d ,index:%d,아이디:%s,  메시지:%s,header1:%d ,header2:%d ,lenght:%d\n ",num_of_Queue,index,szID,szMsg,PACKET_HEADER_START1,PACKET_HEADER_START2,PACKET_LENGHT);
					
					Queue_recv_buf.pop();
					num_of_Queue++;
				break;

			}

		}else
		{
			//break;
		}



	}

		

	 index = 0;	
	 
	 printf("받은 총 메시지수:%d\n",num_of_Queue);
	 
	 num_of_Queue = 1;
	
	return 0;
}


int AddSocketInfo(SOCKET clientsock)
{
	if(g_nTotalSockets >= (FD_SETSIZE-1)){//MAX 체크
		printf("[오류] 소켓정보를 추가할 수 없습니다.");
		return -1;
	}

	PACKET_DATA *ptr = new PACKET_DATA;
	if(ptr == NULL)
	{
		printf("[오류] 메모리가 부족합니다.\n");
		return -1;
	}

	ptr->sock = clientsock;
	ptr->recvBytes = 0;
	ptr->sendBytes = 0;

	FD_SET(clientsock, &g_recv_set);
	FD_SET(clientsock, &g_writ_set);	
	
	g_socket_Info[g_nTotalSockets++] = ptr;

	return g_nTotalSockets-1;
}

void RemoveSocketInfo(int nIndex)
{
	PACKET_DATA* pInfo = g_socket_Info[nIndex];

	// 클라이언트 정보 얻기
	SOCKADDR_IN socketAddr;
	int addrlen = sizeof(socketAddr);
	getpeername(pInfo->sock, (SOCKADDR*)&socketAddr, &addrlen);
	printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트번호 = %d\n",
		inet_ntoa(socketAddr.sin_addr), ntohs(socketAddr.sin_port));

	closesocket(pInfo->sock);
	delete pInfo;

	for(int i = nIndex; i < g_nTotalSockets; ++i)
		g_socket_Info[i] = g_socket_Info[i + 1];

	g_nTotalSockets--;
}


