#include "main.h"

int runcmd(nsh_info_t *nsh_info, struct cmd *cmd)
{
	int p[2];
	// int builtin_ret = 0;
	struct execcmd *ecmd;
	struct listcmd *lcmd;
	struct pipecmd *pcmd;
	struct redircmd *rcmd;
	struct backcmd *bcmd;
	union sigval value =  { 0 };
	unsigned char buffer[sizeof(*nsh_info)] = { 0 };
	sigset_t set;
	pid_t child_pid;
	int sig;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);

	if (cmd == 0)
		return(1);

	switch (cmd->type)
	{
		case PROGRAM:
			ecmd = (struct execcmd *)cmd;
			nsh_info->argv = ecmd->argv;

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

			execve(ecmd->path_to_file, ecmd->argv, environ);
			print_error(nsh_info, "exec failed");

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
			nsh_info->func_builtin = ecmd->func_builtin;
			serialize_nsh_info(*nsh_info, buffer);
			write(nsh_info->pipe[1], buffer, sizeof(*nsh_info));

			value.sival_int = nsh_info->pipe[0];
			sigprocmask(SIG_BLOCK, &set, NULL);
			sigqueue(nsh_info->main_pid, SIGUSR1, value);

                        sigwait(&set, &sig);

			memset(buffer, 0, sizeof(*nsh_info));

			exit(EXIT_SUCCESS);

		case LIST:
			lcmd = (struct listcmd *)cmd;
			if (forking() == 0)
				runcmd(nsh_info, lcmd->left);
			wait(0);
			runcmd(nsh_info, lcmd->right);
			break;

		case PIPE:
			pcmd = (struct pipecmd *)cmd;
			if (pipe(p) < 0)
			{
				perror("pipe");
				break;
			}

			if (forking() == 0)
			{
				dup2(p[1], STDOUT_FILENO);
				close(p[0]);
				close(p[1]);
				runcmd(nsh_info, pcmd->left);
			}
			else 
			{
				dup2(p[0], STDIN_FILENO);
				close(p[0]);
				close(p[1]);
				runcmd(nsh_info, pcmd->right);
			}

			close(p[0]);
			close(p[1]);

			wait(0);
			wait(0);

			break;

		case REDIR:
			rcmd = (struct redircmd *)cmd;
			close(rcmd->fd);
			if (open(rcmd->file, rcmd->mode) < 0)
			{
				fprintf(stderr, "open %s failed\n", rcmd->file);
				exit(127);
			}
			runcmd(nsh_info, rcmd->cmd);

			break;

		case BACK:
			bcmd = (struct backcmd *)cmd;
			child_pid = forking();
			if (child_pid == 0)
				runcmd(nsh_info, bcmd->cmd);
			
			add_job(nsh_info, child_pid);

			break;

		default:
			perror("runcmd");
			break;
	}

	freexit(nsh_info, EXIT_SUCCESS, 0);

	return (EXIT_FAILURE);
}
