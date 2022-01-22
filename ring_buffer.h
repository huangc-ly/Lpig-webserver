/*
 * ring_buffer.h
 *
 *  Created on: 2012-5-5
 *      Author: root
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <cstdio>
#include <vector>

template<typename T>
class ring_buf
{
public:
	ring_buf(int n_size, T val = NULL)
	{
		m_ring_buf.resize(n_size, val);
		mn_total_num = n_size;
		mn_current_num = 0;
		mn_tail = mn_front = -1;
	}
	~ring_buf()
	{
	}
	bool push(const T &val)
	{
		if(mn_current_num >= mn_total_num)
			return false;
		if(++mn_tail == mn_total_num)
			mn_tail = 0;
		m_ring_buf[mn_tail] = val;
		++mn_current_num;
		return true;

	}

	bool pop(T &val)
	{
		if(mn_current_num <= 0)
			return false;
		if(++mn_front == mn_total_num)
			mn_front = 0;
		val = m_ring_buf[mn_front];
		--mn_current_num;
		return true;
	}

private:
	std::vector<T> m_ring_buf;
	size_t mn_total_num;
	size_t mn_current_num;
	int mn_front;
	int mn_tail;
};


#endif /* RING_BUFFER_H_ */
