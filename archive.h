#ifndef ARCHIVE_H
#define ARCHIVE_H

#define PACK 1
#define UNPACK 2
#define BUFF_SIZE 1024
/**************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
//#include <sys/dir.h>

#include <fcntl.h>
#include <sys/errno.h>

//#include <filesystem>//c++
#include <sys/types.h>//chmod(const char *path, int mode)
#include <sys/stat.h>//fchmod(int fd, int mode)
//#include <stdint.h>
#include <limits.h>
//#include <termio.h>
/**************************************/

typedef struct	s_params
{
	int func;
	DIR *ptr_to_dir;
	char *dir_to_exec;
	char *archive_name;
}				t_params;

//utils.c
void show_error(char *msg);
void free_ptr(void *ptr);
void clear_params(t_params *params);
void exit_fatal(void);

//archive.c
int pack(t_params *params);
int unpack(t_params *params);

#endif