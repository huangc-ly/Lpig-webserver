#include "slavethread.h"
#include "job.h"
#include "threadpool.h"


CSlaveThread::CSlaveThread()
	:m_threadpool(NULL), m_job(NULL)
{
	
}

CSlaveThread::~CSlaveThread()
{
	
}

void CSlaveThread::SetThreadPool(CThreadPool* pool)
{
	m_threadpool = pool;
}

void CSlaveThread::Run()
{
	while(1)
	{
		m_threadpool->GetJob(&m_job);
		m_job->SetThread(this);
		m_job->Run();
		delete m_job;				
	}
}












