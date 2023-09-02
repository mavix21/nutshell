#ifndef NSH_CONSTANTS_H_

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

#define WHITESPACE " \t\r\n\a\v\f"
#define DELIM " \t\r\a\v\f"
#define SYMBOLS "<|>&;()\n"
#define NULTERMINATE " \t\r\a\v\f\n<|>&;()"

#define UNUSED(x) (void)(x)

#define INIT_NSH_INFO \
{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, NULL, NULL, NULL, NULL,\
	NULL, NULL, NULL, 0, 1, 1, 0, 0, 0, 0, 0, 0}

#endif
