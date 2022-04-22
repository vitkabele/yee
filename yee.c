#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

static void
print_usage(void) {
	fprintf(stderr,
			"Usage: yee [-e] [-i FD] cmd [arg1 [arg2 [...]]]\n"
			"\tYee duplicates the file descriptor given as parmeter -i\n"
			"\tto descriptor 0 (if not 0), 1 (if not 1) and 2 (if -e is given).\n"
			"\tThen executes command cmd and passes all following args to it\n"
			"Return value:\n"
			"\tThis utility returns 1 if invoked mistakenly, 2 if cloning fd fails\n"
			"\tin which case error is printed on stderr. Otherwise return value should\n"
			"\tbe interpreted as errno set by execvp function. In such case error is printed\n"
			"\tonly if stderr is not redirected (with -e)\n");
}

static int
clone(int what, int where) {
	int err;

	if (what != where) {
		err = dup2(what, where);
		if (err == -1) {
			fprintf(stderr, "Error: dup2(%d, %d): %s\n",
					what, where, strerror(errno));
			return -1;
		}
	}

	return 0;
}

int
main(int argc, char **argv) {

	int ch, source_fd = 0, clone_stderr = 0, err;

	while ((ch = getopt(argc, argv, "ei:")) != -1) {
		switch (ch) {
		case 'e':
			clone_stderr = 1;
			break;
		case 'i':
			source_fd = atoi(optarg);
			break;
		default:
			print_usage();
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0) {
		print_usage();
		return 1;
	}
	
	/*
	 * Clone the source fd to stdin
	 */
	err = clone(source_fd, 0);
	if (err) {
		return 2;	
	}

	/*
	 * Clone the source fd to stdout
	 */
	err = clone(source_fd, 1);
	if (err) {
		return 2;	
	}

	/*
	 * Clone the source fd to stderr if required
	 */
	if (clone_stderr) {
		err = clone(source_fd, 2);
		if (err) {
			return 2;	
		}
	}
	
	(void)execvp(argv[0], argv);
	
	/*
	 * Executable not in path etc...
	 */
	if (!clone_stderr) {
		/* The stderr might not be available */
		fprintf(stderr, "Error: execvp(%s, ...) returned with error code %d\n",
			argv[0], err);
	}
	return errno;
}
