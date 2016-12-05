// Test_ODBC_Client.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

struct socketStruct {
   int nrData;
   double data[100];
};

void ErrorHandling(char* message);
int recvn(SOCKET s, char *buf, int len, int flags);

int _tmain(int argc, _TCHAR* argv[])
{

	int portNum = 1234;

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	// char*�� ���� �Ŀ� ���ϴ� ���·� type cast�� �Ѵ�. 
	char message[500];
	socketStruct* RxData;
	int strLen;
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) !=0)
		ErrorHandling("WSAStartup() errer!");

	hSocket=socket(PF_INET, SOCK_STREAM, 0);
	if(hSocket==INVALID_SOCKET)
		ErrorHandling("hSocketet() error!");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	servAddr.sin_port=htons(portNum);

	if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
	

	int maxCnt = 2;

	while(1)
	{
		//Sleep(10);
		strLen = recv(hSocket, message, sizeof(message)-1, 0);
		if(strLen == 0)
		{
			continue;
		}
		// ���� ���� �� ��ȯ���� ���ϴ� ������ �ٲ۴�. 
		message[strLen] = '\0';
		RxData = (socketStruct*)message;
		if(strLen == -1)
			ErrorHandling("read() error!");
		
		int nrData = RxData->nrData;
		// ���� �������� nrData = 0�̸� �����Ѵ�. 
		if(nrData == 0)
			break;
		
		
		
		for(int i = 0; i < nrData; i++)
		{
			printf("MaxCnt: %d\n",maxCnt);
			printf("Message from server:[%d] %.1f \n", i, RxData->data[i]);
		}
		maxCnt--;
	}

	// ���� ���� 
	printf("Closing Socket \n");
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

//�ڽ��� ���� �������� ũ�⸦ �̸� �˰�������
//�� ũ�⸸ŭ ���� ������ recv() �Լ��� ������ ȣ���ϴ� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left>0)
	{
		received = recv(s,ptr,left,flags);
		if(received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if(received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}