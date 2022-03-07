#include "archive.h"

int pack_file(char *file_name, int arch_fd, int depth)
{
	struct stat statbuf;
	int size_file_name = sizeof(file_name);
	int type = 1;
	long long size_file = statbuf.st_size;
	int file_int;

	if ((file_int = open(file_name, O_RDONLY)) == -1) {
		show_error("Error. Input file couldn't open or doesn't exist\n");
		return 1;
	}

	lstat(file_name, &statbuf);

	if (write(arch_fd, &size_file_name, sizeof(int)) == -1
			|| write(arch_fd, file_name, size_file_name) == -1
			|| write(arch_fd, &type, sizeof(int)) == -1
			|| write(arch_fd, &depth, sizeof(int)) == -1
			|| write(arch_fd, &size_file, sizeof(int)) == -1)
	{
		close(file_int);
		return 1;
	}

	char block[BUFF_SIZE];
	ssize_t nread = read(file_int, block, 1);

	while (nread > 0) {
		write(arch_fd, block, nread);
		nread = read(file_int, block, 1);
	}

//	printf("Успех! Завершена запись файла \"%s\"\n", file_name);
	close(file_int);
	return 0;
}

int pack_dir(char *dir_name, int arch_fd, int depth)
{
	int size_dir_name = sizeof(dir_name);
	int type = 0;

	for (int it = 0; it < depth; it++)
		printf("\t");
	printf("%d--\n", size_dir_name);
	if (write(arch_fd, &size_dir_name, sizeof(int)) == -1
			|| write(arch_fd, dir_name, size_dir_name) == -1
			|| write(arch_fd, &type, sizeof(int)) == -1
			|| write(arch_fd, &depth, sizeof(int)) == -1)
		return 1;

//	printf("Успех! Запись информации о директории \"%s\" закончена\n", dir_name);
	return 0;
}

int packing(char *dir_name, int arch_fd, int depth)
{
	struct stat statbuf;
	struct dirent *entry;

	DIR* folder = opendir(dir_name);

	if (folder == NULL) {
		show_error("Error. Coultn't open folder \"");
		show_error(dir_name);
		show_error("\"\n");
		return 1;
	}

	if (chdir(dir_name)) {
		show_error("Error. Couldn't change directory\n");
		return 1;
	}

	entry = readdir(folder);
	while (entry != NULL)
	{
		if (strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0)
		{

			for (int it = 0; it < depth; it++)
				printf("  ");
			printf("%s", entry->d_name);
			lstat(entry->d_name, &statbuf);
			printf("\t%lld\n", statbuf.st_size);
			if (S_ISDIR(statbuf.st_mode))
			{
				pack_dir(entry->d_name, arch_fd, depth);
				packing(entry->d_name, arch_fd, depth+1);
//				printf("Успех! Запись информации о директории \"%s\" закончена\n", dir_name);
				printf("\n**********************************\n\n");
			}
			else
				pack_file(entry->d_name, arch_fd, depth);
		}
		entry = readdir(folder);
	}

	if (chdir("..")) {
		show_error("Error. Couldn't change directory\n");
		return 1;
	}
	closedir(folder);
	return 0;//Success
}

int pack(t_params *params)
{
	int archive;

	archive = open(params->archive_name, O_CREAT | O_APPEND | O_WRONLY);

	fchmod(archive, S_IROTH | S_IWOTH | S_IXOTH | S_IRGRP | S_IWGRP | S_IXGRP | S_IRUSR | S_IWUSR | S_IXUSR);

	if (archive == -1) {
		show_error("Error. Can't create archive\n");
		return 0;
	}

	if (packing(params->dir_to_exec, archive, 0))
	{
		show_error("Packing files into archive failed\n");
		close(archive);
		return 0;
	}
	close(archive);
	return 1;//Success
}

int unpack(t_params *params) {
	int fa = open(params->archive_name, O_RDONLY);

	if (fa == -1) {
		show_error("Error. Couldn't open file \"");
		show_error(params->archive_name);
		show_error("\"\n");
		return 0;
	}

	printf("File opened successfully: %s\n", params->archive_name);
	lseek(fa, 0, SEEK_CUR);

	DIR* directory = opendir(params->dir_to_exec);

	if (directory == NULL)
	{
		mkdir(params->dir_to_exec, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
		directory = opendir(params->dir_to_exec);
		if (directory == NULL) {
			show_error("Error. Couldn't create new directory with name \"");
			show_error(params->dir_to_exec);
			show_error("\"\n");
			close(fa);
			return 0;
		}
		else
			printf("Запись будет осуществлена в новую директорию: %s\n", params->dir_to_exec);
	}

	if (chdir(params->dir_to_exec) < 0) {
		show_error("Error. Couldn't move to directory \"");
		show_error(params->dir_to_exec);
		show_error("\"\n");
		close(fa);
		closedir(directory);
		return 0;
	}

	printf("Directory has been opened successfully: %s\n", params->dir_to_exec);

	int size, size_name, type, fnew, depth, depth_current = 0;
	char block[BUFF_SIZE];
	char* dir_current, *fname;
	while (read(fa, &size_name, sizeof(int)) > 0)
	{
		printf("size(size_name) = %d, size(int) = %lu\n", size_name, sizeof(int));
		fname = malloc(size_name+1);

		if (read(fa, fname, size_name) < 0) {
			show_error("Error. Couldn't read file name\n");
			close(fa);
			return 0;
		}

		if (read(fa, &type, sizeof(int)) < 0) {
			show_error("Error. Couldn't read checking variable");
			close(fa);
			return 0;
		}
		if (type == 0)
		{
			printf("Creating directory: %s\n", fname);

			read(fa, &depth, sizeof(int));

			if (depth_current < depth)
				chdir(dir_current);
			else if (depth_current > depth)
				chdir("..");

			depth_current = depth;
			fnew = mkdir(fname, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);

			if(fnew == -1) {
				show_error("Error. Couldn't create new directory with name \"");
				show_error(fname);
				show_error("\"\n");
				close(fa);
				return 0;
			}
			else
				dir_current = fname;
		}
		if (type == 1)
		{
			printf("Creating file: %s", fname);
			if (read(fa, &depth, sizeof(int)) > 0)
				printf(", depth = %d\n", depth);
			else
				printf("\n");

			if (depth_current < depth)
				chdir(dir_current);
			else if (depth_current > depth)
				chdir("..");

			depth_current = depth;
			read(fa, &size, sizeof(int));

			if (read(fa, block, size)> 0)
				printf("\n");

			fnew = open(fname, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IWOTH);

			if (write(fnew, block, size) > 0) {
				printf("Запись данных прошла успешно\n");
			}
		}
	}
	close(fa);
	return 1;
}