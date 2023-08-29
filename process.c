#include "main.h"
#include <unistd.h>

/**
 * forking - forks the current process
 *
 * Return: the corresponding process pid
 */
pid_t forking(void)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid < -1)
	{
		perror("fork");
		exit(1);
	}

	return (child_pid);
}

/**
 * handle_sigint - handles the SIGINT signal
 *
 * @sig_num: signal to be handle (SIGINT in this case)
 */
void handle_sigint(int sig_num)
{
	(void) sig_num;
	signal(SIGINT, &handle_sigint);
	write(1, "\n$ ", 3);
}

void replace_stdio(int *p, int fd)
{
	dup2(p[fd], fd);
	close(p[0]);
	close(p[1]);
}
