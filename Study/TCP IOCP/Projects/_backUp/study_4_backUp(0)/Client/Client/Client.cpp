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

//#define CS_CHAT_MSG_NTY		(BYTE(0x01))	// ��û _REQ , ���� _NTY, ���� _ACK

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
		
	// ������ ��ſ� ����� ����
	


	// ������ ������ ���
	while(1){
		// ������ �Է�
		//ZeroMemory(sendBuf, sizeof(sendBuf));
		char szID[MAX_ID+1] = {0x0,};
		char szMsg[MAX_MSG+1] = {0x0,};

		printf(" \n[���̵� �Է��Ͻÿ�] ");
		fgets(szID, MAX_ID, stdin);	
		Message_End(szID);

		printf(" \n[�޽����� �Է��Ͻÿ�] ");
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

		int nTmpIndex = 0;//HeaderLenght�ڸ��� �ε����� ������ ����
		nTmpIndex = index;//HeaderLenght�ڸ��� �ε����� ������ �д�
		//memcpy(sendBuf+index,&PACKET_LENGHT,sizeof(int));
		//HeaderLenght�ε��� ��ŭ �ǳʶڴ�
		index += sizeof(int);


		//----------------------------------------������
		sendBuf[index++] = CS_CHAT_MSG_NTY;
		PACKET_LENGHT++;//���� ������ ���̸�ŭ ����

		memcpy(sendBuf+index,szID,MAX_ID);
		index += MAX_ID;
		PACKET_LENGHT += MAX_ID; //���� ������ ���̸�ŭ ����

		memcpy(sendBuf+index,szMsg,MAX_MSG);
		index += MAX_MSG;
		PACKET_LENGHT += MAX_MSG; //���� ������ ���̸�ŭ ����

		//----------------------------------------������

		//headerLenght�ڸ��� PacketLenght�� �ִ´�
		memcpy(sendBuf+nTmpIndex,&PACKET_LENGHT,sizeof(int));



		for( int i =0; i< 100; i++ )
			{
					// ���� ������ ���

					
					//recvBuf[retval] = '\0';
					//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
					//printf("[���� ������] %s\n", recvBuf);
					//printf("recvBuf[0��] %c\n", recvBuf[0]);
					

					//if(recvBuf[0] == SC_ACK)
						//������ �ɶ�
						//printf("������ �ɶ� retval size %d\n",retval);


					retval = send(sock, sendBuf, index, 0);
					printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
					if(retval == SOCKET_ERROR)
					{
						err_display("send()");
						break;
					}

					/*
					// ������ �ޱ�
					retval = recvn(sock, recvBuf, retval, 0);
					if(retval == SOCKET_ERROR){
						err_display("recv()");
						break;
					}
					else if(retval == 0)
						break;

						// ���� ������ ���
						recvBuf[retval] = '\0';
						printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
						printf("[���� ������] %s\n", recvBuf);
					*/
			}
			
		//----------------------------------------
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