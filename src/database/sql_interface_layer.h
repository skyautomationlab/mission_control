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
#include "results_bucket.h"
#include <jnxc_headers/jnxhash.h>
#define GET_CANDIDATE_JOBS "USE AUTOMATION; call get_candidate_jobs();"
#define SET_MACHINE_STATUS "USE AUTOMATION; update jobs set status='%s' where id=%s;" 
#define GET_MACHINE_FROM_ID "USE AUTOMATION; select * from machines where id=%d;"
#define SET_JOB_STATUS "USE AUTOMATION; call set_job_status_from_id('%s','%s');"
extern jnx_hashmap *config;
int sql_setup_credentials(void);
int sql_send_query(mysql_result_bucket **results_bucket, const char *querytemplate, ...);
#endif
