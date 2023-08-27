#include "main.h"

/**
 * populate_env_list - populates env linked list
 *
 * @sh_info: main sh_info struct
 */
void populate_env_list(nsh_info_t *nsh_info)
{
	list_t *head = NULL;
	size_t i;

	for (i = 0; environ[i]; i++)
		add_node_end(&head, environ[i], 0);

	nsh_info->env = head;
}
