#include "ExportData.h"


ExportData::ExportData(void)
{
}

ExportData::~ExportData(void)
{
}

SOCKET_DATA ExportData::EXPORT_DATA(char buf[], int nTotLen )
{
	int index = 0;
	
	SOCKET_DATA stDatagram;

	int nDatagramLen = 0;

	int nHeader1 = 0;
	int nHeader2 = 0;
	
	while(index < nTotLen)
	{
		nDatagramLen = 0;
		nHeader1 = 0;
		nHeader2 = 0;
		memset( &stDatagram, 0x0, sizeof( SOCKET_DATA ) );

		// 1. h1 �ľ� 
		memcpy(&nHeader1, buf+index,sizeof(int));
		index+=sizeof(int);
		// 2. h2 �ľ�
		memcpy(&nHeader2, buf+index,sizeof(int));
		index+=sizeof(int);

		if( nHeader1 != PACKET_HEADER_START1 || nHeader2 != PACKET_HEADER_START2 )
		{
			// ���� ����.. ���߿� �߰�.
			return stDatagram;
		}

		// 2. ���� ��������
		memcpy(&nDatagramLen, buf+index,sizeof(int));
		index += sizeof(int);
	
		//3. ���̸�ŭ ������ �ܾ����
		memcpy( stDatagram.szPacket, buf+index, nDatagramLen );
		index += nDatagramLen;
		
		//���ϵ� ���� �־��ְ�
		
		//g_Queue_recv_buf.push(stDatagram);
	}

	return stDatagram;
}