#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>

#include <io.h>



using namespace std;

#define BUFSIZE 210400
#define BUFSIZE_LOGIN 256

const static int MAX_ID		=	20;
const static int MAX_MSG	=	256;
const static int MAX_PASSWORD	=	128;


const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);

//���� �������� ù ����Ʈ�� ���� ������ Ÿ��;;(����� ���°� �ƴ�)
const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);
/*
	char szID[20];
	char szMsg[512];
*/

void EXPORT_DATA(char buf[], int nTotLen );
//�����Ϳ� ���̸� �޾Ƽ� Queue�� �����ϴ� �Լ�


//queue�� ���� ����ü
typedef struct packet_data
{
	char szPacket[BUFSIZE+1];
	SOCKET sock;
	//----------�����߰�
	int recvBytes;
	int sendBytes;
	//----------�����߰�
	packet_data()
	{
		memset( szPacket, 0x0, BUFSIZE );
	}
}PACKET_DATA;




//------------���� ���� ����
int g_nTotalSockets = 0;

PACKET_DATA* g_socket_Info[FD_SETSIZE];
FD_SET g_listen_rset;
FD_SET g_recv_set;
FD_SET g_writ_set;

//----------------------���� ���� �Լ�
int AddSocketInfo(SOCKET clientsock);
void RemoveSocketInfo(int nIndex);

//----------------------���� ���� �Լ�

//����ü queue����
queue<PACKET_DATA> Queue_recv_buf;

//queue<PACKET_DATA> Queue_send_buf;


//~~~~~~~~������ �Լ���
DWORD WINAPI Check_Data();
DWORD WINAPI Send_Process();
DWORD WINAPI Receive_Process();

/////////////////////20161010

/////////////////////20161011



// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
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

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}





int main(int argc, char* argv[])
{
	int retval;

	//fd_set readfds, allfds;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;
	// socket() ���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");	
	
	
	//long port;
	//port = atol(argv[1]);

	// bind()
	//������ ���� ip�ּҿ� ������Ʈ ��ȣ�� ���� ����ü����
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");
	
	// listen() - bind()�� ���� ó���� tcp��Ʈ�� ���¸� LISTEN���� �ٲ۴�.
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");
	
	//�ͺ��ŷ �������� ��ȯ
	unsigned long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if(retval == SOCKET_ERROR)err_quit("ioctlsocket()");

	//SO_RESUSEADDR ���� �ɼ� ����(REUSEADDR)
	bool optReuse = true;
	retval = setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,
							(char*)&optReuse,sizeof(optReuse));
	if(retval == SOCKET_ERROR) err_quit("setsocketopt()");

	// ������ ��ſ� ����� ����
	
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrlen;

	HANDLE data_chec_Thread;
	HANDLE recv_Proc_Thread;
	HANDLE send_Thread;

	//���� �� �ʱ�ȭ
	FD_ZERO(&g_listen_rset);  
	FD_ZERO(&g_recv_set);  
	FD_ZERO(&g_writ_set);

	//���� �� ����
	FD_SET(listen_sock,&g_listen_rset);

	// ������üŷ ���������
	data_chec_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Check_Data,  NULL, 0, NULL);

	// recv������ ����
	recv_Proc_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Receive_Process, NULL , 0, NULL);

	//send_Thread Create
	send_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Send_Process, NULL , 0, NULL);


	
	if(data_chec_Thread == NULL)
		printf("[����] ������ ���� ����!\n");
	else
		CloseHandle(data_chec_Thread);

	if(send_Thread == NULL)
		printf("[����] ������ ���� ����!\n");
	else
		CloseHandle(send_Thread);

	if(recv_Proc_Thread == NULL)
		printf("[����] ������ ���� ����!\n");
	else
		CloseHandle(recv_Proc_Thread);

	while(1){

		// 1. SELECT() ������ �б�� ���� ���°� ����� ������ ������ ����.
		// 2. SELECT() �б�¿� ������ �ƾ� ������ �׳� �ٷθ���.
		// 3. ���� : loop�� ��� �ȵ��ϱ� �ڿ��Ҹ� ����. -> ��������� �����Լ� ȣ���Ͽ� Ȯ���� �ʿ����.
		// 4. ���� : select ���ϰ��� Ư�������� �����ϴ°� �ƴϱ⿡, ��� ���Ͽ� ���� FD_ISSET() �˻縦 �ؾ���.(ū������X)
		retval = select( 2 , &g_listen_rset , NULL, NULL , NULL);
		if( retval > 0 )
		{
			if( FD_ISSET(listen_sock,&g_listen_rset) )	
			{
				addrlen = sizeof(clientAddr);
				
				clientSocket = accept(listen_sock,(SOCKADDR*)&clientAddr,&addrlen);
				if(clientSocket == INVALID_SOCKET)
				{
					if((WSAGetLastError() != WSAEWOULDBLOCK))
					{
						err_quit("accept()");
					}
				}else
				{
					// ������ �Ǹ�
					printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
						inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
					//Sleep(10);

					//���� ���� �߰�
					int n_sock_no = AddSocketInfo(clientSocket);
					if( n_sock_no == -1 )
					{
						printf("[TCP ����] Ŭ���̾�Ʈ ���������� �մϴ�n");
						closesocket(clientSocket);	
					}
		
				}//else{//������ �Ǹ�..
			} //(FD_ISSET(listen_sock,&g_recv_set))	
		}

	

	} //while end
	return 0;
}






