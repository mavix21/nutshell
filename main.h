#ifndef H_NUTSHELL
#define H_NUTSHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <asm-generic/errno-base.h>

#define BUILTIN 0
#define PROGRAM 1
#define EXEC 2
#define REDIR 3
#define PIPE 4
#define LIST 5
#define BACK 6

#define MAXARGS 20
#define MAXJOBS 120
#define BUFSIZE 120
#define READ_BUFSIZE 1024
#define WRITE_BUFSIZE 1024
#define BUF_FLUSH -1

#define AT_INDEX 0
#define AT_VALUE 1

#define WHITESPACE " \t\r\n\a\v\f"
#define DELIM " \t\r\a\v\f"
#define SYMBOLS "<|>&;()\n"
#define NULTERMINATE " \t\r\a\v\f\n<|>&;()"

#define UNUSED(x) (void)(x)

#define INIT_NSH_INFO \
{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, NULL, NULL, NULL, NULL,\
	NULL, NULL, NULL, 0, 1, 1, 0, 0, 0, 0, 0, 0}

extern char **environ;

/**
 * struct liststr - singly linked list
 *
 * @id: unique identifier
 * @num: number field
 * @str: string filed
 * @next: pointer to next node
 */
typedef struct liststr
{
	int id;
	int num;
	char *str;
	struct liststr *next;
} list_t;

typedef struct jobs
{
	list_t base;
	pid_t pid;
} jobs_t;

/**
 * struct nsh_info - shell info main struct that contains all the info to be
 * passed to functions, allowing uniform prototyping for them
 *
 * @line: pointer that runs along the command line
 * @start_of_line: pointer that saves the beginning of the command line
 * @end_of_line: pointer that saves the end of the command line
 * @argv: array of strings generated from @line
 * @path: string path for the current command
 * @cmd_tree: pointer to cmd tree
 * @argc: argument count
 * @line_count: error line count
 * @err_num: error status code
 * @filename: program filename
 * @env: linked list generated form environ
 * @environ: custom copy of environ
 * @history: history list
 * @alias: alias list
 * @env_changed: on if environ was changed
 * @status: return status of the last executed command
 * @
 */
typedef struct nsh_info
{
	char *line;
	char *start_of_line;
	char *end_of_line;
	char **argv;
	char *path;
	char *filename;
	char **environ;
	char syntax_err_token[3];
	struct cmd *cmd_tree;
	int (*func_builtin)(struct nsh_info *);
	list_t *env;
	list_t *history;
	list_t *alias;
	list_t *jobs;
	int *pipe;
	int argc;
	unsigned int line_count;
	unsigned int syntax_err_line;
	int main_pid;
	int err_num;
	int env_changed;
	int status;
	int read_fd;
	int histcount;

} nsh_info_t;

typedef struct builtins
{
	char *builtin;
	int (*f)(nsh_info_t *);
} builtin_t;

/**
 * struct cmd - command node
 * @type: command type (EXEC, LIST, etc)
 *
 * Description: command node that stores the command type
 */
struct cmd
{
	int type;
};

/**
 * struct execcmd - exec command node
 * @type: EXEC
 * @line_number: line where the command was found in (when reading from a file)
 * @status: status of the executed command
 * @run_in_bg: 1 if commnad has to be executed in the background, 0 otherwise
 * @fd_to_write_to: file descriptor to write to (used for pipes)
 * @fd_to_read_from: file descriptor to read from (used for pipes)
 * @func_builtin: pointer to the builtin function
 * @argv: array of arguments
 * @path_to_file: absolute path to the program to be run (if not a builtin)
 *
 * Description: command node that stores the EXEC command type and the array of
 * strings containing the arguments to be executed
 */
struct execcmd
{
	int type;
	int line_number;
	int status;
	int run_in_bg;
	int *fd_to_write_to;
	int *fd_to_read_from;
	int (*func_builtin)(nsh_info_t *);
	char *argv[MAXARGS];
	char *path_to_file;
};

/**
 * struct listcmd - list command node
 * @type: LIST
 * @left: exec command node
 * @right: exec command node
 *
 * Description: command node that stores the LIST command type and the pointers
 * to the command trees that will be executed one after the other
 */
