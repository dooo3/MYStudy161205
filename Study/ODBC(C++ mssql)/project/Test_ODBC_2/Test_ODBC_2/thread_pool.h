#pragma once
#include "define.h"



class thread_pool
{
public:
	thread_pool(void);
	~thread_pool(void);

	void InitMutex(void);
	void DeInitMutex(void)	;
	void AcquireMutex(void);
	void ReleaseMutex(void);
	DWORD AddWorkToPool(ConnectInfo work);
	ConnectInfo GetWorkFromPool();
	DWORD MakeThreadToPool();
	
	void Test_Connection(ConnectInfo* connectInfo);
	void CloseHandle();
};
