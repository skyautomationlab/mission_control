/*
 * =====================================================================================
 *
 *       Filename:  terminal.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/20/13 11:00:01
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
#include "terminal.h"
#include <time.h>
long check_time(time_t candidate_time)
{
	time_t current_time = time(0);
	long difference = candidate_time - current_time;
	return difference;
}
void terminal_print(mysql_result_bucket *jobbucket)
{
	system("clear");
	printf("\n");
	printf("=====================================================================================================================\n");
	printf("%-20s\t%-20s\t\t%-20s\t%-20s\t%-5s\n", "JOB ID","STATUS","NEXT RUN TIME","MACHINE ID", "TRIGGER DIFF");
	int y;
	for(y = 0; y < jobbucket->row_count; ++y)
	{
	printf("%-20s\t%-20s\t\t%-20s\t%-20s\t%-5d", jobbucket->rows[y][get_mysql_result_bucket_field_position(&jobbucket,"id")], jobbucket->rows[y][get_mysql_result_bucket_field_position(&jobbucket,"status")], jobbucket->rows[y][get_mysql_result_bucket_field_position(&jobbucket,"TIMESTAMP")],jobbucket->rows[y][get_mysql_result_bucket_field_position(&jobbucket,"machine_id")],check_time(atoi(jobbucket->rows[y][get_mysql_result_bucket_field_position(&jobbucket,"trigger_time")])));
	printf("\n");
	}
	printf("====================================================================================================================\n");
	printf("\n");
}
