/*
 * job_list_manager.cpp
 *
 *  Created on: 2012-5-2
 *      Author: kevin
 */

#include "job_list_manager.h"
#include "network_IO_write.h"
#include "disk_requests.h"

extern utility::log *gp_log;

job_list_mgr::job_list_mgr(unsigned int list_size)
	:network_IO_read_list(10000, NULL), network_IO_write_list(10000, NULL), disk_requests_list(10000, NULL)
{
	sem_init(&m_network_read_list_item, 0, 0);
	sem_init(&m_network_read_list_slot, 0, list_size);
	sem_init(&m_network_read_list_mutx, 0, 1);

	sem_init(&m_network_write_list_item, 0, 0);
	sem_init(&m_network_write_list_slot, 0, list_size);
	sem_init(&m_network_write_list_mutx, 0, 1);

	sem_init(&m_disk_requests_list_item, 0, 0);
	sem_init(&m_disk_requests_list_slot, 0, list_size);
	sem_init(&m_disk_requests_list_mutx, 0, 1);

}

job_list_mgr::~job_list_mgr()
{
}

bool job_list_mgr::push(CJob *job, threadpool e_which)
{
	assert(job != NULL);

	switch(e_which)
	{
	case tp_network_reader:
		if(_push_network_IO_read_list(job))
			return true;
		break;
	case tp_network_writer:
		if(_push_network_IO_write_list(job))
			return true;
		break;
	case tp_disk_reader:
		if(_push_disk_requests_list(job))
			return true;
		break;
	default:
		gp_log->write_log(utility::ll_error, "Unknown kind of theadpool: e_which = %d", e_which);
		return false;
		break;
	}
}

bool job_list_mgr::pop(CJob **job, threadpool e_which)
{
	assert(job != NULL);

	switch(e_which)
	{
	case tp_network_reader:
		if(_pop_network_IO_read_list(job))
			return true;
		break;
	case tp_network_writer:
		if(_pop_network_IO_write_list(job))
			return true;
		break;
	case tp_disk_reader:
		if(_pop_disk_requests_list(job))
			return true;
		break;
	default:
		gp_log->write_log(utility::ll_error, "Unknown kind of theadpool: e_which = %d", e_which);
		return false;
		break;
	}
}

bool job_list_mgr::_push_network_IO_read_list(CJob *job)
{
	assert(job != NULL);

	sem_wait(&m_network_read_list_slot);
	sem_wait(&m_network_read_list_mutx);
	network_IO_read_list.push(job);
	sem_post(&m_network_read_list_mutx);
	sem_post(&m_network_read_list_item);
	return true;
}

bool job_list_mgr::_pop_network_IO_read_list(CJob **job)
{
	assert(job != NULL);

	sem_wait(&m_network_read_list_item);
	sem_wait(&m_network_read_list_mutx);
	network_IO_read_list.pop(*job);
	sem_post(&m_network_read_list_mutx);
	sem_post(&m_network_read_list_slot);
	return true;
}

bool job_list_mgr::_push_network_IO_write_list(CJob *job)
{
	assert(job != NULL);

	sem_wait(&m_network_write_list_slot);
	sem_wait(&m_network_write_list_mutx);
	network_IO_write_list.push(job);
	sem_post(&m_network_write_list_mutx);
	sem_post(&m_network_write_list_item);
	return true;
}

bool job_list_mgr::_pop_network_IO_write_list(CJob **job)
{
	assert(job != NULL);

	sem_wait(&m_network_write_list_item);
	sem_wait(&m_network_write_list_mutx);
	network_IO_write_list.pop(*job);
	sem_post(&m_network_write_list_mutx);
	sem_post(&m_network_write_list_slot);
	return true;
}

bool job_list_mgr::_push_disk_requests_list(CJob *job)
{
	assert(job != NULL);

	sem_wait(&m_disk_requests_list_slot);
	sem_wait(&m_disk_requests_list_mutx);
	disk_requests_list.push(job);
	sem_post(&m_disk_requests_list_mutx);
	sem_post(&m_disk_requests_list_item);
	return true;
}

bool job_list_mgr::_pop_disk_requests_list(CJob **job)
{
	assert(job != NULL);

	sem_wait(&m_disk_requests_list_item);
	sem_wait(&m_disk_requests_list_mutx);
	disk_requests_list.pop(*job);
	sem_post(&m_disk_requests_list_mutx);
	sem_post(&m_disk_requests_list_slot);
	return true;
}

