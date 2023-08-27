#include "main.h"

void nsh_loop(nsh_info_t *nsh_info)
{
	int status = 1;

	do
	{
		clear_nsh_info(nsh_info);

		// Print the prompt
		if (is_interactive(nsh_info))
			write(1, "[nsh]$ ", 7);

		// Read the line
		nsh_info->line = nsh_readline(nsh_info);
		if (is_whitespace(nsh_info->line))
		{
			safe_free(&(nsh_info->line));

			if (is_interactive(nsh_info))
				continue;
			else
				exit(EXIT_SUCCESS);
		}

		// execute
		status = nsh_execute(nsh_info);

		safe_free(&(nsh_info->start_of_line));
		free_tree(nsh_info->cmd_tree);

	} while (status && is_interactive(nsh_info));
}
