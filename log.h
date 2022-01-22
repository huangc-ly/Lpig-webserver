/*
 * log.h
 *
 *  Created on: 2012-4-14
 *      Author: kevin
 */

#ifndef LOG_H_
#define LOG_H_

#include <fstream>
#include <cstdlib>
#include <string>
#include "lock.h"

namespace utility
{
enum log_level
{
	ll_normal,
	ll_important,
	ll_error,
};

class log
{
public:
	log(const std::string& s_filename, size_t n_filesize);
	~log();
	bool init();
	bool write_log(log_level level, const char *p_fmt, ...);
private:
	std::string ms_filename;
	size_t mn_filesize;
	std::fstream m_file;
	mutex m_mutex;
	bool _inner_write(log_level level,const char *p_string);
	bool _get_cur_time();
};
}
#endif /* LOG_H_ */
