#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 533
#define BUFSIZE_LOGIN 256

const static int MAX_ID		=	20;
const static int MAX_MSG	=	512;
const static int MAX_PASSWORD	=	128;



//#define CS_CHAT_MSG_NTY		(BYTE(0x01))	// ��û _REQ , ���� _NTY, ���� _ACK

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
// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	char buf[BUFSIZE+1];
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	
	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while(1){
		// ������ �ޱ�
		// 1. Ŀ�� �۽Ź��ۿ� �����Ͱ� ������ �о�´�.
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
					//20����Ʈ�� ���̵� �а�

					printf("���̵�:%s,  �޽���:%s.",szID,szMsg);
					//512����Ʈ�� ä�ø޽����� �������.
					}

				printf("ä�� ��Ŷ recv!!\n");
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

					printf("���̵�:%s,��й�ȣ:%s,�г���:%s,����:%d",szID_login,szPasswd_login,szNickName_login,nAge_login);
				}
				printf("�α��� ��Ŷ recv!!\n");
				break;
		}
		
		// ���� ������ ���
		buf[retval] = '\0';

//		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
//			ntohs(clientaddr.sin_port), buf);



		// ������ ������
		//retval = send(client_sock, buf, retval, 0);
		//if(retval == SOCKET_ERROR){
		//	err_display("send()");
		//	break;
		//}
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
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
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// ������ ����
		hThread = CreateThread(NULL, 0, ProcessClient, 
			(LPVOID)client_sock, 0, NULL);
			//&ThreadId 
		if(hThread == NULL)
			printf("[����] ������ ���� ����!\n");
		else
			CloseHandle(hThread);
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}