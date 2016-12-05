#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 533
#define BUFSIZE_LOGIN 256


void Message_End(char* c);

const static int MAX_ID		=	20;
const static int MAX_MSG	=	512;
const static int MAX_PASSWORD	=	128;

//#define CS_CHAT_MSG_NTY		(BYTE(0x01))	// 요청 _REQ , 통지 _NTY, 응답 _ACK

const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);
/*
	char szID[20];
	char szMsg[512];
*/

const static int CS_LOGIN_REQ		=	((BYTE)0x02);
/*
	char szID[20];
	char szPasswd[128];
	int  nAge;
	char szNickName[20];
*/

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
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
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

		char szID_login[MAX_ID+1] = {0x0,};
		char szPasswd_login[MAX_PASSWORD+1]= {0x0,};
		int  nAge_login = 0;
		char szNickName_login[20+1]= {0x0,};

		

		

		printf("\n[아이디를 입력하시오] ");
		fgets(szID, MAX_ID, stdin);	
		Message_End(szID);
	

		printf("\n[메시지를 입력하시오] ");
		fgets(szMsg, MAX_MSG, stdin);
		Message_End(szMsg);
		


		/*
		char szID[20];
		char szPasswd[128];
		int  nAge;
		char szNickName[20];
		*/
		
		printf("\n[아이디를 입력하시오] ");
		fgets(szID_login, MAX_ID, stdin);
		Message_End(szID);
		

		printf("\n[비밀번호를 입력하시오] ");
		fgets(szPasswd_login, MAX_PASSWORD, stdin);
		Message_End(szPasswd_login);
		

		printf("\n[닉네임을 입력하시오] ");
		fgets(szNickName_login, 20, stdin);
		Message_End(szNickName_login);
		

		printf("\n[나이를 입력하시오] ");
		scanf("%d",&nAge_login);
		
		

		//------------------------
		int index = 0;
		char sendBuf[BUFSIZE+1] = {0x0,};
		sendBuf[index++] = CS_CHAT_MSG_NTY;

		strncat(sendBuf+index,szID,MAX_ID);
		index += MAX_ID;

		strncat(sendBuf+index,szMsg,MAX_MSG);
		index += MAX_MSG;

		retval = send(sock, sendBuf, index, 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		//----------------------------------------


		//--------------------------------------
		int index_login =0;
		char sendBuf_login[BUFSIZE_LOGIN +1] = {0x0,};
		sendBuf_login[index_login++] = CS_LOGIN_REQ;


		memcpy(sendBuf_login+index_login,szID_login,MAX_ID);
		index_login +=MAX_ID;

		strncat(sendBuf_login+index_login,szPasswd_login,MAX_PASSWORD);
		index_login +=MAX_PASSWORD;

		strncat(sendBuf_login+index_login,szNickName_login,20);
		index_login +=20;
		

		memcpy( sendBuf_login + index_login, &nAge_login, sizeof(int) );
		index_login += sizeof(int);
		//memcpy(sendBuf_login+140,&nAge_login,sizeof(char));
		//memset(sendBuf_login+140,nAge_login,100);
		//itoa(nAge_login,sendBuf_login+140,10);

		

		retval = send(sock, sendBuf_login, index_login, 0);
		
				if(retval == SOCKET_ERROR){
				err_display("send()");
				break;
				}

		//--------------------------------------------------



		//sendBuf_login[140] = nAge_login;


		/*
		for(int i=10;i<30;i++)
		{
			printf("sendBuf[%d] Msg : %c\n",i,sendBuf[i]);
		}*/
		
		

		/*	
		// '\n' 문자 제거
		len = strlen(sendBuf);
		if(sendBuf[len-1] == '\n')
			sendBuf[len-1] = '\0';
		if(strlen(sendBuf) == 0)
			break;
		*/

		// 데이터 보내기
		/*
		//retval = send(sock, sendBuf, strlen(sendBuf), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		*/

		//100번 보내기?
		//for(int i=0;i<1000;i++)
		//{
			//printf("strlen %d\n",strlen(sendBuf));
			
		//}
		
		
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
		/*

		// 데이터 받기
		retval = recvn(sock, sendBuf, retval, 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
			break;
		}
		else if(retval == 0)
			break;
		
		// 받은 데이터 출력
		sendBuf[retval] = '\0';
		printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", sendBuf);
		*/
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