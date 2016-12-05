#include "InItServer.h"




void InItServer::err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}


InItServer::InItServer(void)
{
	m_Retval = 0;
	
	// socket() 소켓 생성
	m_Listen_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if(m_Listen_Sock == INVALID_SOCKET) err_quit("socket()");	
	
	// bind()
	//서버의 지역 ip주소와 지역포트 번호를 위한 구조체전달
	
	ZeroMemory(&m_Serveraddr, sizeof(m_Serveraddr));
	m_Serveraddr.sin_family = AF_INET;
	m_Serveraddr.sin_port = htons(9000);
	m_Serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_Retval = bind(m_Listen_Sock, (SOCKADDR *)&m_Serveraddr, sizeof(m_Serveraddr));
	if(m_Retval == SOCKET_ERROR) err_quit("bind()");
	
	// listen() - bind()에 의해 처리된 tcp포트의 상태를 LISTEN으로 바꾼다.
	m_Retval = listen(m_Listen_Sock, SOMAXCONN);
	if(m_Retval == SOCKET_ERROR) err_quit("listen()");
	
	//넌블로킹 소켓으로 전환
	unsigned long on = 1;
	m_Retval = ioctlsocket(m_Listen_Sock, FIONBIO, &on);
	if(m_Retval == SOCKET_ERROR)err_quit("ioctlsocket()");

	//SO_RESUSEADDR 소켓 옵션 설정(REUSEADDR)
	bool optReuse = true;
	m_Retval = setsockopt(m_Listen_Sock,SOL_SOCKET,SO_REUSEADDR,
							(char*)&optReuse,sizeof(optReuse));
	if(m_Retval == SOCKET_ERROR) err_quit("setsocketopt()");



}

InItServer::~InItServer(void)
{
}

SOCKET InItServer::_get_Listen_Socket()
{
	return m_Listen_Sock;
}

int InItServer::_get_Retval()
{
	return m_Retval;
}

void InItServer::_set_Retval(int value)
{
	m_Retval = value;
}
