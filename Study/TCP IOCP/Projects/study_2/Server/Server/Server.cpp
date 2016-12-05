#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>

using namespace std;

#define BUFSIZE 210400
#define BUFSIZE_LOGIN 256

const static int MAX_ID		=	20;
const static int MAX_MSG	=	256;
const static int MAX_PASSWORD	=	128;


const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);

//int PACKET_LENGHT			=	sizeof(int)+sizeof(int)+sizeof(int)+MAX_ID+MAX_MSG;
//////////////////////////////////////////////////s1,s2,lenght

//const static int PACKET_HEADER_END1		=	((BYTE)0xDD);
//const static int PACKET_HEADER_END2		=	((BYTE)0xCC);

//#define CS_CHAT_MSG_NTY		(BYTE(0x01))	// 요청 _REQ , 통지 _NTY, 응답 _ACK

//실제 데이터의 첫 바이트에 들어가는 데이터 타입;;(헤더에 들어가는게 아님)
const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);
/*
	char szID[20];
	char szMsg[512];
*/

void EXPORT_DATA(char buf[], int nTotLen );
//데이터와 길이를 받아서 Queue에 저장하는 함수
/////////////////////20161010

HANDLE g_Data_Thread;
//queue에 들어간 데이터를 분석하고 출력하는 함수 

//--------------test용 recv_Buf,recv_retval
//char recv_Buf[BUFSIZE+1];
//int recv_retval = 0;


//queue에 넣을 구조체
typedef struct packet_data
{
	char szPacket[BUFSIZE+1];
	SOCKET sock;
	packet_data()
	{
		memset( szPacket, 0x0, BUFSIZE );
	}
}PACKET_DATA;

//구조체로 queue생성
queue<PACKET_DATA> Queue_buf;

PACKET_DATA packet_Data;

DWORD WINAPI Check_Data(LPVOID arg);
/////////////////////20161010

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





///
// 클라이언트와 데이터 통신
DWORD WINAPI Receive_Process(LPVOID arg)
{
	//SOCKET client_sock = (SOCKET)arg;
	char buf[BUFSIZE+1];
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;

	//이걸로 전부 바꾸기
	packet_Data.sock = (SOCKET)arg;

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(packet_Data.sock, (SOCKADDR *)&clientaddr, &addrlen);

	//패킷을 일단 모두 받고 큐에넣고 (소켓도 같이 넣어야 한다? 생각해보고)
	//새 스레드 생성해서 데이터 있으면 스위치 문에 넣어서 받는다
	//만약 헤더가 맞지않는다면 소켓 끊어버리기

	
	while(1){
		// 데이터 받기
		// 1. 커널 송신버퍼에 데이터가 있으면 읽어온다.
		retval = recv(packet_Data.sock, buf, BUFSIZE, 0); 
		//패킷을 읽어와서 데이터 부분만 Queue에 넣는다
		EXPORT_DATA(buf, retval);
		
	}

	// closesocket()
	closesocket(packet_Data.sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", 
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}



	
	


int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");	
	
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");
	
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	//SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	HANDLE recv_Proc_Thread;

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		packet_Data.sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(packet_Data.sock == INVALID_SOCKET){
			err_display("accept()");
			continue;
		}
		printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", 
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 스레드 생성
		recv_Proc_Thread = CreateThread(NULL, 0, Receive_Process, 
			(LPVOID)packet_Data.sock, 0, NULL);
			//&ThreadId 
		if(recv_Proc_Thread == NULL)
			printf("[오류] 스레드 생성 실패!\n");
		else
			CloseHandle(recv_Proc_Thread);


		g_Data_Thread = CreateThread(NULL, 0, Check_Data, 
			(LPVOID)packet_Data.sock, 0, NULL);
			
			if(g_Data_Thread == NULL)
				printf("[오류] 스레드 생성 실패!\n");
			else
				CloseHandle(g_Data_Thread);

		/*
		if(!Queue_buf.empty())
		{
			g_Data_Thread = CreateThread(NULL, 0, Check_Data, 
			NULL, 0, NULL);
			
			if(g_Data_Thread == NULL)
				printf("[오류] 스레드 생성 실패!\n");
			else
				CloseHandle(g_Data_Thread);
		}
		*/
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}


