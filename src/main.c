/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/27/13 20:14:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  AlexsJones (), alexsimonjones@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <jnxc_headers/jnxfile.h>
#include <jnxc_headers/jnxhash.h>
#include "job_scheduler.h"
jnx_hashmap *config = NULL;
void usage()
{
	printf("Please provide -c [configuration]\n");
	exit(0);
}
int parse_conf_file(char *path, jnx_hashmap **config)
{
	jnx_file_kvp_node *head = jnx_file_read_keyvaluepairs(path,"=");
	if(!head) { return 1; }
	(*config) = jnx_hash_init(1024);
	if(!(*config)) { return 1; }
	while(head)
	{
		jnx_hash_put((*config),head->key,head->value);
		printf("Inserted %s %s into configuration\n",head->key,head->value);
		free(head);	
		head = head->next;
	}
	return 0;
}
int main(int argc,char **argv)
{
	int c;
	char *configuration = NULL;
	while((c = getopt(argc, argv,"c:")) != -1)
	{
		switch(c)
		{
			case 'c':
				configuration = optarg;
				break;
			case '?':
				usage();
				break;
		}
	}
	if(!configuration)
	{
		usage();
	}
	if(parse_conf_file(configuration,&config) != 0)
	{
		usage();
	}

	job_scheduler_start();

	return 0;
}
