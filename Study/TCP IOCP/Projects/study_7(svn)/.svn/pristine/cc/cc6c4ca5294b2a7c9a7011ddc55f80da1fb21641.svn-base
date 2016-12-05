#include "InItAll.h"

InItAll::InItAll(void)
{
	WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
		InItAll::ErrorHandling("WSAStartup() error!");
}

InItAll::~InItAll(void)
{
}


void InItAll::ErrorHandling(char *message)
{
	fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}