#pragma once
#include "InItServer.h"



class CClientSock
{

private:

	SOCKET m_ClientSocket;
	
	int m_SockNo;
public:
	CClientSock(void);
	~CClientSock(void);

	
	char* GetIP(SOCKADDR_IN clientAddr);
	
	int GetScokNo();
	void SetSockNo(int value);

	SOCKET Get_CClientSock();

	void Set_CClientSock(SOCKET value);

	
	
};
