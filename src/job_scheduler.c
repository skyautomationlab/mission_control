/*
 * =====================================================================================
 *
 *       Filename:  job_scheduler.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/27/13 20:31:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  AlexsJones (), alexsimonjones@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "job_scheduler.h"
#include "database/sql_interface_layer.h"
void job_scheduler_loop()
{
	do
	{
		mysql_result_bucket *somebucket;
		sql_send_query(&somebucket,TEST,"A","B",53);
		//get jobs that we need to do!
		//
		sleep(1);
	}
	while(1);
}
void job_scheduler_start(void)
{
	printf("Job scheduler starting\n");
	if(sql_setup_credentials() == 0){
	job_scheduler_loop();
	}
}
