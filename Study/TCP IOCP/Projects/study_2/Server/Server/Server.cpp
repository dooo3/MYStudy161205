#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>

using namespace std;

#define BUFSIZE 210400
#define BUFSIZE_LOGIN 256

const static int MAX_ID		=	20;
const static int MAX_MSG	=	256;
const static int MAX_PASSWORD	=	128;


const static int PACKET_HEADER_START1	=	((BYTE)0xFF);
const static int PACKET_HEADER_START2	=	((BYTE)0xA0);

//int PACKET_LENGHT			=	sizeof(int)+sizeof(int)+sizeof(int)+MAX_ID+MAX_MSG;
//////////////////////////////////////////////////s1,s2,lenght

//const static int PACKET_HEADER_END1		=	((BYTE)0xDD);
//const static int PACKET_HEADER_END2		=	((BYTE)0xCC);

//#define CS_CHAT_MSG_NTY		(BYTE(0x01))	// ��û _REQ , ���� _NTY, ���� _ACK

//���� �������� ù ����Ʈ�� ���� ������ Ÿ��;;(����� ���°� �ƴ�)
const static int CS_CHAT_MSG_NTY		=	((BYTE)0x01);
/*
	char szID[20];
	char szMsg[512];
*/

void EXPORT_DATA(char buf[], int nTotLen );
//�����Ϳ� ���̸� �޾Ƽ� Queue�� �����ϴ� �Լ�
/////////////////////20161010

HANDLE g_Data_Thread;
//queue�� �� �����͸� �м��ϰ� ����ϴ� �Լ� 

//--------------test�� recv_Buf,recv_retval
//char recv_Buf[BUFSIZE+1];
//int recv_retval = 0;


//queue�� ���� ����ü
typedef struct packet_data
{
	char szPacket[BUFSIZE+1];
	SOCKET sock;
	packet_data()
	{
		memset( szPacket, 0x0, BUFSIZE );
	}
}PACKET_DATA;

//����ü�� queue����
queue<PACKET_DATA> Queue_buf;

PACKET_DATA packet_Data;

DWORD WINAPI Check_Data(LPVOID arg);
/////////////////////20161010

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





///
// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI Receive_Process(LPVOID arg)
{
	//SOCKET client_sock = (SOCKET)arg;
	char buf[BUFSIZE+1];
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;

	//�̰ɷ� ���� �ٲٱ�
	packet_Data.sock = (SOCKET)arg;

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(packet_Data.sock, (SOCKADDR *)&clientaddr, &addrlen);

	//��Ŷ�� �ϴ� ��� �ް� ť���ְ� (���ϵ� ���� �־�� �Ѵ�? �����غ���)
	//�� ������ �����ؼ� ������ ������ ����ġ ���� �־ �޴´�
	//���� ����� �����ʴ´ٸ� ���� ���������

	
	while(1){
		// ������ �ޱ�
		// 1. Ŀ�� �۽Ź��ۿ� �����Ͱ� ������ �о�´�.
		retval = recv(packet_Data.sock, buf, BUFSIZE, 0); 
		//��Ŷ�� �о�ͼ� ������ �κи� Queue�� �ִ´�
		EXPORT_DATA(buf, retval);
		
	}

	// closesocket()
	closesocket(packet_Data.sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}



	
	


int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");	
	
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");
	
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	//SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	HANDLE recv_Proc_Thread;

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		packet_Data.sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(packet_Data.sock == INVALID_SOCKET){
			err_display("accept()");
			continue;
		}
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// ������ ����
		recv_Proc_Thread = CreateThread(NULL, 0, Receive_Process, 
			(LPVOID)packet_Data.sock, 0, NULL);
			//&ThreadId 
		if(recv_Proc_Thread == NULL)
			printf("[����] ������ ���� ����!\n");
		else
			CloseHandle(recv_Proc_Thread);


		g_Data_Thread = CreateThread(NULL, 0, Check_Data, 
			(LPVOID)packet_Data.sock, 0, NULL);
			
			if(g_Data_Thread == NULL)
				printf("[����] ������ ���� ����!\n");
			else
				CloseHandle(g_Data_Thread);

		/*
		if(!Queue_buf.empty())
		{
			g_Data_Thread = CreateThread(NULL, 0, Check_Data, 
			NULL, 0, NULL);
			
			if(g_Data_Thread == NULL)
				printf("[����] ������ ���� ����!\n");
			else
				CloseHandle(g_Data_Thread);
		}
		*/
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}


