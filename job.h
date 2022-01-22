#ifndef JOB_H
#define JOB_H

#include <iostream>
#include <sys/socket.h>
#include <cassert>

class CThread;

enum reply_status
{
	rs_ok,
	rs_not_exists,
	rs_is_directory,
};

class CJob
{
	public:
		CJob(void* = NULL);
		virtual ~CJob();
		virtual void Run() = 0;
		void SetThread(CThread* p);
		void SetSocket(int sock)
		{
			mn_socket = sock;
		}
		void SetEvent(unsigned int event)
		{
			mn_event = event;
		}
	protected:
		void* m_argument;
		CThread* m_thread;
		int mn_socket;
		unsigned int mn_event;
};

#endif
