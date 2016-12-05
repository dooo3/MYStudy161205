#pragma once
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <list>
#include <map>
#include <io.h>

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
