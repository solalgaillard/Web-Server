/* sys.h
Inclure
 */
# include <time.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <stdio.h>
# include <netdb.h>
# include <unistd.h>
# include <ctype.h>
# include <stdlib.h>
# include <signal.h>
# include <string.h>
# include <sys/stat.h>
// # include <assert.h>

char * answer(char * port, int (*fun)(int)); // dans answer.c
