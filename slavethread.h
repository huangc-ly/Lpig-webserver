#ifndef SLAVETHREAD_H
#define SLAVETHREAD_H
#include "thread.h"

class CThreadPool;
class CJob;

class CSlaveThread : public CThread
{
	public:
		CSlaveThread();
		virtual ~CSlaveThread();
		void SetThreadPool(CThreadPool* pool);
		void Run();

	private:
		CThreadPool* m_threadpool;
		CJob* m_job;
};

#endif
