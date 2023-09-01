#include "main.h"

/**
 * sh_exit - exit builtin
 * @nsh_info: shell info struct
 *
 * Return: always EXIT_SUCCESS
 */
int nsh_exit(nsh_info_t *nsh_info)
{
	int exit_status;
	char *exit_status_str = nsh_info->argv[1];

	if (exit_status_str == NULL)
		freexit(nsh_info, 0, 1);

	exit_status = err_atoi(exit_status_str); 
	if (exit_status == -1)
	{
		print_error(nsh_info, "numeric argument required");
		freexit(nsh_info, 2, 1);
	}

	if (nsh_info->argv[2] != NULL)
	{
		print_error(nsh_info, "too many arguments");
		return (-1);
	}

	freexit(nsh_info, exit_status, 1);

	return (0);
}

/**
 * sh_cd - cd (change directory) builtin
 * @nsh_info: shell info struct
 *
 * Return: always 0
 */
int nsh_cd(nsh_info_t *nsh_info)
{
	char *cwd, *opwd, *home_dir, *destination;
	char buffer[1024];
	int chdir_status;

	destination = nsh_info->argv[1];
	cwd = getcwd(buffer, 1024);
	if (cwd == NULL)
	{
		perror("getcwd");
	}

	if (destination == NULL)
	{
		home_dir = getenv("HOME");
		chdir_status = chdir(home_dir);
	}
	else if (strcmp(destination, "-") == 0)
	{
		opwd = getenv("OLDPWD");
		if (opwd == NULL)
		{
			printf("%s\n", cwd);
			return (1);
		}
		printf("%s\n", opwd);
		chdir_status = chdir(opwd);
	}
	else
	{
		chdir_status = chdir(destination);
	}

	if (chdir_status == -1)
	{
		print_error(nsh_info, "can't cd to ");
		fprintf(stderr, "%s\n", destination);
	}
	else
	{
		setenv("OLDPWD", getenv("PWD"), 1);
		setenv("PWD", cwd, 1);
	}

	return (0);
}

int nsh_history(nsh_info_t *nsh_info)
{
	print_list_str(nsh_info->history);

	return (0);
}

int nsh_env(nsh_info_t *nsh_info)
{
	print_list_str(nsh_info->env);

	return (0);
}

int nsh_jobs(nsh_info_t *nsh_info)
{
	list_t *head = NULL, *temp = NULL;
	int status, i = 0, j;
	char *state = "nil";
	int jobs_id_to_be_deleted[MAXJOBS] = { 0 };
	pid_t wreturn;

	head = nsh_info->jobs;
	if (head == NULL)
		return (0);

	temp = head;
	while (temp != NULL)
	{
		wreturn = waitpid(temp->id, &status, WNOHANG|WUNTRACED|WCONTINUED) == -1;
		if (wreturn == -1)
		{
			temp = temp->next;
			continue;
		}

		if (wreturn == 0)
		{
			state = "Running";
		}
		else 
		{
			if (WIFEXITED(status))
			{
				state = "Done   ";
				jobs_id_to_be_deleted[i++] = temp->num;
			}
		}

		printf("[%d] %d %s\t%s\n", temp->num, temp->id, state, temp->str);

		temp = temp->next;
	}

	for (j = 0; j < i; j++)
		delete_node_at_value(&head, jobs_id_to_be_deleted[j]);

	nsh_info->jobs = head;

	return (0);
}
