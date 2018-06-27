#include <iostream>
#include <Windows.h>
#include <process.h>
#include <tchar.h>
#include <strsafe.h>

#define BUF_SIZE 255

typedef struct MyData {
	int nVal1;
	int nVal2;
} MYDATA, *PMYDATA;

using namespace std;

DWORD WINAPI MyThread( LPVOID lpParam );


int main(void)
{
	HANDLE hProcess = NULL;
	DWORD dwProcessId = 0;
	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;
	PMYDATA pData;

	hProcess = GetCurrentProcess(); //进程伪句柄
	cout << "The Current Process Pseudo Handle is " << hProcess << endl;
	DuplicateHandle(GetCurrentProcess(),
		GetCurrentProcess(),
		GetCurrentProcess(),
		&hProcess,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS
		);
	cout << "The Current Process Real Handle is " << hProcess << endl;
	dwProcessId = GetCurrentProcessId();
	cout << "The Current Process Id is " << dwProcessId << endl;

	// Allocate memory for thread data.
	pData = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MYDATA));

	if( pData == NULL )
	{
		ExitProcess(2);
	}
	// Generate unique data for each thread.
	pData->nVal1 = 1;
	pData->nVal2 = 100;
	// Create Thread
	hThread = CreateThread( NULL,               // default security attributes
		0,                  // use default stack size  
		MyThread,           // thread function 
		pData,              // argument to thread function 
		0,                  // use default creation flags 
		&dwThreadId);       // returns the thread identifier 

	cin.get();
	return 0;
}

DWORD WINAPI MyThread( LPVOID lpParam )
{
	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;
	hThread = GetCurrentThread();   //线程伪句柄
	cout << "The Current Thread Pseudo Handle is " << hThread << endl;
	DuplicateHandle(GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&hThread,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS
		);
	cout << "The Current Thread Real Handle is " << hThread << endl;
	dwThreadId = GetCurrentThreadId();
	cout << "The Current Thread Id is " << dwThreadId << endl;

	HANDLE hStdout;
	PMYDATA pData;

	TCHAR msgBuf[BUF_SIZE] = {0};
	size_t cchStringSize;
	DWORD dwChars;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if( hStdout == INVALID_HANDLE_VALUE )
		return 1;

	// Cast the parameter to the correct data type.
	pData = (PMYDATA)lpParam;

	// Print the parameter values using thread-safe functions.
	StringCchPrintf(msgBuf, 
		BUF_SIZE, 
		TEXT("Parameters = %d, %d\n"), 
		pData->nVal1, 
		pData->nVal2); 
	StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
	WriteConsole(hStdout, msgBuf, cchStringSize, &dwChars, NULL);

	return 0; 

}