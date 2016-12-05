#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 533
#define BUFSIZE_LOGIN 256


void Message_End(char* c);

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

// ����� ���� ������ ���� �Լ�
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

	// ���� �ʱ�ȭ
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
		
	// ������ ��ſ� ����� ����
	


	// ������ ������ ���
	while(1){
		// ������ �Է�
		//ZeroMemory(sendBuf, sizeof(sendBuf));
		
		

		char szID[MAX_ID+1] = {0x0,};
		char szMsg[MAX_MSG+1] = {0x0,};

		char szID_login[MAX_ID+1] = {0x0,};
		char szPasswd_login[MAX_PASSWORD+1]= {0x0,};
		int  nAge_login = 0;
		char szNickName_login[20+1]= {0x0,};

		

		

		printf("\n[���̵� �Է��Ͻÿ�] ");
		fgets(szID, MAX_ID, stdin);	
		Message_End(szID);
	

		printf("\n[�޽����� �Է��Ͻÿ�] ");
		fgets(szMsg, MAX_MSG, stdin);
		Message_End(szMsg);
		


		/*
		char szID[20];
		char szPasswd[128];
		int  nAge;
		char szNickName[20];
		*/
		
		printf("\n[���̵� �Է��Ͻÿ�] ");
		fgets(szID_login, MAX_ID, stdin);
		Message_End(szID);
		

		printf("\n[��й�ȣ�� �Է��Ͻÿ�] ");
		fgets(szPasswd_login, MAX_PASSWORD, stdin);
		Message_End(szPasswd_login);
		

		printf("\n[�г����� �Է��Ͻÿ�] ");
		fgets(szNickName_login, 20, stdin);
		Message_End(szNickName_login);
		

		printf("\n[���̸� �Է��Ͻÿ�] ");
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
		// '\n' ���� ����
		len = strlen(sendBuf);
		if(sendBuf[len-1] == '\n')
			sendBuf[len-1] = '\0';
		if(strlen(sendBuf) == 0)
			break;
		*/

		// ������ ������
		/*
		//retval = send(sock, sendBuf, strlen(sendBuf), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		*/

		//100�� ������?
		//for(int i=0;i<1000;i++)
		//{
			//printf("strlen %d\n",strlen(sendBuf));
			
		//}
		
		
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
		/*

		// ������ �ޱ�
		retval = recvn(sock, sendBuf, retval, 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
			break;
		}
		else if(retval == 0)
			break;
		
		// ���� ������ ���
		sendBuf[retval] = '\0';
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\n", sendBuf);
		*/
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
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