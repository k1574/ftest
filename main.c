/* See LICENSE file for copyright and license details. */
#include <sys/stat.h>

#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arg.h"
char *argv0;

#define FLAG(x)  (flag[(x)-'a'])

static void test(const char *path);
static void usage(void);

static int match = 0;
static int flag[26];
static struct stat old, new;

static unsigned int
ishidden(const char *path)
{
	int i = strnlen(path, BUFSIZ) - 1 ;
	while( i && path[i]!='/' ) --i ;
	if(path[i]=='/') ++i ;
	if(path[i]=='.') return 1 ;
	return 0 ;
}

static void
test(const char *path)
{
	struct stat st, ln;
	if( (!stat(path, &st) /* It also checks if file exists. */
			&& (!FLAG('a') || ishidden(path))/* Hidden files. */
			&& (!FLAG('A') || !ishidden(path)) /* Visible files. */
			&& (!FLAG('b') || S_ISBLK(st.st_mode)) /* Block special. */
			&& (!FLAG('c') || S_ISCHR(st.st_mode)) /* character special. */
			&& (!FLAG('d') || S_ISDIR(st.st_mode)) /* Directory. */
			&& (!FLAG('e') || access(path, F_OK) == 0) /* Exists. */
			&& (!FLAG('f') || S_ISREG(st.st_mode)) /* Regular file. */
			&& (!FLAG('g') || st.st_mode & S_ISGID) /* Set-group-id flag. */
			&& (!FLAG('h') || (!lstat(path, &ln) && S_ISLNK(ln.st_mode))) /* Symbolic link. */
			&& (!FLAG('n') || st.st_mtime > new.st_mtime) /* Newer than file. */
			&& (!FLAG('o') || st.st_mtime < old.st_mtime) /* Older than file.   */
			&& (!FLAG('p') || S_ISFIFO(st.st_mode)) /* Named pipe. */
			&& (!FLAG('r') || access(path, R_OK) == 0) /* Readable. */
			&& (!FLAG('s') || st.st_size > 0) /* Not empty. */
			&& (!FLAG('u') || st.st_mode & S_ISUID) /* Set-user-id flag. */
			&& (!FLAG('w') || access(path, W_OK) == 0) /* Writable. */
			&& (!FLAG('x') || access(path, X_OK) == 0)) != FLAG('v')) { /* Executable. */
		if (FLAG('q')) /* Quit on first correct file. */
			exit(0);
		match = 1 ;
		puts(path);
	}
}

static void
usage(void)
{
	fprintf(stderr, "Usage: %s [-aAbcdefghlpqrsuvwx] "
	        "[-n file] [-o file] [file...]\n", argv0);
	exit(2); /* Like test(1) return > 1 on error. */
}

void
main(int argc, char *argv[])
{
	struct dirent *d;
	char path[PATH_MAX], *line = NULL, *file;
	size_t linesiz = 0;
	ssize_t n;
	DIR *dir;
	int r;

	argv0 = argv[0] ;
	if(argc==1)
		usage();

	ARGBEGIN {
	case 'n': /* Newer than file. */
	case 'o': /* Older than file. */
		file = EARGF(usage());
		if (!(FLAG(ARGC()) = !stat(file, (ARGC() == 'n' ? &new : &old))))
			perror(file);
		break;
	default:
		/* Miscellaneous operators. */
		if (strchr("aAbcdefghlpqrsuvwx", ARGC()))
			FLAG(ARGC()) = 1;
		else
			usage(); /* Unknown flag. */
	} ARGEND ;

	if (!argc) {
		/* Read list from the standard input. */
		while ((n = getline(&line, &linesiz, stdin)) > 0) {
			if (n && line[n - 1] == '\n')
				line[n - 1] = '\0';
			test(line);
		}
		free(line);
	} else {
		for (; argc; argc--, argv++) {
			if (FLAG('l') && (dir = opendir(*argv))) {
				/* Test directory contents. */
				while ((d = readdir(dir))) {
					r = snprintf(path, sizeof path, "%s/%s",
					             *argv, d->d_name);
					if (r >= 0 && (size_t)r < sizeof path)
						test(path);
				}
				closedir(dir);
			} else {
				test(*argv);
			}
		}
	}
	exit(match ? 0 : 1);
}
