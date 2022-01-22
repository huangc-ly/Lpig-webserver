/*
 * log.cpp
 *
 *  Created on: 2012-4-14
 *      Author: kevin
 */

#include "lock.h"
#include "log.h"
#include <cassert>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>

using namespace utility;
using namespace std;

const char* LEVEL_DESC[] = { "Normal : ", "Important : ", "Error : " };

log::log(const string& s_filename, size_t n_filesize)
	:ms_filename(s_filename), mn_filesize(n_filesize)
{
	assert(ms_filename.c_str() != NULL);
	assert(mn_filesize != 0);

}

log::~log()
{
	m_file.close();
}

bool log::init()
{
	m_file.open(ms_filename.c_str(), ios::out | ios::app);
	assert(m_file.is_open());
	m_mutex.init();

	return true;
}

bool log::_get_cur_time()
{
	char sz_current[128];
	time_t curr_time = time(NULL);
	tm *p_curr = localtime(&curr_time);
	strftime(sz_current, 128, "%Y-%m-%d %H:%M:%S ", p_curr);

	struct timeval now_tb;
	gettimeofday(&now_tb, NULL);

	char sz_buf[256];
	sprintf(sz_buf, "%s(%d)", sz_current, now_tb.tv_usec);
	m_file << sz_buf;

	return true;
}

bool log::write_log(enum log_level e_ll, const char *p_fmt, ...)
{
	assert(p_fmt != NULL);
	assert(m_file.is_open());
	static const unsigned int MAX_BUF_SIZE = 5 * 4096;
	char sz_log_buf[MAX_BUF_SIZE];

	va_list arg_list;
	va_start(arg_list, p_fmt);
	vsnprintf(sz_log_buf, MAX_BUF_SIZE, p_fmt, arg_list);
	va_end(arg_list);

	_inner_write(e_ll, sz_log_buf);
	return true;
}

bool log::_inner_write(enum log_level e_ll, const char *p_string)
{
	assert(p_string != NULL);

	if(m_file.is_open())
	{
		pthread_t n_id = pthread_self();
		scope_lock lock(&m_mutex);

		_get_cur_time();
		m_file.setf(std::ios::showbase);
		m_file << "[" << std::hex << n_id << "]" << std::dec;
		m_file << LEVEL_DESC[e_ll] << p_string << std::endl;

		// todo: check file size to see whether file is big enough
	}
	return true;
}
