#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <semaphore.h>
#include "job_list_manager.h"

class CJob;
class CSlaveThread;

class CThreadPool
{
public:
	CThreadPool(job_list_mgr *p_list_mgr, int = 5);
	virtual ~CThreadPool();
	void init();
	virtual bool AddJob(CJob *);
	virtual bool GetJob(CJob **);
protected:
	job_list_mgr *mp_list_mgr;
	std::queue<CSlaveThread*> m_threadlist;

	int m_threadnum;
private:
	void Killall();
	CThreadPool(const CThreadPool& obj){}
	void operator=(const CThreadPool& i){}
};

class network_reader_pool : public CThreadPool
{
public:
	network_reader_pool(job_list_mgr *p_list_mgr, int = 5);
	virtual ~network_reader_pool();
	bool AddJob(CJob *);
	bool GetJob(CJob **);

};

class network_writer_pool : public CThreadPool
{
public:
	network_writer_pool(job_list_mgr *p_list_mgr, int = 5);
	virtual ~network_writer_pool();
	bool AddJob(CJob *);
	bool GetJob(CJob **);
};

class disk_reader_pool : public CThreadPool
{
public:
	disk_reader_pool(job_list_mgr *p_list_mgr, int = 5);
	virtual ~disk_reader_pool();
	bool AddJob(CJob *);
	bool GetJob(CJob **);
};
#endif
