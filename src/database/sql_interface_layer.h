/*
 * =====================================================================================
 *
 *       Filename:  sql_interface_layer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/27/13 20:49:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  AlexsJones (), alexsimonjones@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __SQL_INTERFACE_LAYER_H__
#define __SQL_INTERFACE_LAYER_H__
#ifdef __APPLE__
#include <mysql.h>
#else 
#include <mysql/mysql.h>
#endif
#define GET_CANDIDATE_JOBS "USE AUTOMATION; call get_candidate_jobs();"
#define SET_MACHINE_STATUS "USE AUTOMATION; update machines set status='%s' where id=%d;" 
#define SET_JOB_STATUS "USE AUTOMATION; call set_job_status_from_id('%s','%s');";
typedef struct mysql_result_bucket
{
	MYSQL_RES **result_array;
	int count;
}mysql_result_bucket;

int sql_setup_credentials(void);
int sql_send_query(mysql_result_bucket **results_bucket, const char *querytemplate, ...);
#endif
