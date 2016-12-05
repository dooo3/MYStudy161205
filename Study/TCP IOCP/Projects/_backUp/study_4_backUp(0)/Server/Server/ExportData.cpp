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

		// 1. h1 파씽 
		memcpy(&nHeader1, buf+index,sizeof(int));
		index+=sizeof(int);
		// 2. h2 파씽
		memcpy(&nHeader2, buf+index,sizeof(int));
		index+=sizeof(int);

		if( nHeader1 != PACKET_HEADER_START1 || nHeader2 != PACKET_HEADER_START2 )
		{
			// 소켓 끊기.. 나중에 추가.
			return stDatagram;
		}

		// 2. 길이 가져오고
		memcpy(&nDatagramLen, buf+index,sizeof(int));
		index += sizeof(int);
	
		//3. 길이만큼 데이터 긁어오기
		memcpy( stDatagram.szPacket, buf+index, nDatagramLen );
		index += nDatagramLen;
		
		//소켓도 따로 넣어주고
		
		//g_Queue_recv_buf.push(stDatagram);
	}

	return stDatagram;
}