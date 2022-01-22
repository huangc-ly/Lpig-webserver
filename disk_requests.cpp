/*
 * requests.cpp
 *
 *  Created on: 2012-5-2
 *      Author: kevin
 */

#include "disk_requests.h"
#include "network_IO_write.h"
#include <unistd.h>
#include <errno.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log.h"
#include <cstring>

extern utility::log *gp_log;

disk_requests_item::disk_requests_item(job_list_mgr *p_list_mgr, std::string &s_packet)
	:ms_packet(s_packet)
{
	mp_list_mgr = p_list_mgr;
	static const size_t MAX_FILE_NAME_SIZE = 1024;
	char sz_buf[MAX_FILE_NAME_SIZE];
	::getcwd(sz_buf, MAX_FILE_NAME_SIZE - 1);
	ms_current_dir = sz_buf;
	ms_current_dir += "/doc";
}

disk_requests_item::~disk_requests_item()
{

}

/*
 * 	read packet, get request and start disk IO
 */
void disk_requests_item::Run()
{
	assert(gp_log != NULL);
	int n_errno = errno = 0;
	std::string s_reply_data;

	size_t pos = ms_packet.find("GET");
	pos += 3;	// GET length
	size_t n_start = ms_packet.find_first_not_of(' ', pos);
	size_t n_end = ms_packet.find_first_of(' ', n_start);

	std::string request(ms_packet, n_start, n_end-n_start);
	ms_current_dir += request;

	struct stat file_stat;
	if(-1 == stat(ms_current_dir.c_str(), &file_stat))
	{
		n_errno = errno;
		gp_log->write_log(utility::ll_important, "socket: %d, stat file failed, stat: %s", mn_socket, strerror(n_errno));
		network_IO_write_item *p_write_item = new network_IO_write_item(mp_list_mgr, s_reply_data, rs_not_exists);
		p_write_item->SetSocket(mn_socket);
		mp_list_mgr->push(p_write_item, tp_network_writer);
		return ;
	}
	if(S_ISDIR(file_stat.st_mode))
	{
		gp_log->write_log(utility::ll_important, "socket: %d, request for a directory", mn_socket);
		network_IO_write_item *p_write_item = new network_IO_write_item(mp_list_mgr, s_reply_data, rs_is_directory);
		p_write_item->SetSocket(mn_socket);
		mp_list_mgr->push(p_write_item, tp_network_writer);
		return ;
	}

	int n_fd;
	if((n_fd = open(ms_current_dir.c_str(), O_RDONLY)) == -1)
	{
		n_errno = errno;
		gp_log->write_log(utility::ll_error, "open file: %s failed, open: %s", ms_current_dir.c_str(), strerror(n_errno));
		return;
	}
	char sz_buf[4096];
	int n_read;

	while(true)
	{
		errno = 0;
		n_read = read(n_fd, sz_buf, 4096);
		n_errno = errno;
		if(n_read > 0)
		{
			s_reply_data.append(sz_buf, n_read);
		}
		else if(n_read == 0)
		{
			break;
		}
		else
		{
			gp_log->write_log(utility::ll_important, "read file: %s failed , read: %s, shutdown local socket...", ms_current_dir.c_str(), strerror(n_errno));
			close(mn_socket);
			mn_socket = -1;
		}

	}
	close(n_fd);
	n_fd = -1;

	network_IO_write_item *p_write_item = new network_IO_write_item(mp_list_mgr, s_reply_data, rs_ok);
	p_write_item->SetSocket(mn_socket);
	mp_list_mgr->push(p_write_item, tp_network_writer);
	return ;
}
