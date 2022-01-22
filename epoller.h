/*
 * epoller.h
 *
 *  Created on: 2012-5-1
 *      Author: kevin
 */



#ifndef EPOLLER_H_
#define EPOLLER_H_

#include <sys/epoll.h>

class epoller
{
public:
	epoller();
	virtual ~epoller();
	unsigned int monitor(int = 500);
	bool add(int fd, unsigned int events);
	bool del(int fd, unsigned int events);
	bool modify(int fd, unsigned int events);
	int ctl(int fd, int op, struct epoll_event &event);
	void handle_events();
	virtual void on_events(int n_fd, int event) = 0;
	bool set_non_blocking(int n_fd);
	bool set_reuse_port(int n_fd);
	static const size_t EPOLL_SIZE = 30000;
private:
	int mn_fd;
	int mn_event_fds;
	epoll_event event_pool[EPOLL_SIZE];
};

#endif /* EPOLLER_H_ */
