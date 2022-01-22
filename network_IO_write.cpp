/*
 * network_IO_write.cpp
 *
 *  Created on: 2012-5-3
 *      Author: kevin
 */

#include <string>
#include "network_IO_write.h"
#include "log.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cstring>

extern utility::log *gp_log;

network_IO_write_item::network_IO_write_item(job_list_mgr *p_list_mgr, const std::string &s_data, int n_status)
	:ms_reply_data(s_data)
{
	mp_list_mgr = p_list_mgr;
	mn_reply_status = n_status;
}

network_IO_write_item::~network_IO_write_item()
{

}

void network_IO_write_item::_make_http_packet()
{
	std::string s_head;
	char sz_buf[1024];

	switch(mn_reply_status)
	{
	case rs_ok:
		s_head.append("http/1.0 200 OK\r\n");
		break;
	case rs_not_exists:
		s_head.append("http/1.1 404 Not Found\r\n");
		ms_reply_data.append("<html>\r\n<head><title>404 Not Found</title></head>\r\n");
		ms_reply_data.append("<body bgcolor=\"white\">\r\n<center><h1>404 Not Found</h1></center>\r\n");
		ms_reply_data.append("<hr><center>Lpig</center>\r\n</body>\r\n</html>");
		break;
	case rs_is_directory:
		s_head.append("http/1.1 403 Forbidden\r\n");
		ms_reply_data.append("<html>\r\n<head><title>403 Forbidden</title></head>\r\n");
		ms_reply_data.append("<body bgcolor=\"white\">\r\n<center><h1>403 Forbidden</h1></center>\r\n");
		ms_reply_data.append("<hr><center>Lpig</center>\r\n</body>\r\n</html>");
		break;
	default:
		gp_log->write_log(utility::ll_error, "Unknown reply status: %d", mn_reply_status);
		break;
	}

	sprintf(sz_buf, "Content-length: %d\r\n", ms_reply_data.size());
	s_head.append("Server: Lpig\r\n");
	s_head.append(sz_buf);
	s_head.append("Content-type: text/html\r\n");
	s_head.append("\r\n");

	ms_reply_data.insert(0, s_head);
}

void network_IO_write_item::Run()
{
	assert(gp_log != NULL);

	if(mn_socket == -1)
	{
		gp_log->write_log(utility::ll_important, "Before sending data to client, socket has been shutdown");
		return;
	}
	_make_http_packet();

	int n_errno = errno = 0;
	int n_size = ms_reply_data.size();
	const char *p_ptr = ms_reply_data.c_str();
	int n_write;

	while(true)
	{
		errno = 0;
		n_write = send(mn_socket, p_ptr, n_size, MSG_NOSIGNAL);
		n_errno = errno;
		if(n_write > 0)
		{
			n_size -= n_write;
			p_ptr += n_write;
			if(n_size == 0)
				break;
		}
		else
		{
			if(errno == EAGAIN)
			{
				usleep(30000);
				continue;
			}
			else
			{
				gp_log->write_log(utility::ll_important, "socket: %d, while sending data to client, send failed, send: %s", mn_socket, strerror(n_errno));
				break;
			}

		}
	}
	close(mn_socket);
	mn_socket = -1;
}

