/*
 * =====================================================================================
 *
 *       Filename:  tranceiver_control.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/01/13 09:05:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  AlexsJones (), alexsimonjones@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __TRANSCEIVER_CONTROL_H__
#define __TRANSCEIVER_CONTROL_H__
#include <jnxc_headers/jnxnetwork.h>
int transceiver_control_start_dialogue(char *machine_ip,char *machine_port,char *job_id, char *job_command);
#endif
