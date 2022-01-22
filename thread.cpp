#include <sys/time.h>
#include <stdlib.h>
#include "thread.h"
#include <iostream>
#include <cstdio>
#include "slavethread.h"
CThread::CThread(bool det)
{
	m_created = false;
	m_detach = det;
}

CThread::~CThread()
{
	Stop(); // 终止线程
}

void* CThread::ThreadFunc(void* threadObj)
{
	CThread* thread = (CThread *)threadObj;
	thread->Run();
	return NULL;
}

bool CThread::Start()
{


	if(pthread_create(&m_tid, NULL, ThreadFunc, this) != 0)
		exit(0);
	
	m_created = true;
	return true;
}

void CThread::Detach()
{
	if(m_created && !m_detach)
	{
		pthread_detach(m_tid);
	}
}

void* CThread::Wait()
{
	void* status = NULL;
	if(m_created && !m_detach)
	{
		pthread_join(m_tid, &status);
	}
	return status;
}

void CThread::Stop()
{
	if(m_created)
	{
		if(m_detach)
			pthread_cancel(m_tid);
		else
		{
			pthread_detach(m_tid);
			m_detach = true;
			pthread_cancel(m_tid);
		}
	}
}

