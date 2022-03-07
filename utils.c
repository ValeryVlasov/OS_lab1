#include "archive.h"

void show_error(char *msg) {
	write(2, msg, strlen(msg));
}

void free_ptr(void *ptr)
{
	if (!ptr)
		return ;
	free(ptr);
	ptr = NULL;
}

void clear_params(t_params *params)
{
//	closedir(params->ptr_to_dir);
	free_ptr(params->archive_name);
	free_ptr(params->dir_to_exec);
	params->func = 0;
}

void exit_fatal(void)
{
	show_error("error: fatal\n");
	exit(EXIT_FAILURE);
}