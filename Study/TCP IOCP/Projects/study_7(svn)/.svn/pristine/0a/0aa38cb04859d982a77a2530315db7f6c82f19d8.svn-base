#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
 
void ErrorHandling(char *message);

#define MEG_LENGHT 1024

const static int MAX_MSG	=	256;


const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);

//실제 데이터의 첫 바이트에 들어가는 데이터 타입;;(헤더에 들어가는게 아님)
const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);
 
#pragma comment(lib, "ws2_32.lib")

 void Message_End(char* c);

int main()
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");
 
    SOCKET hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(hSocket == INVALID_SOCKET)
        ErrorHandling("socket() error");
 
    SOCKADDR_IN recvAddr;
    memset(&recvAddr, 0, sizeof(recvAddr));
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    recvAddr.sin_port = htons(9000);
 
    if(connect(hSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");
 
    WSAEVENT event = WSACreateEvent();
 
    WSAOVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
 
    overlapped.hEvent = event;
 

    WSABUF dataBuf;
	dataBuf.buf=new char[MEG_LENGHT+1];

    char message[MEG_LENGHT+1] = {0,};
    int sendBytes = 0;
    int recvBytes = 0;
    int flags = 0;
 
    while(true)
	{
		char szMsg[MAX_MSG + 1] = {0x0,};

        flags = 0;
		printf("전송할데이터:\n");
 
        scanf("%s", szMsg);
		Message_End(szMsg);
        if(!strcmp(message, "exit")) break;

		//----------------------------
		int index = 0;
		int PACKET_LENGHT = 0;
		
		//PACKET_HEADER_START1
		memcpy(message+index,&PACKET_HEADER_START1,sizeof(int));	
		index += sizeof(int);
		//PACKET_HEADER_START2
		memcpy(message+index,&PACKET_HEADER_START2,sizeof(int));
		index += sizeof(int);
	
		int nTmpIndex = 0;
		nTmpIndex = index;
		index += sizeof(int);


		//----------------------------------------데이터
		message[index++] = CS_CHAT_MSG_NTY;
		PACKET_LENGHT++;//실제 데이터 길이만큼 더함

		memcpy(message+index,szMsg,MAX_MSG);
		index += MAX_MSG;
		PACKET_LENGHT += MAX_MSG; //실제 데이터 길이만큼 더함

		//----------------------------------------데이터
		memcpy(message+nTmpIndex,&PACKET_LENGHT,sizeof(int));
	
		//----------------------------
 
		
		//printf("strlen(message) :%d\n",strlen(message));
       // dataBuf.len = strlen(message);
		dataBuf.len = MEG_LENGHT;
		
		dataBuf.buf = message;
		//memcpy(dataBuf.buf, message, MEG_LENGHT);
        //dataBuf.buf = message;
 
        if(WSASend(hSocket, &dataBuf, 1, (LPDWORD)&sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR)
        {
            if(WSAGetLastError() != WSA_IO_PENDING)
                ErrorHandling("WSASend() error");
        }
 
        WSAWaitForMultipleEvents(1, &event, TRUE, WSA_INFINITE, FALSE);  
 
        WSAGetOverlappedResult(hSocket, &overlapped, (LPDWORD)&sendBytes, FALSE, NULL);
 
        printf("전송된바이트수: %d \n", sendBytes);
 
        if(WSARecv(hSocket, &dataBuf, 1, (LPDWORD)&recvBytes, (LPDWORD)&flags, &overlapped, NULL) == SOCKET_ERROR)
        {
            if(WSAGetLastError() != WSA_IO_PENDING)
                ErrorHandling("WSASend() error");
        }
 
        printf("Recv[%s]\n",dataBuf.buf);
    
	}

	//closesocket(hSocket);
 
    WSACleanup();
    return 0;
}
 
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
 
    exit(1);
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