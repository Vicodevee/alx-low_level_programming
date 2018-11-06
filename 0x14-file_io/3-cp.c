#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
int safe_close(int);
void write_error(char *);
void read_error(char *);
/**
 * main - entry point for program to copy files
 * @argc: count of arguments passed
 * @argv: array of char pointers to the arguments
 *
 * Return: 1 on success, exits on failure w/ error code
 */
int main(int argc, char *argv[])
{
	char buff[1024];
	int bytes_read = 0, _EOF = 1, from_fd = -1, to_fd = -1, err = 0;

	if (argc != 3)
	{
		dprintf(STDERR_FILENO, "Usage: cp file_from file_to\n");
		exit(97);
	}
	from_fd = open(argv[1], O_RDONLY);
	if (from_fd < 0) /* file didn't exist */
	{
		read_error(argv[1]);
		exit(98);
	}
	while (_EOF)
	{
		if (bytes_read == 0)
			to_fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0664);
		else if (bytes_read < 2048)
			to_fd = open(argv[2], O_WRONLY | O_APPEND);
		if (to_fd < 0) /* failed to open/create to_file */
		{
			safe_close(from_fd);
			write_error(argv[2]);
			exit(99);
		}
		_EOF = read(from_fd, buff, 1024);
		if (_EOF < 0) /* error reading file */
		{
			safe_close(from_fd);
			safe_close(to_fd);
			read_error(argv[1]);
			exit(98);
		}
		bytes_read += _EOF;
		err = write(to_fd, buff, _EOF);
		if (err < 0) /* failed to write */
		{
			safe_close(from_fd);
			safe_close(to_fd);
			write_error(argv[2]);
			exit(99);
		}

		if (bytes_read <= 1024)
		{
			err = safe_close(to_fd);
			if (err < 0) /* close file failure */
				exit(100);
		}
	}
	if (bytes_read > 1024) /* didn't close it in loop, close now */
	{
		err = safe_close(to_fd);
		if (err < 0) /* close file failure */
			exit(100);
	}
	err = safe_close(from_fd);
	if (err < 0)
		exit(100);
	return (1);
}

/**
 * safe_close - closes a file and exits/prints error if close encounters error
 * @filedescriptor: file descriptor for file to be closed
 *
 * Return: 1 on success, -1 on failure (status code received from close())
 */
int safe_close(int filedescriptor)
{
	int err;

	err = close(filedescriptor);
	if (err < 0)
		dprintf(STDERR_FILENO, "Error: Can't close fd %d\n", filedescriptor);
	return (err);
}

/**
 * read_error - prints a read error message to stderr
 * @filename: filename to print out for errorcode
 *
 * Return: always void
 */
void read_error(char *filename)
{
	dprintf(STDERR_FILENO, "Error: Can't read from file %s\n", filename);
}

/**
 * write_error - prints a write error message to stderr
 * @filename: filename to print out for errorcode
 *
 * Return: always void
 */
void write_error(char *filename)
{
	dprintf(STDERR_FILENO, "Error: Can't write to %s\n", filename);
}