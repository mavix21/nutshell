#include "main.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int runcmd(nsh_info_t *nsh_info, struct cmd *cmd)
{
	int p[2];
	pid_t child_pid;
	int builtin_ret = 0;
	struct execcmd *ecmd;
	struct listcmd *lcmd;
	struct pipecmd *pcmd;
	// struct redircmd *rcmd;

	if (cmd == 0)
		return(1);

	switch (cmd->type)
	{
		case PROGRAM:
			ecmd = (struct execcmd *)cmd;
			nsh_info->argv = ecmd->argv;

			child_pid = forking();
			if (child_pid == 0)
			{
				if ((ecmd->line_number == nsh_info->syntax_err_line) && \
						(nsh_info->syntax_err_token[0] != '\0'))
				{
					print_syntax_error(nsh_info);
					freexit(nsh_info, 2, 0);
				}

				if (ecmd->path_to_file == NULL)
				{
					print_error(nsh_info, "command not found");
					freexit(nsh_info, 127, 0);
				}

				if (ecmd->write_to_fd)
				{
					dup2(ecmd->write_to_fd[1], STDOUT_FILENO);
					close(ecmd->write_to_fd[0]);
					close(ecmd->write_to_fd[1]);
				}
				else if (ecmd->read_from_fd)
				{
					dup2(ecmd->read_from_fd[0], STDIN_FILENO);
					close(ecmd->read_from_fd[0]);
					close(ecmd->read_from_fd[1]);
				}

				execve(ecmd->path_to_file, ecmd->argv, environ);
				print_error(nsh_info, "exec failed");
				exit(EXIT_FAILURE);
			}

			if (ecmd->run_in_bg && ecmd->path_to_file)
				add_job(nsh_info, child_pid);
			else
			{
				if (ecmd->read_from_fd)
					close(ecmd->read_from_fd[1]);

				waitpid(child_pid, NULL, 0);
			}

			break;

		case BUILTIN:
			ecmd = (struct execcmd *)cmd;

			if ((ecmd->line_number == nsh_info->syntax_err_line) && \
					(nsh_info->syntax_err_token[0] != '\0'))
			{
				print_syntax_error(nsh_info);
				break;
			}

			nsh_info->argv = ecmd->argv;

			if (ecmd->run_in_bg && forking() == 0)
				ecmd->func_builtin(nsh_info);
			else
				builtin_ret = ecmd->func_builtin(nsh_info);

			if (builtin_ret < 0)
				nsh_info->status = 1;

			break;

		case LIST:
			lcmd = (struct listcmd *)cmd;
			runcmd(nsh_info, lcmd->left);
			runcmd(nsh_info, lcmd->right);
			break;

		case PIPE:
			pcmd = (struct pipecmd *)cmd;
			if (pipe(p) < 0)
			{
				perror("pipe");
				break;
			}

			((struct execcmd *)pcmd->left)->write_to_fd = p;
			((struct execcmd *)pcmd->right)->read_from_fd = p;
			runcmd(nsh_info, pcmd->left);
			runcmd(nsh_info, pcmd->right);

			close(p[0]);
			close(p[1]);
			break;

		default:
			// perror("runcmd");
			break;
	}

	return (1);
}
