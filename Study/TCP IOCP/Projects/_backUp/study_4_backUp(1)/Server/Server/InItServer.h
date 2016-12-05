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

//void EXPORT_DATA(char buf[], int nTotLen );
//데이터와 길이를 받아서 Queue에 저장하는 함수



using namespace std;

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
