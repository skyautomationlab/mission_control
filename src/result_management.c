/*
 * =====================================================================================
 *
 *       Filename:  result_management.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/08/13 21:48:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <jnxc_headers/jnxhash.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
extern jnx_hashmap *config; 

int result_directory_create()
{
	printf("Creating result directory\n");
	char *directory = jnx_hash_get(config,"RESULTDIR");
	return mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
}
int result_directory()
{
	char *directory = jnx_hash_get(config,"RESULTDIR");
	if(directory)
	{
		struct stat s;
		int err = stat(directory,&s);
		if(err == -1)
		{
			return 0;
		}else
		{
			return 1;
		}
	}else
	{
		return 0;
	}
}
int job_directory_create(char *job_id)
{
	printf("Creating job directory\n");
	char *directory = jnx_hash_get(config,"RESULTDIR");
	char str[256];
	sprintf(str,"%s/%s",directory,job_id);
	return mkdir(str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
}
int job_directory(char *job_id)
{

	char *directory = jnx_hash_get(config,"RESULTDIR");
	if(directory)
	{
		char str[256];
		sprintf(str,"%s/%s",directory,job_id);
		struct stat s;
		int err = stat(str,&s);
		if(err == -1)
		{
			return 0;
		}else
		{
			return 1;
		}
	}else
	{
		return 0;
	}
}
char *result_management_full_path_create(char *jobid, char *filename)
{
	if(!result_directory())
	{
		if(result_directory_create() != 0)
		{
			perror("result_management_full_path_create");
			exit(1);
		}
	}
	if(!job_directory(jobid))
	{
		if(job_directory_create(jobid) != 0)
		{
			perror("result_management_full_path_create");
			exit(1);
		}
	}
	char *buffer = malloc(sizeof(1024));
	char *directory = jnx_hash_get(config,"RESULTDIR");
	sprintf(buffer,"%s/%s/%s",directory,jobid,filename);
	return buffer;
}