///
// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI Send_Process()
{
	SOCKADDR_IN clientAddr;
	int addrlen;
	int retval;

	addrlen = sizeof(clientAddr);

	while(1)
	{
		//������ Ŭ���̾�Ʈ1���� �����ؼ� ������ ������ 1�� 
		//������ �� Ŭ���̾�Ʈ�� ���ӽ�...
		for(int i = 0; i < g_nTotalSockets; ++i)
		{
			PACKET_DATA* pInfo = g_socket_Info[i];
			if(FD_ISSET(pInfo->sock, &g_writ_set))
			{
				// �����ͺ�����
				retval = send(pInfo->sock, pInfo->szPacket + pInfo->sendBytes,
					pInfo->recvBytes - pInfo->sendBytes, 0);


				if(retval == SOCKET_ERROR)
				{
					if(WSAGetLastError() != WSAEWOULDBLOCK)
					{
						err_display("send()");
						RemoveSocketInfo(i);
						continue;
					}
				}

				pInfo->sendBytes += retval;
				if(pInfo->recvBytes == pInfo->sendBytes)
					pInfo->recvBytes = pInfo->sendBytes = 0;
			}//if(FD_ISSET(pInfo->sock, &g_writ_set))
		}//for
		
		//Sleep(1);
		//delay()
	}

	return 0;
}


DWORD WINAPI Receive_Process()
{
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;

	addrlen = sizeof(clientaddr);

	while(1)
	{
		
		retval = select( g_nTotalSockets+1 ,&g_recv_set,NULL,NULL,NULL);
		if(retval>0) 
		{
			for(int i = 0; i < g_nTotalSockets; ++i)
			{
				PACKET_DATA* pInfo = g_socket_Info[i];

				if(FD_ISSET(pInfo->sock, &g_recv_set))
				{
					// �����͹ޱ�
					retval = recv(pInfo->sock, pInfo->szPacket, BUFSIZE, 0);

					if(retval == SOCKET_ERROR)
					{
						if(WSAGetLastError() != WSAEWOULDBLOCK)
						{
							err_display("recv()");
							RemoveSocketInfo(i);
							continue;
						}
					}
					else if(retval == 0)
					{
						RemoveSocketInfo(i);
						continue;
					}
					else
					{
						pInfo->recvBytes = retval;

						// ���� ������ ���
						SOCKADDR_IN sockAddr;
						int nAddrLength = sizeof(sockAddr);
						// Ŭ���̾�Ʈ ���� ���
						getpeername(pInfo->sock, (SOCKADDR*)&sockAddr, &addrlen);
						//���� ��Ŷ �м�
						EXPORT_DATA(pInfo->szPacket,retval);
						
						pInfo->szPacket[retval] = '\0';
						printf("[TCP/%s:%d] %s\n",
							inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port), pInfo->szPacket);
					}
				}
			}
		}


	}

		
	
	return 0;
}





void EXPORT_DATA(char buf[], int nTotLen)
{
	int index = 0;
	
	PACKET_DATA stDatagram;

	int nDatagramLen = 0;
	int nHeader1 = 0;
	int nHeader2 = 0;
	
	while(index < nTotLen)
	{
		nDatagramLen = 0;
		nHeader1 = 0;
		nHeader2 = 0;
		memset( &stDatagram, 0x0, sizeof( PACKET_DATA ) );

		// 1. h1 �ľ� 
		memcpy(&nHeader1, buf+index,sizeof(int));
		index+=sizeof(int);
		// 2. h2 �ľ�
		memcpy(&nHeader2, buf+index,sizeof(int));
		index+=sizeof(int);

		if( nHeader1 != PACKET_HEADER_START1 || nHeader2 != PACKET_HEADER_START2 )
		{
			// ���� ����.. ���߿� �߰�.
			return;
		}

		// 2. ���� ��������
		memcpy(&nDatagramLen, buf+index,sizeof(int));
		index += sizeof(int);
	
		//3. ���̸�ŭ ������ �ܾ����
		memcpy( stDatagram.szPacket, buf+index, nDatagramLen );
		index += nDatagramLen;
		
		//���ϵ� ���� �־��ְ�

		Queue_recv_buf.push(stDatagram);

		//Queue_send_buf.push(stDatagram);

		//g_nCnt++;
		//printf("g_nCnt:%d\n", g_nCnt);

	}
}


