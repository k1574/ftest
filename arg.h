#ifndef ARG_H__
#define ARG_H__

/*
 * Copy me if you can.
 * by 20h
 */

/* Modified by "jien". */

/* Simplest argument parsing.
	It parses options until meet first
	non option argument or "--" argument.
	Options can have 2 looks:
	1) -[option][argument]
	2) -[option] [argument]
	No -[option]=[argument] parsing, it is easy to add
	by yourself though. 
	Suckless team, thanks you stopped my headache
	about parsing arguments. */

extern char *argv0;

/* Use with  "main(int argc, char *argv[])". */
#define ARGBEGIN \
	for( argv0 = *argv, argv++, argc--;\
			argv[0] && argv[0][0] == '-'\
			&& argv[0][1];\
			argc--, argv++ ){\
		char argc_;\
		char **argv_;\
		int brk_;\
		if( argv[0][1] == '-' && argv[0][2] == '\0' ){ /* Stop if meet "--". */ \
			argv++;\
			argc--;\
			break;\
		}\
		for( brk_ = 0, argv[0]++, argv_ = argv;\
				argv[0][0] && !brk_;\
				argv[0]++ ) {\
			if (argv_ != argv)/* Go to next option if we got argument. */\
				break;\
			argc_ = argv[0][0]; /* Option that we will switch. */\
			switch (argc_)\


/* To handle num options. */ /* The next word argument. */
#define ARGNUM		case '0':\
			case '1':\
			case '2':\
			case '3':\
			case '4':\
			case '5':\
			case '6':\
			case '7':\
			case '8':\
			case '9'


#define ARGEND		}\
		}

#define ARGC()		(argc_) /* Current option. */

/* Mandatory argument. */
#define EARGF(x)\
	((argv[0][1] == '\0' && argv[1] == NULL)?\
		((x), abort(), (char *)0) : /* Exit if no any argument. */\
		(brk_ = 1, (argv[0][1] != '\0')?\
			(&argv[0][1]) /* Concatenated argument. */ :\
			(argc--, argv++, argv[0]))) /* The next word argument. */

/* Optional argument. */
#define ARGF()\
	((argv[0][1] == '\0' && argv[1] == NULL)?\
		(char *)0 : /* Return "zero-pointer" if no any argument. */ \
		(brk_ = 1, (argv[0][1] != '\0')?\
			(&argv[0][1]) : /* Concatenated argument. */ \
			(argc--, argv++, argv[0]))) /* The next word argument. */

#endif /* ARG_H__ */
