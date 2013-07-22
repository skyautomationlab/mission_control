/*
 * =====================================================================================
 *
 *       Filename:  tranceiver_control.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/01/13 09:11:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  AlexsJones (), alexsimonjones@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include "../conversion/base64.h"
#include <jnxc_headers/jnxfile.h>
#include <jnxc_headers/jnxnetwork.h>
#include <jnxc_headers/jnxterm.h>
#include <jnxc_headers/jnxhash.h>
#include <pthread.h>
#include "../result_management.h"
#include "../database/sql_interface_layer.h"
#include "transceiver_control.h"
#include "transaction_api.h"
#define MAX_CONNECTIONS 24
extern jnx_hashmap *config;
int transceiver_control_query(char *hostaddr, char *hostport, const char *template, ...)
{
	printf("host addr %s\n",hostaddr);
	printf("Host port %s\n",hostport);
	char query[2048];
	va_list ap;
	va_start(ap,template);
	vsprintf(query,template,ap);
	va_end(ap);
	jnx_term_printf_in_color(JNX_COL_YELLOW,"OUT : %s\n",query);
	if(jnx_network_send_message(hostaddr,atoi(hostport),query,strlen(query)) != -1)
	{
		return 0;
	}else
	{
		return 1;
	}

}
int transceiver_control_start_dialogue(char *machine_ip,char *machine_port,char *job_id, char *job_command)
{
	return transceiver_control_query(machine_ip,machine_port,API_COMMAND,"JOB",job_id,job_command,/* Other field is left empty */ "",(char*)jnx_hash_get(config,"MISSIONCONTROLIP"),(char*)jnx_hash_get(config,"MISSIONCONTROLPORT"));
}
/*-----------------------------------------------------------------------------
 * Below is the transceiver receiver logic. 
 * A new thread will be spawned for every received query 
 *-----------------------------------------------------------------------------*/
void *transciever_control_endpoint_worker(void *arg)
{
	char *query = (char*)arg;
	printf("Raw query %s\n",query);
	jnx_term_printf_in_color(JNX_COL_RED,"raw length %d\n",strlen(query));
	api_command_obj *obj = transaction_api_create_obj(query);
	jnx_term_printf_in_color(JNX_COL_BLUE,"IN : CMD:%d ID:%s DATA:%s OTHER:%s SENDER:%s PORT:%d\n",obj->CMD,obj->ID,obj->DATA,obj->OTHER,obj->SENDER,obj->PORT);

	switch(obj->CMD)
	{
		case JOB:
			printf("transceiver_control_listener_endpoint_worker : Not expecting to be sent JOB from an open node dialogue\n");
			break;
		case RESULT:
			jnx_term_printf_in_color(JNX_COL_GREEN,"Got result\n");
			size_t output;
			char *decoded_output = base64_decode(obj->DATA,strlen(obj->DATA),&output);		
			
			printf("decoded %s\n",decoded_output);
			if(obj->OTHER)
			{
				/*  get job trigger_time */
				mysql_result_bucket *trigger_bucket = NULL;
				if(sql_send_query(&trigger_bucket,GET_JOB_TRIGGER_TIME,obj->ID) == 0)
				{
					time_t trigger_time = (time_t)trigger_bucket->rows[0][get_mysql_result_bucket_field_position(&trigger_bucket,"trigger_time")];	
					if(trigger_time){	
					char *resultspath = result_management_full_path_create(obj->ID,obj->OTHER,trigger_time);
					jnx_file_write(resultspath,decoded_output,output);
					free(resultspath);
					remove_mysql_result_bucket(&trigger_bucket);
					}
				}
			}
			printf("decoded_output length %zu\n",output);
			free(decoded_output);	
			break;
		case STATUS:
			printf("transciever_control_endpoint_worker : Writing status update\n");
			mysql_result_bucket *statusbucket = NULL;
			if(sql_send_query(&statusbucket,SET_JOB_STATUS,obj->ID,obj->DATA) != 0)
			{
				printf("sql_send_query updating job status via transceiver_control_listener_endpoint_worker for job %s\n",obj->ID);
			}
			if(strcmp(obj->DATA,"COMPLETED") == 0)
			{
				mysql_result_bucket *completedbucket = NULL;
				if(sql_send_query(&completedbucket,UPDATE_JOB_INTERVAL,obj->ID) != 0)
				{
					printf("sql_send_query error updating job interval for job %s\n",obj->ID);
				}
			}	
			break;

		case ALIVE:
		
			if(sql_send_query(NULL,SET_MACHINE_STATUS,obj->DATA,obj->SENDER) != 0)
			{
					printf("sql_send_query error updating machine status for machine %s\n",obj->SENDER);
			}
					
			break;
		case UNKNOWN:
			printf("transciever_control_endpoint_worker : Unknown API command %d\n",obj->CMD);	
			break;
		default:
			printf("transciever_control_endpoint_worker : Unknown API command %d\n",obj->CMD);	
			break;
	}

	transaction_api_delete_obj(obj);
}
void transceiver_control_listener_endpoint(char *incoming_query,size_t query_len, char *incoming_ip)
{
	printf("transceiver_control_listener_endpoint receieved message from  %s\n",incoming_ip);
	pthread_t worker_thread;
	pthread_create(&worker_thread,NULL,transciever_control_endpoint_worker,incoming_query);
}
void *transceiver_control_listener_scheduler(void *arg)
{
	assert(jnx_hash_get(config,"MISSIONCONTROLPORT"));
	jnx_network_listener_callback lsc = &transceiver_control_listener_endpoint;
	jnx_term_printf_in_color(JNX_COL_GREEN,"Transceiver online\n");	
	jnx_network_setup_listener(atoi((char*)jnx_hash_get(config,"MISSIONCONTROLPORT")),MAX_CONNECTIONS,lsc);
}
