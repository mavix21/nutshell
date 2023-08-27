#include "main.h"

/**
 * clear_nsh_info - resets some shell info struct fields 
 *
 * @nsh_info: shell info main struct
 */
void clear_nsh_info(nsh_info_t *nsh_info)
{
	nsh_info->line = NULL;
	nsh_info->end_of_line = NULL;
	nsh_info->argv = NULL;
	nsh_info->path = NULL;
	nsh_info->argc = 0;
	nsh_info->err_num = 0;
	memset(nsh_info->syntax_err_token, 0, 3);
}

void set_nsh_info(nsh_info_t *nsh_info, char **av)
{
	nsh_info->filename = strrchr(av[0], '/');
	if (nsh_info->filename != NULL)
		nsh_info->filename += 1;
	else
		nsh_info->filename = av[0];
}
