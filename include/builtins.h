#ifndef _BUILTINS_H_
#define _BUILTINS_H_

#define BUILTIN_ERROR 2

typedef struct {
	char* name;
	int (*fun)(char**); 
} builtin_pair;

extern builtin_pair builtins_table[];

int my_exit(char **argv);
int my_cd(char **argv);
int my_kill(char **argv);
int my_ls(char **argv);

#endif /* !_BUILTINS_H_ */
