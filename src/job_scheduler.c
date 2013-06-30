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
#include <time.h>
#include "job_scheduler.h"
#include "database/sql_interface_layer.h"
#define TIME_WAIT sleep(1);
typedef enum trigger_status { ALREADYRUN,READYTORUN,NOTREADYTORUN }trigger_status;
int job_scheduler_check_time(time_t candidate_time)
{
	time_t current_time = time(0);
	if((candidate_time - current_time) < 0)
	{
		return ALREADYRUN;
	}
	if((candidate_time - current_time) <= 60)
	{
		return READYTORUN;
	}else
	{
		return NOTREADYTORUN;
	}
}
void job_scheduler_loop()
{
	do
	{
		mysql_result_bucket *somebucket = NULL;
		if(sql_send_query(&somebucket,GET_CANDIDATE_JOBS) == 0)
		{
			int x;
			for(x = 0; x < somebucket->row_count; ++x)
			{
				trigger_status status = job_scheduler_check_time(atoi(somebucket->rows[x][get_mysql_result_bucket_field_position(&somebucket,"trigger_time")]));
				switch(status)
				{
					case ALREADYRUN:
						printf("Already run\n");
						break;
					case READYTORUN:
						printf("Ready to run\n");
						break;
					case NOTREADYTORUN:
						printf("Not ready to run\n");
						break;
				}	
			}
			remove_mysql_result_bucket(&somebucket);
		}	
		TIME_WAIT
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
