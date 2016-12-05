#include "InItServer.h"
#include "CClientSock.h"
#include "ExportData.h"

#define BUFSIZE 210400
#define BUFSIZE_LOGIN 256

const static int MAX_ID		=	20;
const static int MAX_MSG	=	256;
const static int MAX_PASSWORD	=	128;
const static int MAX_CLIENT = 100;


const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);

//���� �������� ù ����Ʈ�� ���� ������ Ÿ��;;(����� ���°� �ƴ�)
const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);
/*
	char szID[20];
	char szMsg[512];
*/

//void EXPORT_DATA(char buf[], int nTotLen );
//�����Ϳ� ���̸� �޾Ƽ� Queue�� �����ϴ� �Լ�


//queue�� ���� ����ü
/*
typedef struct socket_data
{
	char szPacket[BUFSIZE+1];
	CClientSock* pClientSock;

	socket_data()
	{
		memset( szPacket, 0x0, BUFSIZE );
		pClientSock = NULL;
	}
}SOCKET_DATA;

*/
///////////////////////



//------------���� ���� ����
int g_nTotalSockets = 0;
int g_nTotalClient = 0;



CClientSock g_ClientSock[MAX_CLIENT];	// <--������ ����.
// xml, conf, text <-- PORT, MAX_CLIENT, AGENT_IP, AGENT_PORT ����� ����.
// �α� �ý��� : "2016-10-14.text","2016-10-15.text" ������ ����� 


SOCKET_DATA* g_socket_Info[FD_SETSIZE];
FD_SET g_listen_rset;
FD_SET g_recv_set;
FD_SET g_writ_set;

//----------------------���� ���� �Լ�
int AddSocketInfo(SOCKET clientsock,int numofClient);
//void RemoveSocketInfo(int nIndex);


//-----------------------------------------------------2016-10-14
void ResetSocketInfo(int nIndex);
void SetAllSocketInfo(int max_NumOfClient);
void InsertClientAtSocket(int numofClient, InItServer InItServ, SOCKADDR_IN clientAddr, int addrlen);

//----------------------���� ���� �Լ�

//����ü queue����
queue<SOCKET_DATA> g_Queue_recv_buf;
//2016 10 17
//g_Qeueu_recv_buf�� InItServ.h ���� ���������ϰ� get set�Լ��� �Ἥ �����ϸ� export�� ���� Ŭ������ ���� ����
//�ƴϸ� �׳� exprot�Լ��� ���ϰ��� �༭ queue�� ������ �ǰڳ� �̰� �� ���ϰڴ�
//g_Queue_recv_buf.push(EXport_data());

//2016-10-13
//list ����


//2016-10-14

//list�� map���� �ٲٱ�
map<int,CClientSock> g_map_CClientSock;
//pair<map<int,CClientSock>::iterator> g_map_client_itor;
//pair<int,CClientSock> g_pair_map_CClientSock;

map<int,CClientSock>::iterator g_map_client_itor;


//queue<SOCKET_DATA> Queue_send_buf;


//~~~~~~~~������ �Լ���
DWORD WINAPI Check_Data();
DWORD WINAPI Send_Process();
DWORD WINAPI Receive_Process();

/////////////////////20161010

/////////////////////20161011

////////////////////20161017
void Queue_Check(queue<SOCKET_DATA> temp_Queue_recv_buf);




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
	
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;


/*
	// socket() ���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");	
	
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


*/


	// ������ ��ſ� ����� ����
	
	
	
	//SOCKET clientSocket;
	//SOCKADDR_IN clientAddr;
	//int addrlen;

	InItServer InitServ;
	//--------------------------------2016--10--14
	SetAllSocketInfo(MAX_CLIENT);
	
	//2016-10-17

	// ������ ��ſ� ����� ����
	
	//���⼭ setAllSocketInfo�� ���ְ�
	SOCKADDR_IN clientAddr;
	int addrlen;

	HANDLE data_chec_Thread;
	HANDLE recv_Proc_Thread;
	HANDLE send_Thread;

	//���� �� �ʱ�ȭ
	FD_ZERO(&g_listen_rset);  
	FD_ZERO(&g_recv_set);  
	FD_ZERO(&g_writ_set);

	//-------------1014
	//���� �� ����
	FD_SET(InitServ._get_Listen_Socket(),&g_listen_rset);
	//-------------1014
	
	// ������üŷ ���������
	data_chec_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Check_Data,  NULL, 0, NULL);

	// recv������ ����
	recv_Proc_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Receive_Process, NULL , 0, NULL);

	//send_Thread Create
	send_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Send_Process, NULL , 0, NULL);

	
	if(data_chec_Thread == NULL)
		printf("[����]  data_chec_Thread ���� ����!\n");
	else
		CloseHandle(data_chec_Thread);
	
	if(recv_Proc_Thread == NULL)
		printf("[����] recv_Proc_Thread ���� ����!\n");
	else
		CloseHandle(recv_Proc_Thread);

	if(send_Thread == NULL)
		printf("[����] send_Thread ���� ����!\n");
	else
		CloseHandle(send_Thread);

	
		

	while(1){
		
		//���� �� ����
		
		// SELECT ���ϵǴ� ���� : (1) set�ȿ� ���Ұ� ������ �ٷ� ���� : ���ϰ�-1
		//									(2) set�ȿ� ���Ұ� ������ �����߿� ���°� �ٲ� ���Ұ� ������� ����. : ���ϰ� : ���°��ٲ� ���ҵ鰹��
		InitServ._set_Retval( select( 2 , &g_listen_rset , NULL, NULL , NULL) );

		if( InitServ._get_Retval() > 0 )
		{
			if( FD_ISSET(InitServ._get_Listen_Socket(),&g_listen_rset) )	
			{
				
				addrlen = sizeof(clientAddr);
				//���ӿ� ����

				//clientAddr�ʱ�ȭ
				memset(&clientAddr,0,sizeof(clientAddr));

				InsertClientAtSocket(g_nTotalClient, InitServ ,clientAddr,addrlen);

				// 1. ��ü ����
				// 2. ��ü ��������� ���ϼ���
				// 3. ����Ʈ�� ��ü�߰�.
				
			} 
		}

	

	} //while end


	

	return 0;
}






