#include "threadpool.h"
#include "slavethread.h"
#include "job.h"


CThreadPool::CThreadPool(job_list_mgr *p_list_mgr, int threadnum)
	:m_threadnum(threadnum)
{
	mp_list_mgr = p_list_mgr;
}

void CThreadPool::init()
{
	for(int i = 0; i < m_threadnum; i++)
	{
		CSlaveThread* t = new CSlaveThread;
		m_threadlist.push(t);
		t->SetThreadPool(this);
		t->Start();
	}
}
CThreadPool::~CThreadPool()
{
	Killall();
}

void CThreadPool::Killall()
{
	for(int i = 0; i < m_threadnum; i++)
	{
		delete m_threadlist.front();
		m_threadlist.pop();
	}
}

bool CThreadPool::AddJob(CJob *job)
{
	assert(false);
}

bool CThreadPool::GetJob(CJob** job)
{
	assert(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

network_reader_pool::network_reader_pool(job_list_mgr *p_list_mgr, int n_thread_num)
	:CThreadPool(p_list_mgr, n_thread_num)
{

}

network_reader_pool::~network_reader_pool()
{

}

bool network_reader_pool::AddJob(CJob *job)
{
	assert(job != NULL);
	if(mp_list_mgr->push(job, tp_network_reader))
		return true;
	return false;
}

bool network_reader_pool::GetJob(CJob** job)
{
	assert(job != NULL);
	if(mp_list_mgr->pop(job, tp_network_reader))
		return true;
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

network_writer_pool::network_writer_pool(job_list_mgr *p_list_mgr, int n_thread_num)
	:CThreadPool(p_list_mgr, n_thread_num)
{

}

network_writer_pool::~network_writer_pool()
{

}

bool network_writer_pool::AddJob(CJob *job)
{
	assert(job != NULL);
	if(mp_list_mgr->push(job, tp_network_writer))
		return true;
	return false;
}

bool network_writer_pool::GetJob(CJob** job)
{
	assert(job != NULL);
	if(mp_list_mgr->pop(job, tp_network_writer))
		return true;
	return false;
}

disk_reader_pool::disk_reader_pool(job_list_mgr *p_list_mgr, int n_thread_num)
	:CThreadPool(p_list_mgr, n_thread_num)
{

}

disk_reader_pool::~disk_reader_pool()
{

}

bool disk_reader_pool::AddJob(CJob *job)
{
	assert(job != NULL);
	if(mp_list_mgr->push(job, tp_disk_reader))
		return true;
	return false;
}

bool disk_reader_pool::GetJob(CJob** job)
{
	assert(job != NULL);
	if(mp_list_mgr->pop(job, tp_disk_reader))
		return true;
	return false;
}
