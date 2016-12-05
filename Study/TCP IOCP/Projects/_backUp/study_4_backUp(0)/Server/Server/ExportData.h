#pragma once
#include "InItServer.h"
#include "CClientSock.h"

using namespace std;


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



class ExportData
{
private:

const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);



public:
	ExportData(void);
	~ExportData(void);

	

	SOCKET_DATA EXPORT_DATA(char buf[], int nTotLen );
};
