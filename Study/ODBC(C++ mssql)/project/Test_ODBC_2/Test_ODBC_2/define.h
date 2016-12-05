#pragma once

#include <stdio.h>
#include<string.h>    //strlen
#include <tchar.h>
#include <winsock2.h> //소켓 추가 하기 위해~~
#include <Ws2tcpip.h>
#include <process.h> //beginthreadex() 함수 사용시 필요 헤더파일
#include "windows.h"
#include "iostream"
#include "sql.h"
#include "sqlext.h"





//클래스 header


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
	//threadID가 필요한가?00
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

	//마지막으로 작업시작한 쓰레드의 다음 쓰레드 인덱스
	DWORD threadIdx;
}ThreadPool;




/*
DWORD AddWorkToPool(ConnectInfo work);	//작업 등록
ConnectInfo GetWorkFromPool(void);				//작업 가져오기
DWORD MakeThreadToPool();				//쓰레드풀 생성
void WorkerThreadFunction(LPVOID pParam);//쓰레드 함수
void Test_Connection(ConnectInfo* connectInfo); // 쓰레드에서 실행할 함수(실제 연결작업)
void CloseHandle();					//리소스 해제
*/

//header에 변수 선언할 때는 static 으로 하거나 해서 중복으로 뭐시기 되는 것을 막아햐나다.
static HANDLE mutex = NULL;	//뮤텍스 선언
static ThreadPool g_ThreadPool;	//쓰레드풀 선언


void WorkerThreadFunction(LPVOID pParam);

