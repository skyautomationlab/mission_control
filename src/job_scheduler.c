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
		mysql_result_bucket *somebucket = NULL;
		if(sql_send_query(&somebucket,GET_CANDIDATE_JOBS) == 0)
		{
			int x = 0;
			for(x = 0; x < somebucket->row_count; ++x)
			{
				printf("%s\n",somebucket->rows[x][0]);
			}
			remove_mysql_result_bucket(&somebucket);
		}	
		sleep(1);
	}
	while(1);
}
void job_scheduler_start()
{
	printf("Job scheduler starting\n");
	if(sql_setup_credentials() == 0){
		job_scheduler_loop();
	}
}