//int g_nCnt = 0;

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

		// 1. h1 파씽 h2 파씽
		memcpy(&nHeader1, buf+index,sizeof(int));
		index+=sizeof(int);

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
		stDatagram.sock = packet_Data.sock;

		Queue_buf.push(stDatagram);

		//g_nCnt++;
		//printf("g_nCnt:%d\n", g_nCnt);

	}

	

}

void MsgReq(char* szBuf, int index,int NumOfMessage )
{
	char szID[MAX_ID+1] = {0x0,};
	char szMsg[MAX_MSG+1] = {0x0,};

	//HeaderLenght만큼 index이동(x)
	//????????? Start1,Start2만큼 더 이동해야 하는거 아닌가?
	//아니다 queue에는 진짜 데이터만 넣었다
	//nType만큼만 이동해야하는데?...

	
	memcpy(szID,szBuf+index,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+index,MAX_MSG);
	index+=MAX_MSG;
	

	//일단 되는것 
	/*
	index+=sizeof(int);

	memcpy(szID,szBuf+1,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+MAX_ID+1,MAX_MSG);
	index+=MAX_MSG;
	*/
	//printf("메시지 수:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);
	
	printf("메시지 수:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);
}


//데이터를 채크하는 swicth문을 여기로 옮긴다
DWORD WINAPI Check_Data(LPVOID arg)
{
	int index = 0;
	int num_of_Queue = 1;
	int nPacketType = 0;
	PACKET_DATA real_Data;

	int data_Lenght = 0;

	while(1)
	{
		if(!Queue_buf.empty())
		{

			real_Data = Queue_buf.front();

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
					
					Queue_buf.pop();
					num_of_Queue++;
				break;

			}

		}else
		{
			//break;
		}

	}

		


		/*
		//send_Buf에 넣은 버전
		switch( nPacketType )
		{

			case CS_CHAT_MSG_NTY:
				
				for(;index<recv_retval;)
				{
					//index의 길이가 retval보다 커질때까지?
						//데이터가 1개있을때
						////////////////////////////////////
							char szID[MAX_ID+1] = {0x0,};
							char szMsg[MAX_MSG+1] = {0x0,};

							//맞는 헤더인지 check
							//첫번째 인덱스는 패킷의수*289+1
							//비교는 나중에...
							//printf("Num_Of_Packet*289+1+1 : %d\n",Num_Of_Packet*289+1+1);
							
							//n=memcmp(recv_Buf+(Num_Of_Packet*289+1+sizeof(int)),&PACKET_HEADER_START1,sizeof(int));

							//if(recv_Buf[Num_Of_Packet*289+1+sizeof(int)] == PACKET_HEADER_START1 && recv_Buf[Num_Of_Packet*289+1+sizeof(int)+sizeof(int)] == PACKET_HEADER_START2)
							{
								
								//printf("첫번째 인덱스%d\n",index);
								//memcpy(&PACKET_HEADER_START1,buf+index,sizeof(int));
								index+= sizeof(int);

								//memcpy(&PACKET_HEADER_START2,buf+index,sizeof(int));
								index+= sizeof(int);

								memcpy(&PACKET_LENGHT,recv_Buf+index,sizeof(int));
								index += sizeof(int);
								
								memcpy(szID,recv_Buf+index,MAX_ID);
								index+=MAX_ID;

								memcpy(szMsg,recv_Buf+index,MAX_MSG);
								index+=MAX_MSG;

								printf("index:%d,아이디:%s,  메시지:%s,header1:%d ,header2:%d ,lenght:%d\n ",index,szID,szMsg,PACKET_HEADER_START1,PACKET_HEADER_START2,PACKET_LENGHT);
								//512바이트는 채팅메시지를 읽으면됨.
								
								index+=1;
								//printf("패킷의 수: %d\n",Num_Of_Packet);
								Num_Of_Packet++;
								
								//index+=2;
								//다 읽으면 인덱스 +2 ?nPacketType까지 생각해서
							}
							//else
							//{
								//맞지 않는패킷
								//소켓 끊어버리기
								// closesocket()
								//closesocket(client_sock);
								//printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", 
								//	inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

							//}
								
					
				}
				printf("채팅 패킷 recv!!\n");
			
				break;

			
		}
		*/
		
		//recv_Buf[recv_retval] = '\0';
		//recv_retval=0;
		//Num_Of_Packet = 0;

	 index = 0;	
	 
	 printf("받은 총 메시지수:%d\n",num_of_Queue);
	 
	 num_of_Queue = 1;
	
	return 0;
}



