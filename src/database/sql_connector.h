#ifndef __SQL_CONNECTOR_H__
#define __SQL_CONNECTOR_H__
#ifdef __APPLE__
#include <mysql.h>
#else 
#include <mysql/mysql.h>
#endif
#include <jnxc_headers/jnxterm.h>
#include "sql_interface_layer.h"
typedef void (*sql_callback)(MYSQL_RES*);

void perform_store_sql_credentials(char* host_addr, char* user, char* pass);
int jnx_sql_query(char *query,mysql_result_bucket **result_bucket);
#endif
