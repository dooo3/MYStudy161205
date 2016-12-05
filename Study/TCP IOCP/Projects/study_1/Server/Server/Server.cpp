#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 533
#define BUFSIZE_LOGIN 256

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

int g_count = 0;
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

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	char buf[BUFSIZE+1];
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	
	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while(1){
		// 데이터 받기
		// 1. 커널 송신버퍼에 데이터가 있으면 읽어온다.
		retval = recv(client_sock, buf, BUFSIZE, 0); 

		
		//g_count++;
		if(retval == SOCKET_ERROR){
			printf("retval is SOCKE_ERROR\n");
			err_display("recv()");
			break;
		}
		else if(retval == 0){
			printf("retval is 0\n");
			break;
		}

		int index = 0;
		int nPacketType = buf[index++];

		switch( nPacketType )
		{

			case CS_CHAT_MSG_NTY:

					{
					char szID[MAX_ID+1] = {0x0,};
					char szMsg[MAX_MSG+1] = {0x0,};
					

					strncat(szID,buf+index,MAX_ID);
					index+=MAX_ID;

					strncat(szMsg,buf+index,MAX_MSG);
					index+=MAX_MSG;
					//20바이트는 아이디를 읽고

					printf("아이디:%s,  메시지:%s.",szID,szMsg);
					//512바이트는 채팅메시지를 읽으면됨.
					}

				printf("채팅 패킷 recv!!\n");
				break;

			case CS_LOGIN_REQ:
				
				{
					int index = 1;
					char szID_login[MAX_ID+1] = {0x0,};
					char szPasswd_login[MAX_PASSWORD+1]= {0x0,};
					int  nAge_login = 0;
					char szNickName_login[20+1]= {0x0,};
					
					strncat(szID_login,buf+index,MAX_ID);
					index+=MAX_ID;

					strncat(szPasswd_login,buf+index,MAX_PASSWORD);
					index+=MAX_PASSWORD;

					strncat(szNickName_login,buf+index,20);
					index+=20;
					
					//memcpy( sendBuf_login + index_login, &nAge_login, sizeof(int) );
					//index_login += sizeof(int);

					memcpy(&nAge_login , buf+index, sizeof(int) );
					index += sizeof(int);

					printf("아이디:%s,비밀번호:%s,닉네임:%s,나이:%d",szID_login,szPasswd_login,szNickName_login,nAge_login);
				}
				printf("로그인 패킷 recv!!\n");
				break;
		}
		
		// 받은 데이터 출력
		buf[retval] = '\0';

//		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
//			ntohs(clientaddr.sin_port), buf);



		// 데이터 보내기
		//retval = send(client_sock, buf, retval, 0);
		//if(retval == SOCKET_ERROR){
		//	err_display("send()");
		//	break;
		//}
	}

	// closesocket()
	closesocket(client_sock);
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
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	HANDLE hThread;

	//DWORD ThreadId;

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display("accept()");
			continue;
		}
		printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", 
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient, 
			(LPVOID)client_sock, 0, NULL);
			//&ThreadId 
		if(hThread == NULL)
			printf("[오류] 스레드 생성 실패!\n");
		else
			CloseHandle(hThread);
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}