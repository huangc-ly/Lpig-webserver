/*
 * lock.h
 *
 *  Created on: 2012-4-14
 *      Author: kevin
 */

#ifndef LOCK_H_
#define LOCK_H_

#include <pthread.h>
#include <cstdlib>

namespace utility
{
class mutex;

class scope_lock
{
public:
	scope_lock(mutex *p_mutex);
	~scope_lock();
private:
	scope_lock(const scope_lock&);
	scope_lock& operator=(scope_lock&);
	mutex *mp_mutex;
};

class mutex
{
public:
	mutex();
	~mutex();
	bool init();
	bool lock();
	bool unlock();
protected:
	pthread_mutex_t *mp_mutex;
};

}
#endif /* LOCK_H_ */
