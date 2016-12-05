#include "CClientSock.h"

CClientSock::CClientSock(void)
{
	
}

CClientSock::~CClientSock(void)
{
}


//------------------------------------

char* CClientSock::GetIP(SOCKADDR_IN clientAddr)
{
	return inet_ntoa(clientAddr.sin_addr);
}


void CClientSock::SetSockNo(int value)
{
	//g_nTotalSockets
	m_SockNo = value;
}

int CClientSock::GetScokNo()
{
	return m_SockNo;
}

SOCKET CClientSock::Get_CClientSock()
{
	return m_ClientSocket;
}

void CClientSock::Set_CClientSock(SOCKET value)
{
	m_ClientSocket = value;
}



