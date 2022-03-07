#include "archive.h"

int input_validation(t_params *params, char **args)
{
	int is_error = 0;
	int func = -1;
	if (strcmp(args[1], "pack") == 0)
		func = PACK;
	else if (strcmp(args[1], "unpack") == 0)
		func = UNPACK;
	if (func != PACK && func != UNPACK)//Если команда не "pack" или "unpack", то
	{
		is_error = 1;
		show_error("Error! Wrong function name.\n");
		show_error("There are only \"pack\" and \"unpack\" functions\n");
	}
	if (func == PACK)
	{
		if ((params->ptr_to_dir = opendir(args[2])) == NULL)
		{
			is_error = 1;
			show_error("Error! Can not open \"");
			show_error(args[2]);
			show_error("\" directory or it doesn't exist\n");
		}
		else
			closedir(params->ptr_to_dir);
	}
	else if (func == UNPACK)
	{
		if ((params->ptr_to_dir = opendir(args[2])) != NULL)
		{
			is_error = 1;
			closedir(params->ptr_to_dir);
			show_error("Error! Can not open \"");
			show_error(args[2]);
			show_error("\" directory or it doesn't exist\n");
		}
	}
	if (func == PACK && access(args[3], F_OK) != -1)//Если файл существует, то
	{
		is_error = 1;
		show_error("Error! File/Archive with name \"");
		show_error(args[3]);
		show_error("\" already exists\n");
	}
	else if (func == UNPACK && access(args[3], F_OK == -1))
	{
		is_error = 1;
		show_error("Error! File/Archive with name \"");
		show_error(args[3]);
		show_error("\" don't exist\n");
	}
	if (is_error)
		return 1;
	params->func = func;
	params->archive_name = strdup(args[3]);
	if (params->archive_name == NULL)
		exit_fatal();
	params->dir_to_exec = strdup(args[2]);
	if (params->dir_to_exec == NULL)
		exit_fatal();
	return 0;
}

int main(int argc, char **argv, char **env)
{
	(void)env;
//	int fd = open("file.txt", O_RDONLY);
//	int length = lseek(fd, 0, SEEK_END);
//	printf("length = %d\n", length);
	if (argc != 4)
	{
		printf("Wrong input.\n");
		printf("usage: ./archiver <name_of_func> <directory_name> <archive_name>\n");
		return 1;
	}
	t_params params;
	if (input_validation(&params, argv))
		return 1;
	if (params.func == PACK)
	{
		if (pack(&params))
			printf("Packing finished successfully\n");
		else
			printf("Packing failed\n");
	}
	else if (params.func == UNPACK)
	{
		if (unpack(&params))
			printf("Unpacking finished successfully");
		else
			printf("Unpacking failed\n");
	}
	clear_params(&params);
	return 0;
}