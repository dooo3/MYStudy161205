#include "define.h"

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
	
#ifdef WIN32
	 WSADATA    wsaData;
	 if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0 )
	 {
		 printf("error");
	 }
#else
#endif

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
		
		SLEEP(100);
		
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
