#include "main.h"
#include <stdio.h>

/**
 * err_putchar - writes a character to stderr
 *
 * @c: character to write
 *
 * Return: Always 1
 */
int err_putchar(char c)
{
	return (putchar_to_fd(c, 2));
}

/**
 * err_puts - writes a string to stderr
 *
 * @str: string to write
 *
 * Return: nothing
 */
void err_puts(char *str)
{
	int i = 0;

	if (!str)
		return;

	while (str[i] != '\0')
	{
		err_putchar(str[i]);
		i++;
	}
}

/**
 * err_atoi- converts a string representing the exit status code to an integer
 *
 * @str: string to be converted
 *
 * Return: exit status code as integer, -1 on error
 */
int err_atoi(char *s)
{
	int i = 0;
	unsigned long int exit_status = 0;

	if (*s == '+')
		s++;

	for (i = 0; s[i] != '\0'; i++)
	{
		if (s[i] >= '0' && s[i] <= '9')
		{
			exit_status *= 10;
			exit_status += (s[i] - '0');
			if (exit_status > INT_MAX)
				return (-1);
		}
		else 
		{
			return (-1);
		}
	}

	return (exit_status);
}

/**
 * print_error - prints and error message
 *
 * @nsh_info: shell info main struct
 * @err_str: string error message
 */
void print_error(nsh_info_t *nsh_info, char *err_str)
{
	char *command = nsh_info->argv[0];
	
	fprintf(stderr, "%s: ", nsh_info->filename);

	if (!is_interactive(nsh_info))
		fprintf(stderr, "line %d: ", nsh_info->line_count);

	fprintf(stderr, "%s: %s\n", command, err_str);
}

/**
 * print_syntax_error - prints and error message related to a syntax error
 *
 * @nsh_info: shell info main struct
 */
void print_syntax_error(nsh_info_t *nsh_info)
{
	char *err_tok = nsh_info->syntax_err_token;

	fprintf(stderr, "%s: ", nsh_info->filename);

	if (!is_interactive(nsh_info))
		fprintf(stderr, "line %d: ", nsh_info->line_count);

	fprintf(stderr, "syntax error near unexpected token `%s\n", err_tok);
}