///
// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI Send_Process()
{

	/*

	//SOCKADDR_IN clientAddr;
	//int addrlen;
	//addrlen = sizeof(clientAddr);

	int retval;

	while(1)
	{
		//������ Ŭ���̾�Ʈ1���� �����ؼ� ������ ������ 1�� 
		//������ �� Ŭ���̾�Ʈ�� ���ӽ�...
		for(int i = 0; i < g_nTotalSockets; ++i)
		{
			SOCKET_DATA* pInfo = g_socket_Info[i];

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

*/
	return 0;

}


DWORD WINAPI Receive_Process()
{
	//SOCKADDR_IN clientaddr;
	//int addrlen;
	//addrlen = sizeof(clientaddr);

	ExportData export_Data;

	int retval;

	while(1)
	{
		retval = select( g_nTotalSockets+1 ,&g_recv_set, NULL,NULL,NULL);

		if(retval>0) 
		{
		
			if( !g_Queue_recv_buf.empty() )
					{
						queue<SOCKET_DATA> temp_queue_recv_buf;
						temp_queue_recv_buf = g_Queue_recv_buf;

							g_map_client_itor = g_map_CClientSock.find(g_nTotalClient);
		

						temp_queue_recv_buf.front().pClientSock = &g_map_client_itor->second;

						if(temp_queue_recv_buf.front().pClientSock->GetScokNo() ==g_map_client_itor->first)
						{
							if(FD_ISSET(g_map_client_itor->second.Get_CClientSock(), &g_recv_set))
							{
								// �����͹ޱ�
								retval = recv(g_map_client_itor->second.Get_CClientSock(), g_Queue_recv_buf.front().szPacket, BUFSIZE, 0);
								
								if(retval == SOCKET_ERROR)
								{
									if(WSAGetLastError() != WSAEWOULDBLOCK)
									{
										err_display("recv()");
										ResetSocketInfo(g_nTotalClient);
										continue;
									}
								}else if(retval == 0)
								{
									ResetSocketInfo(g_nTotalClient);
									continue;
								}else
								{
								
									//pInfo->recvBytes = retval;
									// ���� ������ ���
									SOCKADDR_IN sockAddr;
									int nAddrLength = sizeof(sockAddr);
									// Ŭ���̾�Ʈ ���� ���
									getpeername(g_map_client_itor->second.Get_CClientSock(), (SOCKADDR*)&sockAddr, &nAddrLength);
									//���� ��Ŷ �м�

									g_Queue_recv_buf.push(export_Data.EXPORT_DATA(g_Queue_recv_buf.front().szPacket,retval));

									temp_queue_recv_buf.front().szPacket[retval] = '\0';
									printf("[TCP/%s:%d] %s\n",
									inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port), temp_queue_recv_buf.front().szPacket);
									
								
								}

							}
						}
					}


		}
	}//while end
	return 0;
}






