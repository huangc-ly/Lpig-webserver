/*
 * Lpig_server.cpp
 *
 *  Created on: 2012-5-2
 *      Author: kevin
 */
#include <cstdio>
#include <iostream>
#include "Lpig_server.h"
#include <cstring>
#include "log.h"
#include <signal.h>
#include <errno.h>
#include "network_IO_read.h"
#include <netdb.h>

extern utility::log *gp_log;

Lpig_server::Lpig_server()
{
	mp_network_reader = NULL;
	mp_network_writer = NULL;
	mp_disk_reader = NULL;
	mp_list_mgr = NULL;
	memset(&m_srv_addr, 0, sizeof(m_srv_addr));
}

Lpig_server::~Lpig_server()
{
	gp_log->write_log(utility::ll_important, "network_reader is freeing");
	if(mp_network_reader != NULL)
		delete mp_network_reader;

	gp_log->write_log(utility::ll_important, "network_writer is freeing");
	if(mp_network_writer != NULL)
		delete mp_network_writer;

	gp_log->write_log(utility::ll_important, "disk_reader is freeing");
	if(mp_disk_reader != NULL)
		delete mp_disk_reader;
}

bool Lpig_server::init()
{
	gp_log->write_log(utility::ll_important, "Lpig-server starts init");

	gp_log->write_log(utility::ll_important, "Creating job_list_mgr");
	if(mp_list_mgr == NULL)
		mp_list_mgr = new job_list_mgr();

	gp_log->write_log(utility::ll_important, "Creating network_reader threadpool");
	if(mp_network_reader == NULL)
	{
		mp_network_reader = new network_reader_pool(mp_list_mgr, 25);
		mp_network_reader->init();
	}

	gp_log->write_log(utility::ll_important, "Creating network_writer threadpool");
	if(mp_network_writer == NULL)
	{
		mp_network_writer = new network_writer_pool(mp_list_mgr, 25);
		mp_network_writer->init();
	}
	gp_log->write_log(utility::ll_important, "Creating disk_reader threadpool");
	if(mp_disk_reader == NULL)
	{
		mp_disk_reader = new disk_reader_pool(mp_list_mgr, 30);
		mp_disk_reader->init();
	}
	mn_listen_port = m_conf.mn_listen_port = 80;
	m_conf.ms_host.append("127.0.0.1");

	if(!_start_listen())
		return false;

	return true;
}

bool Lpig_server::run()
{
	gp_log->write_log(utility::ll_important, "Lpig_server starts to run");

	size_t n_events;
	while(true)
	{
		n_events = monitor();
		if(n_events >= 0)
		{
			handle_events();
		}
		else
		{
			gp_log->write_log(utility::ll_error, "Lpig_server monitor failed");
			return false;
		}
	}
	return true;
}

void Lpig_server::on_events(int n_fd, int event)
{
	int n_errno = errno = 0;
	if(_is_listen_fd(n_fd))
	{
		if(event & (EPOLLHUP | EPOLLERR))
		{
			gp_log->write_log(utility::ll_error, "listening socket: %d error, event: %d", n_fd, event);
			exit(1);
		}
		else
		{
			while(true)
			{
				sockaddr_in sa;
				memset(&sa, 0, sizeof(sa));
				unsigned int n_socklen = sizeof(sa);
				errno = 0;
				int n_new_sock = accept(n_fd, (sockaddr *)&sa, &n_socklen);
				n_errno = errno;
				if(n_new_sock > 0)
				{
					gp_log->write_log(utility::ll_normal, "ip: %s, port: %d connects to server", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
				}
				else
				{
					if(errno != EAGAIN)
						gp_log->write_log(utility::ll_important, "accept failed, accept: %s", strerror(n_errno));
					return ;
				}
				set_non_blocking(n_new_sock);
				add(n_new_sock, EPOLLET | EPOLLIN | EPOLLHUP | EPOLLERR);
			}
		}
	}
	else if(event & EPOLLIN)
	{
		gp_log->write_log(utility::ll_normal, "socket: %d has read event", n_fd);
		network_IO_read_item *p_network_read_item = new network_IO_read_item(mp_list_mgr);
		p_network_read_item->SetEvent(EPOLLIN);
		p_network_read_item->SetSocket(n_fd);
		mp_list_mgr->push(p_network_read_item, tp_network_reader);
	}
	else if(event & (EPOLLERR | EPOLLHUP))
	{
		gp_log->write_log(utility::ll_important, "socket: %d has error or hang-up event", n_fd);
		del(n_fd, EPOLLET | EPOLLIN | EPOLLHUP | EPOLLERR);
		close(n_fd);
		n_fd = -1;
	}

}

bool Lpig_server::_is_listen_fd(int n_fd)
{
	assert(n_fd != -1);

	if(n_fd == mn_listen_fd)
		return true;
	return false;
}

bool Lpig_server::_start_listen()
{
	assert(gp_log != NULL);
	int n_errno = errno = 0;

	errno = 0;
	int n_socket = socket(AF_INET, SOCK_STREAM, 0);
	n_errno = errno;
	if(n_socket == -1)
	{
		gp_log->write_log(utility::ll_error, "Create listen socket failed, socket: %s", strerror(n_errno));
		return false;
	}
	set_non_blocking(n_socket);
	set_reuse_port(n_socket);
	if(!_generate_addr(mn_listen_port))
	{
		return false;
	}
	errno = 0;
	int n_result = bind(n_socket, (sockaddr*)&m_srv_addr, sizeof(m_srv_addr));
	n_errno = errno;
	if(n_result == -1)
	{
		gp_log->write_log(utility::ll_error, "bind failed, bind: %s", strerror(n_errno));
		close(n_socket);
		n_socket = -1;
		return false;
	}

	errno = 0;
	n_result = listen(n_socket, LISTEN_BACKLOG);
	n_errno = errno;
	if(n_result == -1)
	{
		gp_log->write_log(utility::ll_error, "listen failed, socket: %d, listen: %s", n_socket, strerror(n_errno));
		return false;
	}
	bool b_result = add(n_socket, EPOLLIN | EPOLLERR |EPOLLHUP);
	assert(b_result);

	mn_listen_fd = n_socket;
	return true;
}

bool Lpig_server::_generate_addr(unsigned short n_port)
{
	assert(m_conf.ms_host.size() != 0);

	m_srv_addr.sin_family = AF_INET;
	m_srv_addr.sin_port = htons(n_port);

	int n_result = inet_pton(AF_INET, m_conf.ms_host.c_str(), &m_srv_addr.sin_addr);
	if(n_result <= 0)
	{
		gp_log->write_log(utility::ll_error, "inet_pton failed, return value: %d", n_result);
		return false;
	}

	return true;
}





