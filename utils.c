#include "main.h"

/**
 * is_interactive - returns true if shell is in interactive mode
 *
 * @nsh_info: main shell struct
 *
 * Return: 1 if in interactive mode, 0 otherwise
 */
int is_interactive(nsh_info_t *nsh_info)
{
	return (isatty(STDIN_FILENO) && nsh_info->read_fd <= 2);
}

/**
 * ignore_comments - replaces the first instance of '#' with '\0'
 *
 * @buf: buffer to modify
 */
void ignore_comments(char *buf)
{
	int i;

	for (i = 0; buf[i] != '\0'; i++)
	{
		if (buf[i] == '#' && (!i || buf[i - 1] == ' '))
		{
			buf[i] = '\0';
			break;
		}
	}
}

int is_whitespace(const char *str)
{
	while (*str != '\0')
	{
		if (!isspace((unsigned char)*str))
			return (0);
		
		str++;
	}

	return (1);
}

int open_file(char *argv[])
{
	int fd = 0;

	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		if (errno == EACCES)
			exit(126);

		if (errno == ENOENT)
		{
			fprintf(stderr, "%s: 0: Can't open %s\n", argv[0], argv[1]);
			exit(127);
		}

		exit(EXIT_FAILURE);
	}

	return (fd);
}

char *join(char **strings, char *separator)
{
	size_t total_length = 0, i;
	char *result = NULL;
	if (strings == NULL || *strings == NULL)
		return NULL;

	for (i = 0; strings[i] != NULL; i++)
		total_length += strlen(strings[i]);

	total_length += (size_t)(i - 1);

	result = calloc(total_length + 1, sizeof(char));
	if (result == NULL)
		return (NULL);

	for (i = 0; strings[i] != NULL; i++)
	{
		strcat(result, strings[i]);
		if (strings[i + 1] != NULL)
			strcat(result, separator);
	}

	return (result);
}
