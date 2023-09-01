#include "main.h"

builtin_t nsh_bins[] = {
	{"exit", nsh_exit},
	{"cd", nsh_cd},
	{"history", nsh_history},
	{"env", nsh_env},
	{"jobs", nsh_jobs},
	{NULL, NULL}
};

int (*builtin_func)(nsh_info_t *nsh_info) = NULL;
volatile pid_t caller_pid = -1;

void handle_sigusr(int signo, siginfo_t *info, void *context)
{
	int i;
	char *builtin_name = (char *)info->si_ptr;
	printf("Received signal from process with PID: %d\n", info->si_pid);
	printf("Builtin: %s\n", builtin_name);
	for (i = 0; nsh_bins[i].builtin != NULL; i++)
	{
		if (strcmp(nsh_bins[i].builtin, builtin_name) == 0)
		{
			builtin_func = nsh_bins[i].f;
			caller_pid = info->si_pid;
			break;
		}

	}
}

int nsh_execute(nsh_info_t *nsh_info)
{
	struct sigaction sa;
	struct cmd *cmd;
	pid_t child_pid, wpid;
	int status;
	int p[2];
	unsigned char buffer[sizeof(*nsh_info)];

	cmd = parsecmd(nsh_info);
	set_program_or_builtin(cmd);
	nsh_info->cmd_tree = cmd;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handle_sigusr;
	sigaction(SIGUSR1, &sa, NULL);

	pipe(p);
	child_pid = forking();
	if (child_pid == 0)
	{
		close(p[0]);
		nsh_info->pipe = p;
		nsh_info->main_pid = getppid();
		return (runcmd(nsh_info, cmd));
	}
	else do
	{
		if (builtin_func != NULL && caller_pid > 0)
		{
			read(p[0], buffer, sizeof(*nsh_info));
			deserialize_nsh_info(buffer, nsh_info);
			builtin_func(nsh_info);
			builtin_func = NULL;
			kill(caller_pid, SIGUSR2);
			caller_pid = -1;
		}
		wpid = waitpid(child_pid, &status, 0);
	} while (wpid <= 0 || !WIFEXITED(status));

	close(p[1]);

	return (EXIT_SUCCESS);
}

void set_program_or_builtin(struct cmd *cmd)
{
	struct execcmd *ecmd;
	struct listcmd *lcmd;
	struct pipecmd *pcmd;
	struct redircmd *rcmd;
	char *command = NULL;

	if (cmd == 0)
		return;

	switch(cmd->type)
	{
		case EXEC:
			ecmd = (struct execcmd*)cmd;
			command = ecmd->argv[0];
			if (is_builtin(command))
			{
				ecmd->type = BUILTIN;
				ecmd->func_builtin = is_builtin(command);
			}
			else
			{
				ecmd->type = PROGRAM;
				ecmd->path_to_file = cmdfinder(ecmd->argv[0]);
			}
			return;

		case REDIR:
			rcmd = (struct redircmd*)cmd;
			set_program_or_builtin(rcmd->cmd);
			break;

		case LIST:
			lcmd = (struct listcmd *)cmd;
			set_program_or_builtin(lcmd->left);
			set_program_or_builtin(lcmd->right);
			break;

		case PIPE:
			pcmd = (struct pipecmd *)cmd;
			set_program_or_builtin(pcmd->left);
			set_program_or_builtin(pcmd->right);
			break;
	}
}

/**
 * is_builtin - Checks if the command is a nutshell builtin
 *
 * @command: command to be analysed
 *
 * Return: pointer to the corresponding function, NULL otherwise
 */
int (*is_builtin(char *command))(nsh_info_t *)
{
	int i;

	if (command == NULL)
		return (NULL);

	for (i = 0; nsh_bins[i].builtin != NULL; i++)
	{
		if (strcmp(command, nsh_bins[i].builtin) == 0)
			return (nsh_bins[i].f);
	}

	return (NULL);
}

char *cmdfinder(char *command)
{
	char paths[512];
	char *path_to_file = NULL, *dummy = NULL, *dir = NULL;
	char *path = getenv("PATH");

	if (access(command, F_OK) != -1)
		return (strdup(command));

	if (path == NULL)
		return (NULL);

	dummy = strdup(path);
	dir = strtok(dummy, ":");
	while (dir != NULL)
	{
		snprintf(paths, sizeof(paths), "%s/%s", dir, command);
		if (access(paths, F_OK) != -1)
		{
			path_to_file = strdup(paths);
			safe_free(&dummy);
			return (path_to_file);
		}
		memset(paths, 0, sizeof(paths));
		dir = strtok(NULL, ":");
	}
	safe_free(&dummy);

	return (NULL);
}