struct listcmd
{
	int type;
	struct cmd *left;
	struct cmd *right;
};

struct redircmd
{
	int type;
	struct cmd *cmd;
	char *file;
	int mode;
	int fd;
};

struct pipecmd
{
	int type;
	struct cmd *left;
	struct cmd *right;
};

struct backcmd
{
	int type;
	struct cmd *cmd;
};

int runcmd(nsh_info_t *nsh_info, struct cmd *cmd);

// loop
void nsh_loop(nsh_info_t *nsh_info);

// execute
int nsh_execute(nsh_info_t *nsh_info);
int (*is_builtin(char *command))(nsh_info_t *);
void set_program_or_builtin(struct cmd *cmd);
char *cmdfinder(char *command);

// readers
char *nsh_readline(nsh_info_t *nsh_info);
ssize_t _getline(nsh_info_t *nsh_info, char **lineptr, size_t *n);
char **nsh_splitline(char *ps);

// finders
int peek(char **ps, char *es, char *tokens);
char gettoken(char **ps, char *es, char **q);

// process
pid_t forking(void);
void handle_sigint(int sig_num);
void replace_stdio(int *p, int fd);

// nsh_info
void clear_nsh_info(nsh_info_t *nsh_info);
void set_nsh_info(nsh_info_t *nsh_info, char **av);

// free
void safe_free(char **ps);
void free_tree(struct cmd *cmd);
void freexit(nsh_info_t *nsh_info, int status_code, int print_exit);
void free_node(list_t **node);

// writers
int putchar_to_fd(char c, int fd);
int puts_to_fd(char *str, int fd);

// errors
int err_putchar(char c);
void err_puts(char *str);
int err_atoi(char *s);
void print_error(nsh_info_t *nsh_info, char *err_str);
void print_syntax_error(nsh_info_t *nsh_info);

// lists
list_t *set_node(const char *str, int history_index);
list_t *add_node(list_t **head, const char *str, int history_index);
list_t *add_node_end(list_t **head, const char *str, int history_index);
size_t print_list_str(const list_t *node);
list_t *get_node_at_index(list_t *head, unsigned int index);
list_t *get_node_at_value(list_t *head, int value);
int delete_node_at_index(list_t **head, unsigned int index);
int delete_node_at_value(list_t **head, int value);
size_t list_length(list_t *node);
int get_last_value(list_t *head);
size_t get_index_at_value(list_t *head, int value);

// utils
int is_interactive(nsh_info_t *nsh_info);
int reallocate_line(char **lineptr, size_t pos, size_t *n);
int is_whitespace(const char *str);
int open_file(char *argv[]);
char *join(char **strings, char *separator);

// builtins
int nsh_exit(nsh_info_t *nsh_info);
int nsh_cd(nsh_info_t *nsh_info);
int nsh_env(nsh_info_t *nsh_info);
int nsh_history(nsh_info_t *nsh_info);
int nsh_jobs(nsh_info_t *nsh_info);

// jobs
int add_job(nsh_info_t *nsh_info, pid_t pid);

// parsers
struct cmd *parsecmd(nsh_info_t *nsh_info);
struct cmd *parseline(nsh_info_t *nsh_info);
struct cmd *parseexec(nsh_info_t *nsh_info);
struct cmd *parsepipe(nsh_info_t *nsh_info);
struct cmd *parseblock(nsh_info_t *nsh_info);
struct cmd *parseredirs(nsh_info_t *nsh_info, struct cmd *cmd);

// constructors
struct cmd *execcmd();
struct cmd *listcmd(struct cmd *left, struct cmd *right);
struct cmd *redircmd(struct cmd *subcmd, char *file, int mode, int fd);
struct cmd *backcmd(struct cmd *subcmd);
struct cmd *pipecmd(struct cmd *left, struct cmd *right);

// serialize
void serialize_nsh_info(nsh_info_t data, unsigned char *buffer);
void deserialize_nsh_info(unsigned char *buffer, nsh_info_t *data);

#endif
