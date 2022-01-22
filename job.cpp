#include "job.h"

CJob::CJob(void* data)
{
	m_argument = data;
}

CJob::~CJob()
{
	
}

void CJob::SetThread(CThread* p)
{
	m_thread = p;
}
