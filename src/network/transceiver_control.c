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
#include <stdio.h>
#include <stdarg.h>
#include <jnxc_headers/jnxnetwork.h>
#include <jnxc_headers/jnxterm.h>
#include <jnxc_headers/jnxhash.h>
#include <pthread.h>
#include "transceiver_control.h"
#include "transaction_api.h"
extern jnx_hashmap *config;
void transceiver_control_query_callback(char *reply)
{
	printf("transceiver_control_query_callback: %s\n",reply);
}
int transceiver_control_query(char *hostaddr, char *hostport, const char *template, ...)
{
	char query[1024];
	va_list ap;
	va_start(ap,template);
	vsprintf(query,template,ap);
	va_end(ap);
#ifdef DEBUG
	printf("Sending %s:%s query -> %s\n",hostaddr,hostport,query);
#endif
	jnx_network_send_message_callback smc = &transceiver_control_query_callback;	
	return jnx_network_send_message(hostaddr,atoi(hostport),query,smc);	
}
int transceiver_control_start_dialogue(char *machine_ip,char *machine_port,char *job_id, char *job_command)
{
	return transceiver_control_query(machine_ip,machine_port,API_COMMAND,"JOB",job_id,job_command,jnx_hash_get(config,"MISSIONCONTROLIP"),jnx_hash_get(config,"MISSIONCONTROLPORT"));
}
/*-----------------------------------------------------------------------------
 * Below is the transceiver receiver logic. 
 * A new thread will be spawned for every received query 
 *-----------------------------------------------------------------------------*/
void *transciever_control_endpoint_worker(void *arg)
{
	char *query = (char*)arg;
	api_command_obj *obj = transaction_api_create_obj(query);
	printf("CMD:%d ID:%s DATA:%s SENDER:%s PORT:%d\n",obj->CMD,obj->ID,obj->DATA,obj->SENDER,obj->PORT);



	transaction_api_delete_obj(obj);
}
void transceiver_control_listener_endpoint(char *incoming_query, char *incoming_ip)
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
	jnx_network_setup_listener(atoi(jnx_hash_get(config,"MISSIONCONTROLPORT")),lsc);
}
