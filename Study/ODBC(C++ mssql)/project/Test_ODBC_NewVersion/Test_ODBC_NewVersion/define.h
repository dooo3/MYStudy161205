
#pragma once


#ifdef WIN32			

	#include <stdio.h>
	#include <tchar.h>
	#include <stdlib.h>
	#include <winsock2.h>

	#define NET_INVALID_SOCKET INVALID_SOCKET
	#define NET_SOCKET_ERROR SOCKET_ERROR
	#define SLEEP(msec) Sleep(msec)

	typedef int socklen_t;
#else

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
	
	#define SLEEP(msec) usleep((msec*1000))

#endif