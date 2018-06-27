#include <iostream>
#include <string>
#include <sstream>


#include <OpenThreads/Thread>
#include <OpenThreads/Atomic>
#include <OpenThreads/Condition>
#include <OpenThreads/ScopedLock>
#include <iostream>
#include <string>

// from :https://blog.csdn.net/snail_hunan/article/details/69802551

#include <OpenThreads/Thread>
#include <iostream>

#include <windows.h>


// 线程安全输出
class SafePrint
{
public:
	void print(std::string&  info)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

		std::cout<< info << std::endl;
	}
private:

	OpenThreads::Mutex _mutex;

};
SafePrint g_print;

int g_ticketCounts = 10000;

OpenThreads::Mutex g_ticketMutex;

int g_nCPUNum = 1;

int g_cpu = 1;
DWORD_PTR g_dwThreadAffinityMask = 1<<1;

class TicketSellThread : public OpenThreads::Thread
{
public:
	TicketSellThread(char* name) :_name(name){ }
	virtual void run()
	{
		HANDLE hThread = GetCurrentThread();
		int code = GetLastError();
		g_dwThreadAffinityMask = g_dwThreadAffinityMask<<1;
		
	//	DWORD_PTR res = SetThreadAffinityMask(hThread,  g_dwThreadAffinityMask);
		int code1 = GetLastError();
		//ERROR_INVALID_PARAMETER
	 setProcessorAffinity(g_cpu++);

		for (int i = 0; i < 5000; i++)
		{
		//	g_ticketMutex.lock();
			g_ticketCounts--;
			std::ostringstream os;
			os << _name  << ":" << g_ticketCounts << std::endl;
			std::cout<< os.str();
		//	g_print.print(os.str());
		//	g_ticketMutex.unlock();
		}
	}
private:
	 char* _name;
};


int main(int argc, char** argv)
{

	DWORD   dwThreadId = GetCurrentThreadId();
 

	DWORD handle = GetThreadLocale();
	HANDLE hMainThread = GetCurrentThread();
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	g_nCPUNum = info.dwNumberOfProcessors;

	printf("Number of processors: %d.\n", info.dwNumberOfProcessors);

	TicketSellThread ticketSellThread1("A");
	ticketSellThread1.start();

	TicketSellThread ticketSellThread2("B");
	ticketSellThread2.start();

	//等待ticketSellThread1运行完
	while(ticketSellThread1.isRunning())
		OpenThreads::Thread::YieldCurrentThread();

	//等待ticketSellThread1运行完
	while(ticketSellThread2.isRunning())
		OpenThreads::Thread::YieldCurrentThread();

	std::cout <<"final ticket count:" << g_ticketCounts << std::endl;
	return 0;
}