void MsgReq(char* szBuf, int index,int NumOfMessage )
{
	char szID[MAX_ID+1] = {0x0,};
	char szMsg[MAX_MSG+1] = {0x0,};

	
	//????????? Start1,Start2��ŭ �� �̵��ؾ� �ϴ°� �ƴѰ�?
	//�ƴϴ� queue���� ��¥ �����͸� �־���
	//nType��ŭ�� �̵��ؾ��ϴµ�?...

	
	memcpy(szID,szBuf+index,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+index,MAX_MSG);
	index+=MAX_MSG;
	
	printf("�޽��� ��:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);

	//�ϴ� �Ǵ°� 
	/*
	index+=sizeof(int);

	memcpy(szID,szBuf+1,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+MAX_ID+1,MAX_MSG);
	index+=MAX_MSG;
	*/
	//printf("�޽��� ��:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);
	
}


//�����͸� äũ�ϴ� swicth���� ����� �ű��
DWORD WINAPI Check_Data()
{
	int index = 0;
	int num_of_Queue = 1;
	int nPacketType = 0;
	PACKET_DATA real_Data;

	int data_Lenght = 0;

	while(1)
	{
		if(!Queue_recv_buf.empty())
		{

			real_Data = Queue_recv_buf.front();

			data_Lenght = real_Data.szPacket[sizeof(int) + sizeof(int) + sizeof(int)];
			real_Data.szPacket[data_Lenght+1];

			//����� RealData�� ���ϱ� ����� HeaderLenght��ŭ �־�� �ϴ°� �ƴѰ�?

		
			
			//real_Data.szPacket[BUFSIZE+1];  //��� �ּ�ó�� �� ��
			//memset(real_Data.szPacket, 0x0, sizeof(BUFSIZE) );

			index = 0;
			
			nPacketType = real_Data.szPacket[index++];

			switch(nPacketType)
			{
				case CS_CHAT_MSG_NTY:
					MsgReq(real_Data.szPacket, index,num_of_Queue);
					
					//printf("ť����:%d ,index:%d,���̵�:%s,  �޽���:%s,header1:%d ,header2:%d ,lenght:%d\n ",num_of_Queue,index,szID,szMsg,PACKET_HEADER_START1,PACKET_HEADER_START2,PACKET_LENGHT);
					
					Queue_recv_buf.pop();
					num_of_Queue++;
				break;

			}

		}else
		{
			//break;
		}



	}

		

	 index = 0;	
	 
	 printf("���� �� �޽�����:%d\n",num_of_Queue);
	 
	 num_of_Queue = 1;
	
	return 0;
}


int AddSocketInfo(SOCKET clientsock)
{
	if(g_nTotalSockets >= (FD_SETSIZE-1)){//MAX üũ
		printf("[����] ���������� �߰��� �� �����ϴ�.");
		return -1;
	}

	PACKET_DATA *ptr = new PACKET_DATA;
	if(ptr == NULL)
	{
		printf("[����] �޸𸮰� �����մϴ�.\n");
		return -1;
	}

	ptr->sock = clientsock;
	ptr->recvBytes = 0;
	ptr->sendBytes = 0;

	FD_SET(clientsock, &g_recv_set);
	FD_SET(clientsock, &g_writ_set);	
	
	g_socket_Info[g_nTotalSockets++] = ptr;

	return g_nTotalSockets-1;
}

void RemoveSocketInfo(int nIndex)
{
	PACKET_DATA* pInfo = g_socket_Info[nIndex];

	// Ŭ���̾�Ʈ ���� ���
	SOCKADDR_IN socketAddr;
	int addrlen = sizeof(socketAddr);
	getpeername(pInfo->sock, (SOCKADDR*)&socketAddr, &addrlen);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n",
		inet_ntoa(socketAddr.sin_addr), ntohs(socketAddr.sin_port));

	closesocket(pInfo->sock);
	delete pInfo;

	for(int i = nIndex; i < g_nTotalSockets; ++i)
		g_socket_Info[i] = g_socket_Info[i + 1];

	g_nTotalSockets--;
}


