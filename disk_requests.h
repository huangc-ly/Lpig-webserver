/*
 * requests.h
 *
 *  Created on: 2012-5-1
 *      Author: kevin
 */

#ifndef REQUESTS_H_
#define REQUESTS_H_

#include "log.h"
#include "job.h"
#include "lock.h"
#include "job_list_manager.h"
#include <string>

class disk_requests_item : public CJob
{
public:
	disk_requests_item(job_list_mgr *p_list_mgr, std::string &s_packet);
   ~disk_requests_item();
	virtual void Run();
private:
	job_list_mgr *mp_list_mgr;
	std::string ms_packet;
	std::string ms_current_dir;
};


#endif /* REQUESTS_H_ */
