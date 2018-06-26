#include <iostream>
#include <string>
#include <sstream>

#include <OpenThreads/Block>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/Mutex>
#include <OpenThreads/Thread>


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

// block线程测试
class TestBlockThread :public OpenThreads::Thread
{
public:
	TestBlockThread():_num(0) {}

	  virtual void run() 
	  {

		  while(!testCancel())
		  {
			  _block.block();
			   _block.reset();
			  std::ostringstream info ;
			  info << "blockThread: waited the block release,block number:" <<  _num; 
			  g_print.print(info.str());
			 
		  }
	  }
public:
	OpenThreads::Block _block;
	int _num;

};

void main()
{

	std::cout<< "start test Block" << std::endl;
	TestBlockThread blockThread;

	blockThread.start();

	OpenThreads::Thread::microSleep(1000);

	while(1)
	{
			blockThread._num +=1;
		
			std::ostringstream info ;
			info <<"main Thread release block,set block number:"<< blockThread._num;
			g_print.print(info.str());

			blockThread._block.release();
			OpenThreads::Thread::microSleep(0);
			
	}
}

// wangrui的osgblock例子

//class TestThread :public OpenThreads::Thread
//{
//
//public:
//	TestThread() :_done(false), _count(0)
//	{}
//	~TestThread()
//	{
//		cancel();
//	}
//
//	void block() { _operator.block();}
//
//	virtual int cancel() 
//	{
//
//		_operator.release();
//		_done = true;
//		while(isRunning())
//			OpenThreads::Thread::YieldCurrentThread();
//		return 0;
//
//	}
//	virtual void run()
//	{
//		do 
//		{
//			std::cout<< "(" << _count << ")" ;
//			++ _count;
//
//			if(_count == 10)
//			{
//				_operator.release();
//				_operator.reset();
//				_operator.block();
//
//			}
//			OpenThreads::Thread::microSleep(150000L);
//		} while (!_done);
//	}
//
//protected:
//	bool _done;
//	unsigned long _count;
//	OpenThreads::Block _operator;
//};
//
//
//int main()
//{
//
//	TestThread t;
//	t.start();
//	t.block();
//	std::cout<< "(Main)" << std::endl;
//
//	getchar();
//	t.cancel();
//	return 0;
//}
