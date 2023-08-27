#include "main.h"

builtin_t nsh_bins[] = {
	{"exit", nsh_exit},
	{"cd", nsh_cd},
	{"history", nsh_history},
	{"env", nsh_env},
	{"jobs", nsh_jobs},
	{NULL, NULL}
};

int nsh_execute(nsh_info_t *nsh_info)
{
	// char *es, *anchor, *dummy;
	struct cmd *cmd;

	cmd = parsecmd(nsh_info);
	set_program_or_builtin(cmd);
	nsh_info->cmd_tree = cmd;

	return (runcmd(nsh_info, cmd));
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
		return (command);

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
