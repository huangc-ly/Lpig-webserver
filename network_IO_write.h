/*
 * network_IO_write.h
 *
 *  Created on: 2012-5-3
 *      Author: kevin
 */

#ifndef NETWORK_IO_WRITE_H_
#define NETWORK_IO_WRITE_H_

#include "job.h"
#include "job_list_manager.h"

class network_IO_write_item : public CJob
{
public:
	network_IO_write_item(job_list_mgr *p_list_mgr, const std::string &s_data, int n_status);
	virtual ~network_IO_write_item();
	virtual void Run();
private:
	void _make_http_packet();

	job_list_mgr *mp_list_mgr;
	std::string ms_reply_data;
	int mn_reply_status;

};

#endif /* NETWORK_IO_WRITE_H_ */
