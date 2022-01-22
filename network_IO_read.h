/*
 * network_IO_read.h
 *
 *  Created on: 2012-5-2
 *      Author: kevin
 */

#include "job.h"
#include "log.h"
#include "epoller.h"
#include "threadpool.h"
#include "slavethread.h"
#include <sys/socket.h>
#include "job_list_manager.h"

#ifndef NETWORK_IO_READ_H_
#define NETWORK_IO_READ_H_


class network_IO_read_item : public CJob
{
public:
	network_IO_read_item(job_list_mgr *p_list_mgr);
	virtual ~network_IO_read_item();
	virtual void Run();
private:
	job_list_mgr *mp_list_mgr;
};

#endif /* NETWORK_IO_READ_H_ */
