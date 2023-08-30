#include "main.h"

/**
 * @execcmd - allocates memory for a structure of type struct execcmd
 *
 * Return: pointer to structure of type struct cmd
 */
struct cmd *execcmd()
{
	struct execcmd *cmd;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->fd_to_write_to = NULL;
	cmd->fd_to_read_from = NULL;

	return ((struct cmd *)cmd);
}

/**
 * listcmd - allocates memory for a structure of type struct listcmd
 * @left: pointer to a command tree which be executed first
 * @right: pointer to a command tree which be executed second
 *
 * Return: pointer to structure of type struct cmd
 */
struct cmd *listcmd(struct cmd *left, struct cmd *right)
{
	struct listcmd *cmd;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = LIST;
	cmd->left = left;
	cmd->right = right;

	return ((struct cmd *)cmd);
}

struct cmd *pipecmd(struct cmd *left, struct cmd *right)
{
	struct pipecmd *cmd;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = PIPE;
	cmd->left = left;
	cmd->right = right;

	return (struct cmd *)cmd;
}

struct cmd *redircmd(struct cmd *subcmd, char *file, int mode, int fd)
{
	struct redircmd *cmd;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = REDIR;
	cmd->cmd = subcmd;
	cmd->file = file;
	cmd->mode = mode;
	cmd->fd = fd;

	return (struct cmd *)cmd;
}

struct cmd *backcmd(struct cmd *subcmd)
{
	struct backcmd *cmd;
	struct execcmd *ecmd;

	ecmd = (struct execcmd *)subcmd;
	ecmd->run_in_bg = 1;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = BACK;
	cmd->cmd = subcmd;

	return (struct cmd*)cmd;
}
