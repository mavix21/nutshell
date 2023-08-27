#include "main.h"

/**
 * main - entry point
 *
 * @argc: arguments counter
 * @argv: arguments vector
 *
 * Return: always EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
	nsh_info_t nsh_info[] = { INIT_NSH_INFO };

	if (argc == 2)
		nsh_info->read_fd = open_file(argv); 

	set_nsh_info(nsh_info, argv);
	nsh_loop(nsh_info);

	return (nsh_info->status);
}
