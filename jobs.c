#include "main.h"

/**
 * add_job - adds a job node to the singly linked list of jobs
 *
 * @nsh_info: nsh info main struct
 * @pid: process id of the job to be added
 *
 * Return: 0 on success, -1 otherwise
 */
int add_job(nsh_info_t *nsh_info, pid_t pid)
{
	size_t last_id;
	list_t *jobs = NULL, *new_job = NULL;
	char *process = NULL;

	jobs = nsh_info->jobs;
	last_id = get_last_value(jobs);
	printf("[%ld] %ld\n", ++last_id, (long) pid);

	process = join(nsh_info->argv, " ");
	new_job = add_node_end(&jobs, process, last_id);

	new_job->id = pid;
	nsh_info->jobs = jobs;

	free(process);

	return (0);
}
