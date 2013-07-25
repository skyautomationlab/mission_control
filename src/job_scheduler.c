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
#include <jnxc_headers/jnxterm.h>
#include "network/transceiver_control.h"
#include "job_scheduler.h"
#include "database/sql_interface_layer.h"
#define TIME_WAIT sleep(1);

/*-----------------------------------------------------------------------------
 *  
 * Job states
 *
 *  'NOT STARTED' -> 'QUEUED' -> 'IN PROGRESS' -> 'COMPLETED' 
 *
 *-----------------------------------------------------------------------------*/
typedef enum trigger_status { ALREADYRUN,READYTORUN,NOTREADYTORUN }trigger_status;
int job_scheduler_check_time(time_t candidate_time, long *difference)
{
	time_t current_time = time(0);
	(*difference) = (candidate_time - current_time);
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
	return NOTREADYTORUN;
}
void job_scheduler_loop()
{
	jnx_term_printf_in_color(JNX_COL_GREEN,"Job scheduler online\n");	
	do
	{
		mysql_result_bucket *jobbucket = NULL;
		if(sql_send_query(&jobbucket,GET_CANDIDATE_JOBS) == 0)
		{
			int x;
			for(x = 0; x < jobbucket->row_count; ++x)
			{
				time_t job_trigger = atoi(jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"trigger_time")]);
				long difference;
				trigger_status status = job_scheduler_check_time(job_trigger,&difference);
				char *job_status = jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"status")];
				switch(status)
				{
					case ALREADYRUN:
						break;
					case READYTORUN:
						/*-----------------------------------------------------------------------------
						 *  Check whether the job has already been queued, if not - continue
						 *-----------------------------------------------------------------------------*/
						if(strcmp(job_status,"QUEUED") == 0)
						{
							printf("Job %s is queued but has not been rescheduled to run\n",jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"name")]);
						}
						if(strcmp(job_status,"NOT STARTED") == 0)
						{
							mysql_result_bucket *send_bucket = NULL;
							/*-----------------------------------------------------------------------------
							 *  Set the machine status to queued
							 *-----------------------------------------------------------------------------*/
							if(sql_send_query(&send_bucket,SET_JOB_STATUS,jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"id")],"QUEUED") == 0)
							{
								mysql_result_bucket *machine_ip_bucket = NULL;	
								char *machine_ip = NULL;
								char *machine_port = NULL;
								/*-----------------------------------------------------------------------------
								 *  Get machine_id
								 *-----------------------------------------------------------------------------*/
								if(sql_send_query(&machine_ip_bucket,GET_MACHINE_FROM_ID,atoi(jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"machine_id")])) == 0)
								{
									machine_ip = machine_ip_bucket->rows[0][get_mysql_result_bucket_field_position(&machine_ip_bucket,"ip_address")];
									machine_port = machine_ip_bucket->rows[0][get_mysql_result_bucket_field_position(&machine_ip_bucket,"port")];
								}
								remove_mysql_result_bucket(&machine_ip_bucket);
								if(machine_ip == NULL || machine_port == NULL)
								{
									jnx_term_printf_in_color(JNX_COL_RED,"Error resolving machine_ip and port\n");
									exit(1);
								}
								/*-----------------------------------------------------------------------------
								 *  Send our components to transceiver control and start a dialogue 
								 *-----------------------------------------------------------------------------*/
								if(transceiver_control_start_dialogue(machine_ip,machine_port,jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"id")],jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"command")]) != 0)
								{
									jnx_term_printf_in_color(JNX_COL_RED,"Error sending to transceiver_control_start_dialogue\n");
									/*-----------------------------------------------------------------------------
									 *  Send failure status to sql
									 *-----------------------------------------------------------------------------*/
									mysql_result_bucket *failure_bucket = NULL;
									if(sql_send_query(&failure_bucket,SET_JOB_STATUS,jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"id")],"FAILED") == 0)
									{
										free(failure_bucket);
									}else
									{
										jnx_term_printf_in_color(JNX_COL_RED,"Error updating job status via sql_send_query\n");
									}

									/*-----------------------------------------------------------------------------
									 *  The node will tell us when the job is progressing, not the other way around!
									 *-----------------------------------------------------------------------------*/
								}
							}
							free(send_bucket);
						}
						break;
					case NOTREADYTORUN:
						printf("%s has status of NOT READY TO RUN %lds\n",jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"name")],difference);
						break;
				}	
			}
			remove_mysql_result_bucket(&jobbucket);
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
