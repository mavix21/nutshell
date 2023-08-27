#include "main.h"

/**
 * safe_free - frees the given pointer and sets it to NULL
 *
 * @ps: pointer to a string
 */
void safe_free(char **ps)
{
	if (ps && *ps)
	{
		free(*ps);
		*ps = NULL;
	}
}

void free_tree(struct cmd *cmd)
{
	struct execcmd *ecmd;
	struct listcmd *lcmd;

	if (cmd == NULL)
		return;

	switch (cmd->type)
	{
		case PROGRAM:
		case BUILTIN:
			ecmd = (struct execcmd *)cmd;
			safe_free(&(ecmd->path_to_file));
			free(ecmd);
			break;

		case LIST:
			lcmd = (struct listcmd *)cmd;
			if (lcmd->left != NULL)
				free_tree(lcmd->left);
			if (lcmd->right != NULL)
				free_tree(lcmd->right);
			free(lcmd);
			break;
	}
}

/**
 * freexit - free all resources and exit with a specified status code
 *
 * @nsh_info: nsh info main struct
 * @status_code: exit status code
 * @print_exit: if true, print `exit` to stdout
 *
 */
void freexit(nsh_info_t *nsh_info, int status_code, int print_exit)
{
	free_tree(nsh_info->cmd_tree);
	safe_free(&(nsh_info->start_of_line));
	if (print_exit)
		printf("exit\n");

	exit(status_code);
}

void free_node(list_t **ptr)
{
	list_t *node = *ptr;

	if (ptr != NULL && *ptr != NULL)
	{
		if (node->str != NULL)
		{
			free(node->str);
			node->str = NULL;
		}
		free(*ptr);
		*ptr = NULL;
	}
}
