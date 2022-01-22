/*
 * epoller.cpp
 *
 *  Created on: 2012-5-1
 *      Author: kevin
 */

#include <cassert>
#include "epoller.h"
#include <errno.h>
#include <fcntl.h>
#include "log.h"
#include <sys/socket.h>
#include <sys/types.h>

extern utility::log *gp_log;

epoller::epoller()
{
	int n_errno = errno =  0;
	mn_fd = epoll_create(EPOLL_SIZE);
	n_errno = errno;
	if(mn_fd == -1)
	{
		gp_log->write_log(utility::ll_error, "Create epoll fd error: errno = %d\n", n_errno);
		exit(1);
	}
}

epoller::~epoller()
{

}

bool epoller::add(int fd, unsigned int events)
{
	assert(fd != -1);
	epoll_event polevt;
	polevt.events = events;
	polevt.data.fd = fd;
	int n_result = ctl(fd, EPOLL_CTL_ADD, polevt);
	gp_log->write_log(utility::ll_normal, "fd: %d add to epoll, return value %d", fd, n_result);
	return n_result == 0 ? true : false;
}

bool epoller::del(int fd, unsigned int events)
{
	assert(fd != -1);
	epoll_event polevt;
	polevt.events = events;
	polevt.data.fd = fd;
	int n_result = ctl(fd, EPOLL_CTL_DEL, polevt);
	gp_log->write_log(utility::ll_normal, "fd: %d delete from epoll, return value %d", fd, n_result);
	return n_result;
}

bool epoller::modify(int fd, unsigned int events)
{
	assert(fd != -1);
	epoll_event polevt;
	polevt.events = events;
	polevt.data.fd = fd;
	int n_result = ctl(fd, EPOLL_CTL_MOD, polevt);
	gp_log->write_log(utility::ll_normal, "fd: %d modified, return value %d", fd, n_result);
	return n_result;
}

int epoller::ctl(int fd, int op, epoll_event &event)
{
	int n_errno = errno = 0;
	int n_result = epoll_ctl(mn_fd, op, fd, &event);
	n_errno = errno;
	if(n_result != 0)
	{
		if((n_errno == ENOENT) && (op == EPOLL_CTL_MOD || op == EPOLL_CTL_DEL))
		{
			if(op == EPOLL_CTL_MOD)
				gp_log->write_log(utility::ll_error, "modify fd = %d but not registered with this epoll instance", fd);
			else
				gp_log->write_log(utility::ll_error, "remove fd = %d but not registered with this epoll instance", fd);
		}
		else
		{
			if(op == EPOLL_CTL_MOD)
				gp_log->write_log(utility::ll_error, "epoll_ctl failed, modify fd = %d, errno = %d", fd, n_errno);
			else
				gp_log->write_log(utility::ll_error, "epoll_ctl failed, remove fd = %d, errno = %d", fd, n_errno);
		}
	}
	return n_result;
}

void epoller::handle_events()
{
	for(int i = 0; i < mn_event_fds; ++i)
	{
		on_events(event_pool[i].data.fd, event_pool[i].events);
	}
}

unsigned int epoller::monitor(int timeout)
{
	int n_errno = errno = 0;
	int n_result = epoll_wait(mn_fd, event_pool, EPOLL_SIZE, timeout);
	n_errno = errno;
	if(n_result == -1)
	{
		if(n_errno == EINTR)
		{
			gp_log->write_log(utility::ll_important, "epoll_wait is interrupted by signal");
			return 0;
		}
		gp_log->write_log(utility::ll_error, "epoll_wait failed, errno = %d", n_errno);
		exit(2);
	}
	mn_event_fds = n_result;
	return n_result;
}

bool epoller::set_non_blocking(int n_fd)
{
	assert(n_fd != -1);
	int flags;
	if((flags = fcntl(n_fd, F_GETFL, 0)) < 0)
	{
		gp_log->write_log(utility::ll_error, "get fd: %d flags failed", n_fd);
		return false;
	}
	flags |= O_NONBLOCK;
	if(fcntl(n_fd, F_SETFL, flags) < 0)
	{
		gp_log->write_log(utility::ll_error, "set fd: %d non-blocking failed", n_fd);
		return false;
	}
	return true;
}

bool epoller::set_reuse_port(int n_fd)
{
	int on = 1;
	int result = setsockopt(n_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(int));
	return result != -1;
}


