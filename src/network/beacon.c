/*
 * =====================================================================================
 *
 *       Filename:  beacon.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/12/13 11:21:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <jnxc_headers/jnxstring.h>
#include <jnxc_headers/jnxnetwork.h>
#include <jnxc_headers/jnxhash.h>
#include "transaction_api.h"
#include "transceiver_control.h"
#include "../database/sql_interface_layer.h"
#include "beacon.h"
#define BPORT 12345
#define BGROUP "225.0.0.37"
#define WAIT_PERIOD 30
#ifdef __APPLE__
#define INTERFACE "en0"
#else
#define INTERFACE "eth0"
#endif
extern jnx_hashmap *config;
void beacon_send(void)
{
	char query[1024];
	sprintf(query,API_COMMAND,"ALIVE","","","",(char*)jnx_hash_get(config,"MISSIONCONTROLIP"),(char*)jnx_hash_get(config,"MISSIONCONTROLPORT"));

	jnx_network_send_broadcast(BPORT,BGROUP,query);
}
void *beacon_loop(void*ar)
{
	while(1)
	{
		sleep(WAIT_PERIOD);
		//set machines offline for challenge
		mysql_result_bucket *res_bucket = NULL;
		sql_send_query(&res_bucket,SET_ALL_MACHINES_OFFLINE);
		beacon_send();
	}
}
void beacon_pulse(void)
{
	pthread_t beacon_thread;
	pthread_create(&beacon_thread,NULL,beacon_loop,NULL);
}
void beacon_message_intercept(char *msg)
{
}
void *beacon_receive(void*ar)
{
	jnx_network_broadcast_listener_callback c = beacon_message_intercept;
	jnx_network_broadcast_listener(BPORT,BGROUP,c);	
}
void beacon_listen(void)
{
	pthread_t beacon_listen_thread;
	pthread_create(&beacon_listen_thread,NULL,beacon_receive,NULL);
}
