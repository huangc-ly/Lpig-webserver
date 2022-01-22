/*
 * Lpig_server.h
 *
 *  Created on: 2012-5-2
 *      Author: kevin
 */

#ifndef LPIG_SERVER_H_
#define LPIG_SERVER_H_

#include "epoller.h"
#include "log.h"
#include "job.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "threadpool.h"
#include <string>
#include <vector>
#include "lock.h"

extern utility::log *gp_log;

struct srv_config
{
	std::string ms_host;
	unsigned short mn_listen_port;
};

class Lpig_server : public epoller
{
public:
	Lpig_server();
	virtual ~Lpig_server();
	bool run();
	bool init();
	static const size_t LISTEN_BACKLOG = 50;
//	bool parse_configure_file();

	CThreadPool *get_network_reader_pool() const
	{
		return mp_network_reader;
	}

	CThreadPool *get_network_writer_pool() const
	{
		return mp_network_writer;
	}

	CThreadPool *get_disk_reader_pool() const
	{
		return mp_disk_reader;
	}

	job_list_mgr *get_list_mgr() const
	{
		return mp_list_mgr;
	}


protected:
	virtual void on_events(int fd, int event);

private:
	bool _is_listen_fd(int n_fd);
	bool _start_listen();
	bool _generate_addr(unsigned short n_port);

	CThreadPool *mp_network_reader;
	CThreadPool *mp_network_writer;
	CThreadPool *mp_disk_reader;
	job_list_mgr *mp_list_mgr;
	struct sockaddr_in m_srv_addr;
	struct srv_config m_conf;
	int mn_listen_fd;
	unsigned short mn_listen_port;
};

#endif /* LPIG_SERVER_H_ */
