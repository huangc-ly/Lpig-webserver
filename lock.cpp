/*
 * lock.cpp
 *
 *  Created on: 2012-4-14
 *      Author: kevin
 */

#include "lock.h"
#include <pthread.h>
#include <cassert>

using namespace std;
using namespace utility;

mutex::mutex()
{
	mp_mutex = NULL;
}

mutex::~mutex()
{
	if(mp_mutex != NULL)
	{
		pthread_mutex_destroy(mp_mutex);
		delete mp_mutex;
		mp_mutex = 0;
	}
}

bool mutex::init()
{
	if(mp_mutex == NULL)
	{
		mp_mutex = new pthread_mutex_t();
		assert(mp_mutex != NULL);
		pthread_mutex_init(mp_mutex, NULL);
	}
	return true;
}

bool mutex::lock()
{
	assert(mp_mutex != NULL);
	pthread_mutex_lock(mp_mutex);
	return true;
}

bool mutex::unlock()
{
	assert(mp_mutex != NULL);
	pthread_mutex_unlock(mp_mutex);
	return true;
}

scope_lock::scope_lock(mutex *p_mutex)
	:mp_mutex(p_mutex)
{
	assert(mp_mutex != NULL);
	mp_mutex->lock();
}

scope_lock::~scope_lock()
{
	mp_mutex->unlock();
	mp_mutex = NULL;
}