//int g_nCnt = 0;

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

		// 1. h1 �ľ� h2 �ľ�
		memcpy(&nHeader1, buf+index,sizeof(int));
		index+=sizeof(int);

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
		stDatagram.sock = packet_Data.sock;

		Queue_buf.push(stDatagram);

		//g_nCnt++;
		//printf("g_nCnt:%d\n", g_nCnt);

	}

	

}

void MsgReq(char* szBuf, int index,int NumOfMessage )
{
	char szID[MAX_ID+1] = {0x0,};
	char szMsg[MAX_MSG+1] = {0x0,};

	//HeaderLenght��ŭ index�̵�(x)
	//????????? Start1,Start2��ŭ �� �̵��ؾ� �ϴ°� �ƴѰ�?
	//�ƴϴ� queue���� ��¥ �����͸� �־���
	//nType��ŭ�� �̵��ؾ��ϴµ�?...

	
	memcpy(szID,szBuf+index,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+index,MAX_MSG);
	index+=MAX_MSG;
	

	//�ϴ� �Ǵ°� 
	/*
	index+=sizeof(int);

	memcpy(szID,szBuf+1,MAX_ID);
	index+=MAX_ID;

	memcpy(szMsg,szBuf+MAX_ID+1,MAX_MSG);
	index+=MAX_MSG;
	*/
	//printf("�޽��� ��:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);
	
	printf("�޽��� ��:%d, ID:%s,MEG:%s \n",NumOfMessage,szID,szMsg);
}


//�����͸� äũ�ϴ� swicth���� ����� �ű��
DWORD WINAPI Check_Data(LPVOID arg)
{
	int index = 0;
	int num_of_Queue = 1;
	int nPacketType = 0;
	PACKET_DATA real_Data;

	int data_Lenght = 0;

	while(1)
	{
		if(!Queue_buf.empty())
		{

			real_Data = Queue_buf.front();

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
					
					Queue_buf.pop();
					num_of_Queue++;
				break;

			}

		}else
		{
			//break;
		}

	}

		


		/*
		//send_Buf�� ���� ����
		switch( nPacketType )
		{

			case CS_CHAT_MSG_NTY:
				
				for(;index<recv_retval;)
				{
					//index�� ���̰� retval���� Ŀ��������?
						//�����Ͱ� 1��������
						////////////////////////////////////
							char szID[MAX_ID+1] = {0x0,};
							char szMsg[MAX_MSG+1] = {0x0,};

							//�´� ������� check
							//ù��° �ε����� ��Ŷ�Ǽ�*289+1
							//�񱳴� ���߿�...
							//printf("Num_Of_Packet*289+1+1 : %d\n",Num_Of_Packet*289+1+1);
							
							//n=memcmp(recv_Buf+(Num_Of_Packet*289+1+sizeof(int)),&PACKET_HEADER_START1,sizeof(int));

							//if(recv_Buf[Num_Of_Packet*289+1+sizeof(int)] == PACKET_HEADER_START1 && recv_Buf[Num_Of_Packet*289+1+sizeof(int)+sizeof(int)] == PACKET_HEADER_START2)
							{
								
								//printf("ù��° �ε���%d\n",index);
								//memcpy(&PACKET_HEADER_START1,buf+index,sizeof(int));
								index+= sizeof(int);

								//memcpy(&PACKET_HEADER_START2,buf+index,sizeof(int));
								index+= sizeof(int);

								memcpy(&PACKET_LENGHT,recv_Buf+index,sizeof(int));
								index += sizeof(int);
								
								memcpy(szID,recv_Buf+index,MAX_ID);
								index+=MAX_ID;

								memcpy(szMsg,recv_Buf+index,MAX_MSG);
								index+=MAX_MSG;

								printf("index:%d,���̵�:%s,  �޽���:%s,header1:%d ,header2:%d ,lenght:%d\n ",index,szID,szMsg,PACKET_HEADER_START1,PACKET_HEADER_START2,PACKET_LENGHT);
								//512����Ʈ�� ä�ø޽����� �������.
								
								index+=1;
								//printf("��Ŷ�� ��: %d\n",Num_Of_Packet);
								Num_Of_Packet++;
								
								//index+=2;
								//�� ������ �ε��� +2 ?nPacketType���� �����ؼ�
							}
							//else
							//{
								//���� �ʴ���Ŷ
								//���� ���������
								// closesocket()
								//closesocket(client_sock);
								//printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
								//	inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

							//}
								
					
				}
				printf("ä�� ��Ŷ recv!!\n");
			
				break;

			
		}
		*/
		
		//recv_Buf[recv_retval] = '\0';
		//recv_retval=0;
		//Num_Of_Packet = 0;

	 index = 0;	
	 
	 printf("���� �� �޽�����:%d\n",num_of_Queue);
	 
	 num_of_Queue = 1;
	
	return 0;
}



