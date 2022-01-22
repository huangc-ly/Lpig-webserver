/*
 * network_IO_read.cpp
 *
 *  Created on: 2012-5-2
 *      Author: kevin
 */

#include "network_IO_read.h"
#include <errno.h>
#include "log.h"
#include <string>
#include <unistd.h>
#include "threadpool.h"
#include "disk_requests.h"
#include <cstring>
#include "job.h"

extern utility::log *gp_log;

network_IO_read_item::network_IO_read_item(job_list_mgr *p_list_mgr)
{
	assert(p_list_mgr != NULL);
	mp_list_mgr = p_list_mgr;
}

network_IO_read_item::~network_IO_read_item()
{

}

void network_IO_read_item::Run()
{
	assert(mn_socket != -1);

	char buf[4096];
	char *p_buf = buf;
	std::string s_data;
	int n_errno = errno = 0;
	int n_read = 0;
	while(true)
	{
		errno = 0;
		n_read = recv(mn_socket, p_buf, 4096, 0);
		n_errno = errno;
		if(n_read > 0)
		{
			s_data.assign(p_buf, n_read);
		}
		else
		{
			if(n_read == 0)
			{
				gp_log->write_log(utility::ll_important, "socket: %d, n_read: 0, peer close socket, shutdown local socket...", mn_socket);
				close(mn_socket);
				mn_socket = -1;
				return ;
			}
			else if(n_errno == EAGAIN)
			{
				gp_log->write_log(utility::ll_important, "socket: %d, n_read: -1, errno: EAGAIN.", mn_socket);
				break;
			}
			else if(n_errno == EINTR)
			{
				continue;
			}
			else
			{
				gp_log->write_log(utility::ll_important, "socket: %d, read failed: %s, shutdown local socket...", mn_socket, strerror(errno));
				close(mn_socket);
				mn_socket = -1;
				return ;
			}
		}
	}

	disk_requests_item *p_requests_item = new disk_requests_item(mp_list_mgr, s_data);

	assert(p_requests_item != NULL);
	p_requests_item->SetSocket(mn_socket);
	mp_list_mgr->push(p_requests_item, tp_disk_reader);
}


