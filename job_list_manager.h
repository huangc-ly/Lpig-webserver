/*
 * job_list_manager.h
 *
 *  Created on: 2012-5-2
 *      Author: kevin
 */

#ifndef JOB_LIST_MANAGER_H_
#define JOB_LIST_MANAGER_H_

#include <cassert>
#include <cstdlib>
#include "job.h"
#include "log.h"
#include <semaphore.h>
#include <list>
#include "ring_buffer.h"

enum threadpool
{
	tp_network_reader,
	tp_network_writer,
	tp_disk_reader,
};

class job_list_mgr
{
public:
	job_list_mgr(unsigned int list_size = 10000);
	~job_list_mgr();
	bool push(CJob *job, threadpool e_which);
	bool pop(CJob **job, threadpool e_which);
private:
	bool _push_network_IO_read_list(CJob *job);
	bool _pop_network_IO_read_list(CJob **job);
	bool _push_network_IO_write_list(CJob *job);
	bool _pop_network_IO_write_list(CJob **job);
	bool _push_disk_requests_list(CJob *job);
	bool _pop_disk_requests_list(CJob **job);

/*	std::list<CJob*> network_IO_read_list;
	std::list<CJob*> network_IO_write_list;
	std::list<CJob*> disk_requests_list;
*/
	ring_buf<CJob*> network_IO_read_list;
	ring_buf<CJob*> network_IO_write_list;
	ring_buf<CJob*> disk_requests_list;

	sem_t m_network_read_list_item;
	sem_t m_network_read_list_slot;
	sem_t m_network_read_list_mutx;

	sem_t m_network_write_list_item;
	sem_t m_network_write_list_slot;
	sem_t m_network_write_list_mutx;

	sem_t m_disk_requests_list_item;
	sem_t m_disk_requests_list_slot;
	sem_t m_disk_requests_list_mutx;

};

#endif /* JOB_LIST_MANAGER_H_ */
