#pragma once
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <list>
#include <map>
#include <io.h>

#define BUFSIZE 210400
#define BUFSIZE_LOGIN 256

const static int MAX_ID		=	20;
const static int MAX_MSG	=	256;
const static int MAX_PASSWORD	=	128;
const static int MAX_CLIENT = 100;


const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);

//실제 데이터의 첫 바이트에 들어가는 데이터 타입;;(헤더에 들어가는게 아님)
const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);

/*
	char szID[20];
	char szMsg[512];
*/








using namespace std;

/*
typedef struct socket_data
	{
		//BUFSIZE 210400
		char szPacket[210400+1];
		CClientSock* pClientSock;

		socket_data()
		{
			memset( szPacket, 0x0, 210400 );
			pClientSock = NULL;
		}
	}SOCKET_DATA;
*/	//여기서는 안되고 ExportData.h 에 넣으면 된다...이유는?
	//굳이 차이점을 찾자면 ClientSock.h가 없는건데..근데 ClientSock.h에는 아무것도 include가 안되있는데...
	//include 순서 차이인가...



class InItServer
{

private:
	int m_Retval;
	SOCKET m_Listen_Sock;
	SOCKADDR_IN m_Serveraddr;

public:
	 InItServer(void);
	~InItServer(void);

	SOCKET _get_Listen_Socket();
	int _get_Retval();

	void _set_Retval(int value);

	void err_quit(char *msg);
};
