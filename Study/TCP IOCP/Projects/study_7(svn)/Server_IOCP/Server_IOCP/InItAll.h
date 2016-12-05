#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>
#include <iostream>
#include <queue>
#include <io.h>

#define BUFSIZE 201400
#define ID_LENGHT 20
#define MEG_LENGHT 1024

const static int MAX_MSG	=	256;

const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);

//실제 데이터의 첫 바이트에 들어가는 데이터 타입;;(헤더에 들어가는게 아님)111
const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);





typedef struct socket_data//소켓정보를구조체화
{
    SOCKET hClientSock;
    SOCKADDR_IN clientAddr;
	char clientid[ ID_LENGHT ];
} SOCKET_DATA, *PPER_SOCK_HANDLE_DATA;
 
typedef struct io_data// 소켓의버퍼정보를구조체화.
{
    OVERLAPPED overlapped; // 현재 완료된 입출력 정보를 얻어 낼때 사용 된다.
    char buffer[BUFSIZE];
    WSABUF wsaBuf;
} IO_DATA, *PPER_IO_HANDLE_DATA;

typedef struct real_data
{
    char buffer[BUFSIZE];
	SOCKET sock;
} REAL_DATA;


class InItAll
{
public:
	InItAll(void);
	~InItAll(void);

	void ErrorHandling(char *message);
};
