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
#include "network/transceiver_control.h"
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
		mysql_result_bucket *jobbucket = NULL;
		if(sql_send_query(&jobbucket,GET_CANDIDATE_JOBS) == 0)
		{
			int x;
			for(x = 0; x < jobbucket->row_count; ++x)
			{
				trigger_status status = job_scheduler_check_time(atoi(jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"trigger_time")]));
				switch(status)
				{
					case ALREADYRUN:
						break;
					case READYTORUN:
						if(strcmp(jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"status")],"QUEUED") != 0)
						{
							mysql_result_bucket *send_bucket = NULL;
							if(sql_send_query(&send_bucket,SET_MACHINE_STATUS,"QUEUED",jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"machine_id")]) == 0)
							{

								mysql_result_bucket *machine_ip_bucket = NULL;	
								char *machine_ip = NULL;
								char *machine_port = NULL;
								if(sql_send_query(&machine_ip_bucket,GET_MACHINE_FROM_ID,atoi(jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"machine_id")])) == 0)
								{
									machine_ip = machine_ip_bucket->rows[0][get_mysql_result_bucket_field_position(&machine_ip_bucket,"ip_address")];
									machine_port = machine_ip_bucket->rows[0][get_mysql_result_bucket_field_position(&machine_ip_bucket,"port")];
								}
								remove_mysql_result_bucket(&machine_ip_bucket);
								if(machine_ip == NULL || machine_port == NULL)
								{
									printf("Could not resolve machine ip and port\n");
									exit(1);
								}
								if(transceiver_control_start_dialogue(machine_ip,machine_port,jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"id")],jobbucket->rows[x][get_mysql_result_bucket_field_position(&jobbucket,"command")]) != 0)
								{
									printf("error sending to transceiver_control_start_dialogue\n");
									exit(1);
								}
							}
							free(send_bucket);
						}
						break;
					case NOTREADYTORUN:
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
