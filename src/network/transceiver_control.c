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
#include "transceiver_control.h"
#include "../database/sql_interface_layer.h"


/*-----------------------------------------------------------------------------
 *  The following defines are API calls agreed at both mission_control and satellite
 *-----------------------------------------------------------------------------*/
int transceiver_control_start_dialogue(char *machine_ip,char *machine_port,char *job_id, char *job_command)
{
	printf("transceiver control starting dialogue with new job %s to be run on %s\n",job_id,machine_ip);
	return 0;
}