void MsgReq(char* szBuf, int index,int NumOfMessage )
{
	char szID[MAX_ID+1] = {0x0,};
	char szMsg[MAX_MSG+1] = {0x0,};

	memcpy(szID,szBuf+index,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+index,MAX_MSG);
	index+=MAX_MSG;
	
	printf("�޽��� ��:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);

}


//�����͸� äũ�ϴ� swicth���� ����� �ű��
DWORD WINAPI Check_Data()
{
	int index = 0;
	int num_of_Queue = 1;
	int nPacketType = 0;
	SOCKET_DATA real_Data;

	int data_Lenght = 0;

	while(1)
	{
		if(!g_Queue_recv_buf.empty())
		{

			real_Data = g_Queue_recv_buf.front();

			//------------------------------------h1,h2,lenght
			data_Lenght = real_Data.szPacket[sizeof(int) + sizeof(int) + sizeof(int)];
			real_Data.szPacket[data_Lenght+1];

			index = 0;
			
			nPacketType = real_Data.szPacket[index++];

			switch(nPacketType)
			{
				case CS_CHAT_MSG_NTY:
					MsgReq(real_Data.szPacket, index,num_of_Queue);
					
					/*
					if(!g_Queue_recv_buf.empty())
					{
					g_Queue_recv_buf.pop();
					num_of_Queue++;	
					}
					*/
					
				break;

			}

		}else
		{
			//break;
		}
	}//while end

	 index = 0;	
	 printf("���� �� �޽�����:%d\n",num_of_Queue);
	 num_of_Queue = 1;
	
	return 0;
}


int AddSocketInfo(SOCKET clientsock,int numofClient)
{

	if( clientsock == NULL)
		return -1;

	// �ش� ���� recv,send Ȯ���ϱ����س־��ֱ�.
	FD_SET( clientsock, &g_recv_set);
	FD_SET( clientsock, &g_writ_set);	

	g_ClientSock[numofClient].Set_CClientSock(clientsock);
	g_ClientSock[numofClient].SetSockNo(numofClient);
	
	//g_map_client_itor = g_map_CClientSock.find(numofClient);

	g_map_CClientSock.insert( map<int,CClientSock>::value_type( numofClient , g_ClientSock[numofClient]) );
	
	g_nTotalClient++;
	g_nTotalSockets++;

	return g_nTotalSockets-1;
}

/*
void RemoveSocketInfo(int nIndex)
{

	//SOCKET_DATA* pInfo = g_socket_Info[nIndex];

	// Ŭ���̾�Ʈ ���� ���
	SOCKADDR_IN socketAddr;
	int addrlen = sizeof(socketAddr);

	//getpeername(pInfo->sock, (SOCKADDR*)&socketAddr, &addrlen);
	g_map_client_itor =	g_map_CClientSock.find(nIndex);
	g_map_client_itor.second()

	//getpeername(g_list_CClientSock.back().Get_CClientSock(), (SOCKADDR*)&socketAddr, &addrlen);
	getpeername(g_map_client_itor.second(), (SOCKADDR*)&socketAddr, &addrlen);
	
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n",
			inet_ntoa(socketAddr.sin_addr), ntohs(socketAddr.sin_port));

	closesocket(g_list_CClientSock.back().Get_CClientSock());
	
	//�ش� Index����
	g_map_CClientSock.erase(g_map_CClientSock.find(nIndex));

	




		if(nIndex == g_list_CClientSock.back().GetScokNo())
		{
			
			getpeername(g_list_CClientSock.back().Get_CClientSock(), (SOCKADDR*)&socketAddr, &addrlen);

			printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n",
			inet_ntoa(socketAddr.sin_addr), ntohs(socketAddr.sin_port));

			closesocket(g_list_CClientSock.back().Get_CClientSock());

		}

	
	g_nTotalSockets--;
}
*/





void SetAllSocketInfo(int max_NumOfClient)
{
	
	//g_ClientSock[i]...�̰� �ʱ�ȭ??!

	for(int i=0;i<max_NumOfClient;i++)
	{
		
		g_map_CClientSock.insert(map<int,CClientSock>::value_type( i,g_ClientSock[i]) );

	}

}
void InsertClientAtSocket(int numofClient, InItServer InItServ, SOCKADDR_IN clientAddr, int addrlen)
{
	//������ �������� ���Ͽ� �������� �־��ֱ�


	SOCKET new_sock = accept(InItServ._get_Listen_Socket(),(SOCKADDR*)&clientAddr,&addrlen);

	

	if( new_sock == INVALID_SOCKET)
	{
		if((WSAGetLastError() != WSAEWOULDBLOCK))
		{
			InItServ.err_quit("accept()");
		}
	}else
	{
		// ������ �Ǹ�
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
			inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		//Sleep(10);

		
		//���⼭ AddSocketInfo ��� InsertClientAtSock�� ���ְ�
		//���� ���� �߰�
		int n_sock_no = AddSocketInfo( new_sock,g_nTotalSockets);

		if( n_sock_no == -1 )
		{
			printf("[TCP ����] Ŭ���̾�Ʈ ���������� �մϴ�n");
			closesocket(new_sock );	
		}

	}

}

void ResetSocketInfo(int nIndex)
{
	//������ �������� ���� �ʱ�ȭ

	// Ŭ���̾�Ʈ ���� ���
	SOCKADDR_IN socketAddr;
	int addrlen = sizeof(socketAddr);

	g_map_client_itor =	g_map_CClientSock.find(nIndex);

	//g_map_client_itor.first = 0;//?!?!?!?!?!?!?
	//g_map_client_itor.second() =null //null������ �ʱ�ȭ

	getpeername(g_map_client_itor->second.Get_CClientSock(), (SOCKADDR*)&socketAddr, &addrlen);
	
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n",
		inet_ntoa(socketAddr.sin_addr), ntohs(socketAddr.sin_port));

	closesocket(g_map_client_itor->second.Get_CClientSock());

	//�ش�g_map_�ʱ�ȭ~~~
	//g_map_CClientSock[nIndex] = NULL;

	g_map_CClientSock.insert(map<int,CClientSock>::value_type(nIndex,g_ClientSock[nIndex]));

	g_nTotalClient --;
	g_nTotalSockets --;
}