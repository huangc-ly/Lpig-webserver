/*
 * main.cpp
 *
 *  Created on: 2012-5-4
 *      Author: kevin
 */

#include "log.h"
#include "Lpig_server.h"
#include "network_IO_read.h"
#include "network_IO_write.h"
#include "disk_requests.h"
#include "threadpool.h"

using namespace utility;

log *gp_log = new log("log", 1000);

int main()
{
	gp_log->init();
	gp_log->write_log(ll_important, "In main, starting Lpig_server...");

	Lpig_server * p_srv = new Lpig_server();

	if(!p_srv)
	{
		gp_log->write_log(ll_error, "Creating Lpig_server failed");
		exit(1);
	}
	if(!p_srv->init())
	{
		gp_log->write_log(ll_error, "server init flase, Lpig exiting...");
		exit(1);
	}

	p_srv->run();

	return 0;
}


