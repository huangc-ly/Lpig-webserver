#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class CThread
{
	public:
		CThread(bool = false);
		virtual ~CThread();
		bool Start(); // 线程开始开关
		void Stop(); // 销毁线程 回收资源
		void Detach(); // 分离线程		
		void* Wait(); // 挂起调用线程 
		pthread_t GetTid();
		static void* ThreadFunc(void*); // 线程入口函数
		virtual void Run() = 0;

	private:
		pthread_t m_tid;
		bool m_created; //是否被创建
		bool m_detach; // 是否分离
};

#endif

