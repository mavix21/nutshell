#include "main.h"

builtin_t nsh_bins[] = {
	{"exit", nsh_exit},
	{"cd", nsh_cd},
	{"history", nsh_history},
	{"env", nsh_env},
	{"jobs", nsh_jobs},
	{NULL, NULL}
};

/**
 * handle_builtin: Handles signal SIGUSR1 sent from a child process. Child may
 * send a SIGUSR1 when it encounters a builtin node, along with the necessary
 * data for the parent to be able to execute the builtin function.
 *
 * @signo: the signal received, in this case, SIGUSR1
 * @info: pointer to structure containing further information about the signal 
 * @context: pointer to structure containing signal context information.
 */
void handle_builtin(int signo, siginfo_t *info, void *context)
{
	nsh_info_t nsh_info;
	int fd = info->si_int; // file descriptor the child has written to
	unsigned char buffer[sizeof(nsh_info_t)] = { 0 };

	// read the data the child have written to `fd`
	read(fd, buffer, sizeof(nsh_info));

	// deserialize the bytes into an actual structure of type nsh_info_t
	deserialize_nsh_info(buffer, &nsh_info);

	// parent executes the builtin function sent from the child
	nsh_info.func_builtin(&nsh_info);

	// clean buffer
	memset(buffer, 0, sizeof(nsh_info_t));

	// notifies child that execution is over
	kill(info->si_pid, SIGUSR2);
}

int nsh_execute(nsh_info_t *nsh_info)
{
	struct sigaction sa;
	struct cmd *cmd;
	sigset_t ss;
	pid_t child_pid, wpid;
	int wait_status, run_status = 1, pipe_status;
	int p[2];

	cmd = parsecmd(nsh_info);
	set_program_or_builtin(cmd);
	nsh_info->cmd_tree = cmd;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handle_builtin;
	sigemptyset(&ss);
	sa.sa_mask = ss;
	sigaction(SIGUSR1, &sa, NULL);

	pipe_status = pipe(p);
	if (pipe_status < 0)
		return (2);

	nsh_info->pipe = p;

	child_pid = forking();
	if (child_pid == 0)
	{
		close(p[0]);
		nsh_info->main_pid = getppid();
		run_status =  runcmd(nsh_info, cmd);
	}
	else do
	{
		wpid = waitpid(child_pid, &wait_status, 0);
	} while (wpid <= 0 || !WIFEXITED(wait_status));

	close(p[1]);

	return (run_status);
}

void set_program_or_builtin(struct cmd *cmd)
{
	struct execcmd *ecmd;
	struct listcmd *lcmd;
	struct pipecmd *pcmd;
	struct redircmd *rcmd;
	struct backcmd *bcmd;
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

		case BACK:
			bcmd = (struct backcmd *)cmd;
			set_program_or_builtin(bcmd->cmd);

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
