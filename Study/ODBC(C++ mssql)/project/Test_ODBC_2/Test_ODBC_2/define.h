#pragma once

#include <stdio.h>
#include<string.h>    //strlen
#include <tchar.h>
#include <winsock2.h> //���� �߰� �ϱ� ����~~
#include <Ws2tcpip.h>
#include <process.h> //beginthreadex() �Լ� ���� �ʿ� �������
#include "windows.h"
#include "iostream"
#include "sql.h"
#include "sqlext.h"





//Ŭ���� header


//////////////////////////
//////////////////////////
//2016-11-15

#define WORK_MAX 8
#define THREAD_MAX 10
#define DEFAULT_BUFLEN 500


using namespace std;




typedef struct _ConnectInfo
{
	SQLCHAR* DNSID; //DNSID
	SQLCHAR* ID;//ID
	SQLCHAR* PW;//PW

	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;

	SQLRETURN retVal;
	//threadID�� �ʿ��Ѱ�?00
}ConnectInfo;

typedef struct _WorkerThread
{
	HANDLE hThread;
	DWORD idThread;
	BOOL bWork;
}WorkerThread;

typedef struct _ThreadPool
{
	WorkerThread workerThreadList[THREAD_MAX];
	HANDLE workerEventList[THREAD_MAX];

	ConnectInfo workList[WORK_MAX];

	DWORD idxOfCurrentWork;
	DWORD idxOfLastAddedWork;

	//���������� �۾������� �������� ���� ������ �ε���
	DWORD threadIdx;
}ThreadPool;




/*
DWORD AddWorkToPool(ConnectInfo work);	//�۾� ���
ConnectInfo GetWorkFromPool(void);				//�۾� ��������
DWORD MakeThreadToPool();				//������Ǯ ����
void WorkerThreadFunction(LPVOID pParam);//������ �Լ�
void Test_Connection(ConnectInfo* connectInfo); // �����忡�� ������ �Լ�(���� �����۾�)
void CloseHandle();					//���ҽ� ����
*/

//header�� ���� ������ ���� static ���� �ϰų� �ؼ� �ߺ����� ���ñ� �Ǵ� ���� �����ᳪ��.
static HANDLE mutex = NULL;	//���ؽ� ����
static ThreadPool g_ThreadPool;	//������Ǯ ����


void WorkerThreadFunction(LPVOID pParam);

