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
void terminal_print(mysql_result_bucket *jobbucket)
{
	system("clear");
	printf("\n");
	printf("=========================================================================\n");
	printf("%-20s\t%-20s\t\t%-20s\n", "JOB ID","STATUS","NEXT RUN TIME");
	int y;
	for(y = 0; y < jobbucket->row_count; ++y)
	{
	printf("%-20s\t%-20s\t\t%-20s", jobbucket->rows[y][get_mysql_result_bucket_field_position(&jobbucket,"id")], jobbucket->rows[y][get_mysql_result_bucket_field_position(&jobbucket,"status")], jobbucket->rows[y][get_mysql_result_bucket_field_position(&jobbucket,"TIMESTAMP")]);
	printf("\n");
	}
	printf("=========================================================================\n");
	printf("\n");
}
