#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()#"

// run condition
#define NONE 0
#define AND 1
#define OR 2

/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */
int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) {
		*s++ = 0;
	}
	if (*s == 0) {
		return 0;
	}

	if (strchr(SYMBOLS, *s)) {
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		return t;
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		s++;
	}
	*p2 = s;
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

#define MAXARGS 128

int parsecmd(char **argv, int *rightpipe) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int c = gettoken(0, &t);
		switch (c) {
			case '#':
			case 0: {
				return argc;
			}
			case 'w': {
				if (argc >= MAXARGS) {
					debugf("too many arguments\n");
					exit();
				}
				argv[argc++] = t;
				break;
			}
			case '<': {
				if (gettoken(0, &t) != 'w') {
					debugf("syntax error: < not followed by word\n");
					exit();
				}
				fd = open(t, O_RDONLY);
				if (fd < 0) {
					debugf("open %s failed!\n", t);
					exit();
				}
				dup(fd, 0);
				close(fd);
				break;
			}
			case '>': {
				if (gettoken(0, &t) != 'w') {
					debugf("syntax error: > not followed by word\n");
					exit();
				}
				fd = open(t, O_WRONLY);
				if (fd < 0) {
					debugf("open %s failed!\n", t);
					exit();
				}
				dup(fd, 1);
				close(fd);
				break;
			}
			case '|': {
				int p[2];
				pipe(p);
				*rightpipe = fork();
				if (*rightpipe == 0) {
					dup(p[0], 0);
					close(p[0]);
					close(p[1]);
					return parsecmd(argv, rightpipe);
				} else if (*rightpipe > 0) {
					dup(p[1], 1);
					close(p[1]);
					close(p[0]);
					return argc;
				}
				break;
			}
		}
	}
	return argc;
}

int runcmd(char *s) {
	gettoken(s, 0);

	char *argv[MAXARGS];
	int rightpipe = 0;
	int argc = parsecmd(argv, &rightpipe);
	int exit_status = -1;
	if (argc == 0) {
		return 0;
	}
	argv[argc] = 0;
	int len = strlen(argv[0]);
	char temp[10] = {0};
	if (len >= 2 && (argv[0][len - 2] != '.' || argv[0][len - 1] != 'b')) {
		strcpy(temp, argv[0]);
		temp[len] = '.';
		temp[len + 1] = 'b';
		temp[len + 2] = 0;
		argv[0] = temp;
	}
	for (int i = 0; i < argc; i++) {
		debugf("arg %d: %s\n", i, argv[i]);
	}
	int child = spawn(argv[0], argv);
	close_all();
	if (child >= 0) {
		exit_status = ipc_recv(0, 0, 0);
		debugf("\033[1;34menv %08x recieved return value %d\n\033[0m", env->env_id, exit_status);
		wait(child);
	} else {
		debugf("spawn %s: %d\n", argv[0], child);
	}
	if (rightpipe) {
		wait(rightpipe);
	}
	debugf("\033[1;35menv %d finished running %s\n\033[0m", child, argv[0]);
	return exit_status;
}

void conditionally_run(char *s) {
    char buf[1024];
    int pos = 0;
    int return_value = 0;
    int previous_op = NONE;
	int r;

    while (*s) {
        if ((*s == '&' && *(s + 1) == '&') || 
			(*s == '|' && *(s + 1) == '|')) {
            // AND or OR
			char temp = *s;
            s += 2;
            buf[pos] = '\0';
            if (previous_op == NONE || (previous_op == AND && return_value == 0) || (previous_op == OR && return_value != 0)) {
				if ((r = fork()) < 0) {
					user_panic("fork: %d", r);
				}
				if (r == 0) {
					return_value = runcmd(buf);
					syscall_ipc_try_send(env->env_parent_id, return_value, 0, 0);
					exit();
				} else {
					return_value = ipc_recv(0, 0, 0);
					wait(r);
				}
            }
			previous_op = (temp == '&') ? AND : OR;
            pos = 0;
        } else {
            buf[pos++] = *s++;
            buf[pos] = '\0';
        }
    }

    if (pos > 0) {
        buf[pos] = '\0';
        if (previous_op == NONE || (previous_op == AND && return_value == 0) || (previous_op == OR && return_value != 0)) {
				if ((r = fork()) < 0) {
					user_panic("fork: %d", r);
				}
				if (r == 0) {
					return_value = runcmd(buf);
					syscall_ipc_try_send(env->env_parent_id, return_value, 0, 0);
					exit();
				} else {
					return_value = ipc_recv(0, 0, 0);
					wait(r);
				}
        }
    }
}

void readline(char *buf, u_int n) {
	int r;
	for (int i = 0; i < n; i++) {
		if ((r = read(0, buf + i, 1)) != 1) {
			if (r < 0) {
				debugf("read error: %d\n", r);
			}
			exit();
		}
		if (buf[i] == '\b' || buf[i] == 0x7f) {
			if (i > 0) {
				i -= 2;
			} else {
				i = -1;
			}
			if (buf[i] != '\b') {
				printf("\b");
			}
		}
		if (buf[i] == '\r' || buf[i] == '\n') {
			buf[i] = 0;
			return;
		}
	}
	debugf("line too long\n");
	while ((r = read(0, buf, 1)) == 1 && buf[0] != '\r' && buf[0] != '\n') {
		;
	}
	buf[0] = 0;
}

char buf[1024];

void usage(void) {
	printf("usage: sh [-ix] [script-file]\n");
	exit();
}

int main(int argc, char **argv) {
	int r;
	int interactive = iscons(0);
	int echocmds = 0;
	printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	printf("::                                                         ::\n");
	printf("::                     MOS Shell 2024                      ::\n");
	printf("::                                                         ::\n");
	printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	ARGBEGIN {
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
	ARGEND

	if (argc > 1) {
		usage();
	}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[0], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[0], r);
		}
		user_assert(r == 0);
	}
	for (;;) {
		if (interactive) {
			printf("\n$ ");
		}
		readline(buf, sizeof buf);

		if (buf[0] == '#') {
			continue;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}
		conditionally_run(buf);
		// if ((r = fork()) < 0) {
		// 	user_panic("fork: %d", r);
		// }
		// if (r == 0) {
		// 	conditionally_run(buf);
		// 	exit();
		// } else {
		// 	wait(r);
		// }
	}
	return 0;
}
