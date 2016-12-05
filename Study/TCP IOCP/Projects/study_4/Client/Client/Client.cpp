#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 210400
#define BUFSIZE_LOGIN 256

//#define PACKET_LEGHT  2+MAX_ID+MAX_MSG+2


void Message_End(char* c);

const static int MAX_ID		=	20;
const static int MAX_MSG	=	256;
const static int MAX_PASSWORD	=	128;

//#define CS_CHAT_MSG_NTY		(BYTE(0x01))	// 요청 _REQ , 통지 _NTY, 응답 _ACK

const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);
/*
	char szID[20];
	char szMsg[512];
*/

const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);


///////////////////////////////////s1,s2,lenght



//const static int PACKET_HEADER_END1		=	((BYTE)0xDD);
//const static int PACKET_HEADER_END2		=	((BYTE)0xCC);






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



// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left > 0){
		received = recv(s, ptr, left, flags);
		if(received == SOCKET_ERROR) 
			return SOCKET_ERROR;
		else if(received == 0) 
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}


int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD( 2 , 2 ), &wsa) != 0)
		return -1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");	
	
	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");
		
	// 데이터 통신에 사용할 변수
	


	// 서버와 데이터 통신
	while(1){
		// 데이터 입력
		//ZeroMemory(sendBuf, sizeof(sendBuf));
		char szID[MAX_ID+1] = {0x0,};
		char szMsg[MAX_MSG+1] = {0x0,};

		printf(" \n[아이디를 입력하시오] ");
		fgets(szID, MAX_ID, stdin);	
		Message_End(szID);

		printf(" \n[메시지를 입력하시오] ");
		fgets(szMsg, MAX_MSG, stdin);
		Message_End(szMsg);
		
		//------------------------
		int index = 0;
		int PACKET_LENGHT = 0;

		char sendBuf[BUFSIZE+1] = {0x0,};
		char recvBuf[BUFSIZE+1] = {0x0,};
	
		//PACKET_HEADER_START1
		memcpy(sendBuf+index,&PACKET_HEADER_START1,sizeof(int));
		index += sizeof(int);
		//PACKET_HEADER_START2
		memcpy(sendBuf+index,&PACKET_HEADER_START2,sizeof(int));
		index += sizeof(int);

		int nTmpIndex = 0;//HeaderLenght자리의 인덱스를 저장할 변수
		nTmpIndex = index;//HeaderLenght자리의 인덱스를 저장해 둔다
		//memcpy(sendBuf+index,&PACKET_LENGHT,sizeof(int));
		//HeaderLenght인덱스 만큼 건너뛴다
		index += sizeof(int);


		//----------------------------------------데이터
		sendBuf[index++] = CS_CHAT_MSG_NTY;
		PACKET_LENGHT++;//실제 데이터 길이만큼 더함

		memcpy(sendBuf+index,szID,MAX_ID);
		index += MAX_ID;
		PACKET_LENGHT += MAX_ID; //실제 데이터 길이만큼 더함

		memcpy(sendBuf+index,szMsg,MAX_MSG);
		index += MAX_MSG;
		PACKET_LENGHT += MAX_MSG; //실제 데이터 길이만큼 더함

		//----------------------------------------데이터

		//headerLenght자리에 PacketLenght를 넣는다
		memcpy(sendBuf+nTmpIndex,&PACKET_LENGHT,sizeof(int));



		for( int i =0; i< 100; i++ )
			{
					// 받은 데이터 출력

					
					//recvBuf[retval] = '\0';
					//printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
					//printf("[받은 데이터] %s\n", recvBuf);
					//printf("recvBuf[0터] %c\n", recvBuf[0]);
					

					//if(recvBuf[0] == SC_ACK)
						//보내면 될때
						//printf("보내면 될때 retval size %d\n",retval);


					retval = send(sock, sendBuf, index, 0);
					printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
					if(retval == SOCKET_ERROR)
					{
						err_display("send()");
						break;
					}

					/*
					// 데이터 받기
					retval = recvn(sock, recvBuf, retval, 0);
					if(retval == SOCKET_ERROR){
						err_display("recv()");
						break;
					}
					else if(retval == 0)
						break;

						// 받은 데이터 출력
						recvBuf[retval] = '\0';
						printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
						printf("[받은 데이터] %s\n", recvBuf);
					*/
			}
			
		//----------------------------------------
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void Message_End(char* c)
{
	int len=0;

	len = strlen(c);
	if(c[len-1] == '\n')
		c[len-1] = '\0';

	//if(strlen(c) == 0)
	//	return 0;
}