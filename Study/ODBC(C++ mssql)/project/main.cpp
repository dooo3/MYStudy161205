


#ifdef WIN32 //Windows Headers

#include "stdafx.h"

	WSADATA WsaData;
	WSAStartup(0x0101,&WsaData);;//winsock1.1
	
	#define NET_INVALID_SOCKET INVALID_SOCKET
	#define NET_SOCKET_ERROR SOCKET_ERROR
	
	#define SLEEP(msec)    Sleep(msec);
	
	typedef int socklen_t;	

#else //Unix Headers

	#include <stdio.h>
	#include <stdlib.h>//for exit()
	#include <string.h>
	#include <sys/types.h>
	#include <sys/socket.h>//for socket(),bind(),connect()
	#include <netinet/in.h>
	#include <arpa/inet.h>//for sockaddr_in,inet_ntoa()
	#include <unistd.h>//for close()
	
	#define NET_INVALID_SOCKET -1
	#define NET_SOCKET_ERROR -1
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define closesocket(sock)close(sock)
	
	typedef unsigned short WORD;
	typedef unsigned long DWORD;
	typedef char *LPSTR;
	typedef const char *LPCSTR;
	typedef const char *LPCTSTR;
	
	typedef int SOCKET;
	
	typedef sockaddr_in SOCKADDR_IN;
	typedef sockaddr SOCKADDR;
	//typedef close closesocket;
	
	#define SLEEP(msec) usleep((msec*1000));	

#endif


/*
 \ *BF\B9\B8\A6\B5\E9\BE\EE 
 WINDOWS : INT64
 LINUX : INTEGER64
 
 
 #ifdef WIN32
 TYPEDEF INT64 _INT64;
 #else
 TYPEDEF INTEGER64 _INT64;
 #endif
*/

struct socketStruct {
	int nrData;
	double data[100];
};


void ErrorHandling(char* message);

void Data_Send(SOCKET hClntSock,SOCKET hServSock,socketStruct txData, socketStruct TermData);
int recvn(SOCKET s, char *buf, int len, int flags);

int main()
{
	int portNum = 1234;
	
	socketStruct txData, TermData;
	txData.nrData = 3;
	for(int i = 0; i<txData.nrData; i++)
	{
		txData.data[i] = i*10;
	}
	TermData.nrData = 0;
	
	//WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;
	
	int szClntAddr;
	char message[]="Hello World!";
	
	//if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
	//	ErrorHandling("WSAStartup() error!");
	
	hServSock=socket(PF_INET, SOCK_STREAM, 0);
	if(hServSock==INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port = htons(portNum);
	
	if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	
	if(listen(hServSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");
	
	szClntAddr=sizeof(clntAddr);
	
	while(1)
	{
		
		printf("[Server] Holding at accept \n");
#ifdef WIN32
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);  
#else
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, (socklen_t*)&szClntAddr);  
#endif
		
		
		
		
		printf("[Server] accept !! \n");
		
		if(hClntSock==INVALID_SOCKET)
		{
			ErrorHandling("accept() error");
			continue;
		}
		
		Data_Send(hClntSock,hServSock,txData,TermData);
		
	}
	
	
	
	printf("Closing Socket \n");
	closesocket(hClntSock);
	closesocket(hServSock);
	
#ifdef WIN32
	WSACleanup();
#else
	close(hClntSock);
	close(hServSock);
#endif
	
	
	
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void Data_Send(SOCKET hClntSock,SOCKET hServSock,socketStruct txData, socketStruct TermData)
{
	int maxCnt = 2;
	while(1)
	{
		printf("maxCnt: %d \n",maxCnt);
		send(hClntSock, (char*)&txData, sizeof(txData), 0);
		
#ifdef WIN32
		Sleep(100);
#else
		sleep(5);
#endif
		
		
		
		maxCnt--;
		if(maxCnt < 0)
		{
			printf("maxCnt: %d \n",maxCnt);
			break;
		}
	}
	
	send(hClntSock, (char*)&TermData, sizeof(TermData), 0);
	
	
}


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
