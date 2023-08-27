#include "main.h"

/**
 * peek - checks if the first non whitespace character
 * matches any tokens
 * @ps: pointer to string
 * @es: end of string
 * @tokens: string of tokens
 *
 * Return: 1 if matches, 0 otherwise
 */
int peek(char **ps, char *es, char *tokens)
{
	char *s;

	s = *ps;
	while (s < es && strchr(DELIM, *s))
		s++;
	*ps = s;

	return (*s && strchr(tokens, *s));
}

/**
 * gettoken - searchs for tokens
 * @ps: pointer to string
 * @es: end of string
 * @q: pointer to argument string
 *
 * Return: the token found
 */
char gettoken(char **ps, char *es, char **q)
{
	char *s;
	int token;

	s = *ps;
	while (s < es && strchr(DELIM, *s))
		s++;

	if (q)
		*q = s;

	token = *s;

	switch (*s)
	{
		case 0:
			break;

		case '|':
		case '(':
		case ')':
		case ';':
		case '&':
		case '<':
		case '\n':
			s++;
			break;
		case '>':
			if (*s == '>')
			{
				token = '+';
				s++;
			}
			break;
		default:
			token = 'a';
			while (s < es && !strchr(DELIM, *s) && !strchr(SYMBOLS, *s))
				s++;
			break;
	}

	while (s < es && strchr(DELIM, *s))
		s++;
	*ps = s;

	return (token);
}
