#include "main.h"

/**
 * parsecmd - builds the command tree by calling the corresponding
 * parser functions
 * @line: user's input string
 *
 * Return: pointer to command tree
 */
struct cmd *parsecmd(nsh_info_t *nsh_info)
{
	struct cmd *cmd;
	char *dummy, *s;

	nsh_info->start_of_line = nsh_info->line;
	s = nsh_info->line;
	while (*s != '\0' && strchr(WHITESPACE, *s))
	{
		if (*s == '\n')
			nsh_info->line_count++;
		s++;
	}

	nsh_info->line = s;
	nsh_info->end_of_line = s + strlen(s);
	cmd = parseline(nsh_info);

	dummy = strtok(nsh_info->start_of_line, NULTERMINATE);
	while (dummy != NULL)
		dummy = strtok(NULL, NULTERMINATE);

	return (cmd);
}

/**
 * parseline - builds a node of type struct listcmd
 * @nsh_info: main shell info struct
 * @ps: pointer to string
 * @es: end of string
 *
 * Return: pointer to command tree
 */
struct cmd *parseline(nsh_info_t *nsh_info)
{
	struct cmd *cmd;
	struct execcmd *ecmd;
	char **ps = &(nsh_info->line);
	char tok;
	int i = 0;

	cmd = parsepipe(nsh_info);
	while (peek(ps, nsh_info->end_of_line, "&"))
	{
		gettoken(ps, nsh_info->end_of_line, 0);
		ecmd = (struct execcmd *)cmd;
		ecmd->run_in_bg = 1;
	}

	while (peek(ps, nsh_info->end_of_line, ";\n"))
	{
		tok = gettoken(ps, nsh_info->end_of_line, 0);
		if (*ps == nsh_info->end_of_line)
			return (cmd);

		if (tok == '\n')
			nsh_info->line_count++;

		if (strchr("&|);", **ps))
		{
			if (tok == ';')
				nsh_info->syntax_err_token[i++] = ';';

			nsh_info->syntax_err_token[i++] = **ps;
			nsh_info->syntax_err_line = nsh_info->line_count;
			if (gettoken(ps, nsh_info->end_of_line, 0) == 0)
				return (cmd);

			if (strchr("&|);", **ps) && tok != ';')
				nsh_info->syntax_err_token[i++] = **ps;

			print_syntax_error(nsh_info);
			*ps = nsh_info->end_of_line;
			return (cmd);
		}
		else if (**ps == '\n')
			continue;

		cmd = listcmd(cmd, parseline(nsh_info));
	}

	return (cmd);
}

struct cmd *parsepipe(nsh_info_t *nsh_info)
{
	struct cmd *cmd;

	char **ps = &(nsh_info->line);
	cmd = parseexec(nsh_info);
	if (peek(ps, nsh_info->end_of_line, "|"))
	{
		gettoken(ps, nsh_info->end_of_line, 0);
		cmd = pipecmd(cmd, parsepipe(nsh_info));
	}

	return (cmd);
}

struct cmd *parseexec(nsh_info_t *nsh_info)
{
	struct execcmd *cmd;
	struct cmd *ret;
	int tok, argc;
	char *q;
	char **ps = &(nsh_info->line);

	if (peek(ps, nsh_info->end_of_line, "("))
		return parseblock(nsh_info);

	ret = execcmd();
	cmd = (struct execcmd *)ret;

	argc = 0;
	ret = parseredirs(nsh_info, ret);

	while (!peek(ps, nsh_info->end_of_line, "|)&;\n"))
	{
		tok = gettoken(ps, nsh_info->end_of_line, &q);
		if (tok == 0)
			break;

		cmd->argv[argc] = q;
		argc++;
		if (argc >= MAXARGS)
		{
			// TODO
		}
		ret = parseredirs(nsh_info, ret);
	}
	cmd->argv[argc] = 0;
	cmd->line_number = nsh_info->line_count;
	cmd->type = EXEC;

	return (ret);
}

struct cmd *parseblock(nsh_info_t *nsh_info)
{
	struct cmd *cmd;
	char **ps = &(nsh_info->line);

	gettoken(ps, nsh_info->end_of_line, 0);
	cmd = parseline(nsh_info);
	if (!peek(ps, nsh_info->end_of_line, ")"))
	{
		// TODO (error message)
		return (cmd);
	}

	gettoken(ps, nsh_info->end_of_line, 0);
	cmd = parseredirs(nsh_info, cmd);

	return (cmd);
}

struct cmd *parseredirs(nsh_info_t *nsh_info, struct cmd *cmd)
{
	int tok;
	char *q;
	char **ps = &(nsh_info->line);

	while (peek(ps, nsh_info->end_of_line, "<>"))
	{
		tok = gettoken(ps, nsh_info->end_of_line, 0);
		if (gettoken(ps, nsh_info->end_of_line, &q) != 'a')
		{
			// TODO (error message)
			return (cmd);
		}

		switch (tok)
		{
			case '<':
				cmd = redircmd(cmd, q, O_RDONLY, 0);
				break;
			case '>':
				cmd = redircmd(cmd, q, O_WRONLY|O_CREAT, 1);
				break;
			case '+': // >>
				cmd = redircmd(cmd, q, O_WRONLY|O_CREAT, 1);
				break;
		}
	}

	return (cmd);
}
