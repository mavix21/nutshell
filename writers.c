#include "main.h"

/**
 * putchar_to_fd - writes a character to the given file descriptor
 *
 * @c: character to write
 * @fd: file descriptor to write to
 *
 * Return: Always 1
 */
int putchar_to_fd(char c, int fd)
{
	static int i;
	static char buf[WRITE_BUFSIZE];

	if (c == BUF_FLUSH || i >= WRITE_BUFSIZE)
	{
		write(fd, buf, i);
		i = 0;
	}

	if (c != BUF_FLUSH)
	{
		buf[i++] = c;
	}

	return (1);
}

/**
 * puts_to_fd - prints a string to a file descriptor
 *
 * @str: string to be printed
 * @fd: file descriptor to write to
 *
 * Return: number of chars written
 */
int puts_to_fd(char *str, int fd)
{
	int i = 0;

	if (!str)
		return (0);

	while (*str)
	{
		i += putchar_to_fd(*str++, fd);
	}

	return (i);
}

