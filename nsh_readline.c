#include "main.h"
#include <unistd.h>

/**
 * wish_readline - reads the user's input
 * @nsh_info: shell info main struct
 *
 * Return: line read
 */
char *nsh_readline(nsh_info_t *nsh_info)
{
	ssize_t linelen = 0;
	size_t bufsize = 0;
	char *line = NULL;
	int i;

	linelen = _getline(nsh_info, &line, &bufsize);

	if (linelen <= 0)
	{
		safe_free(&line);

		if (linelen == 0 && is_interactive(nsh_info)) // EOF
		{
			printf("\nexit\n");
			exit(EXIT_SUCCESS);
		}
		else if (linelen < 0)
		{
			perror("_getline");
			exit(EXIT_FAILURE);
		}
	}

	for (i = linelen - 1; i >= 0 && strchr("\n", line[i]); i--)
	{
		line[i] = '\0';
	}

	return (line);
}

/**
 * _getline - reads user's input from file descriptor and stores
 * the address of the buffer containing the text into *lineptr
 * @lineptr: pointer to string
 * @n: pointer to number of bytes allocated for *lineptr
 *
 * Return: number of characters read. -1 on failure to read a line
 * (including EOF condition).
 */
ssize_t _getline(nsh_info_t *nsh_info, char **lineptr, size_t *n)
{
	char buffer[BUFSIZE];
	size_t pos = 0;
	ssize_t nread = 0;

	if (*lineptr != NULL)
		safe_free(lineptr);

	*n = BUFSIZE;
	*lineptr = calloc(*n, sizeof(char));
	if (*lineptr == NULL)
		return (-1);

	while ((nread = read(nsh_info->read_fd, buffer, BUFSIZE)) > 0)
	{
		pos += nread;
		if (pos == BUFSIZE || pos >= 2 * BUFSIZE)
		{
			if (reallocate_line(lineptr, pos, n))
				return (-1);
		}
		*lineptr = strncat(*lineptr, buffer, nread);
		if (buffer[nread - 1] == '\n')
			break;
	}

	if (nread < 0)
	{
		safe_free(lineptr);
		return (-1);
	}
	(*lineptr)[pos] = '\0';

	return (pos);
}

/**
 * reallocate_line - reallocates memory for storing the line, updating
 * *lineptr and *n as necessary
 * @lineptr: pointer to string
 * @n: pointer to number of bytes allocated for *lineptr
 * @pos: number of characters read so far
 *
 * Return: 0 on success, 1 if realloc fails
 */
int reallocate_line(char **lineptr, size_t pos, size_t *n)
{
	char *newptr = NULL;

	if (pos == BUFSIZE)
		*n *= 2;
	else if (pos == 2 * BUFSIZE)
		*n += 1;
	else if (pos > 2 * BUFSIZE)
		*n += (pos - (2 * BUFSIZE));

	newptr = realloc(*lineptr, *n);
	if (newptr == NULL)
	{
		safe_free(lineptr);
		return (1);
	}

	*lineptr = newptr;

	return (0);
}
