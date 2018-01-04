/* $Id: tstavl.c,v 1.8 2014/01/22 08:01:21 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Thu Aug 13 19:38:00     2009
 *
 * Disclaimer:
 *  (C) 2009 LUIS COLORADO SISTEMAS S.L.
 *  All rights reserved.  No part of this code can be distributed in
 *  any form, withour previous written consent of the copyright owner.
 *  This code is distributed WITHOUT ANY WARRANTY, even the implied
 *  WARRANTY OF MERCHANTABILITY or FITNESS FOR ANY PARTICULAR PURPOSE.
 *  In case of an agreed previous, written contract, the WARRANTIES
 *  applied will be the ones written in the applicable contract.
 */

#define IN_TSTAVL_C

/* Standard include files */
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "stravl.h"

#ifndef HAS_MALLOC_STATS
#define HAS_MALLOC_STATS 0
#endif

/* variables */
static char TSTAVL_CPP_RCSId[]="\n$Id: tstavl.c,v 1.8 2014/01/22 08:01:21 luis Exp $\n";

#define FLAG_USEPAGER (1 << 0)
static int flags = 0;
static char *pager = "less";

void help()
{
	printf(
		"tstavl: prueba de las funciones de test de árboles AVL "
		"(treemaps)\n"
		"(c) 2012 LUIS COLORADO SISTEMAS S.L.U.\n"
		"All rights reseved.\n"
		"Ayuda:\n"
		"+id   añade <id,null> a la tabla.\n"
		"-id   elimina <id,null> de la tabla. Si no existe no hace nada.\n"
		"?id   consulta si <id,...> existe en la tabla.\n"
		"*     realiza un listado por orden de la tabla.\n"
		"/     realiza un listado por orden inverso.\n"
		"<id   realiza una búsqueda obteniendo la mayor clave menor que id.\n"
		"<=id  realiza una búsqueda obteniendo la mayor clave menor o igual que id.\n"
		"=id   realiza una búsqueda obteniendo la clave id.\n"
		">=id  realiza una búsqueda obteniendo la menor clave mayor o igual que id.\n"
		">id   realiza una búsqueda obteniendo la menor clave mayor que id.\n"
		"#     devuelve el número de entradas de la tabla.\n"
		"%%     borra todas las entradas de la tabla.\n"
		"@n    genera n elementos aleatorios y los mezcla en la tabla.\n"
        "s     imprime estadísticas de uso de malloc(3)\n"
		".     termina el programa.\n"
	);
} /* help */

static struct timespec beg_ts;
static int			   beg_res;
void set_timestamp()
{

	beg_res = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &beg_ts);
	if (beg_res < 0) {
		fprintf(stderr, "ERROR: clock_gettime: %s (errno = %d)\n",
			strerror(errno), errno);
	} /* if */
} /* set_timestamp */

void print_timestamp()
{
	struct timespec end_ts;
	int res = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end_ts);
	end_ts.tv_nsec -= beg_ts.tv_nsec;
	end_ts.tv_sec -= beg_ts.tv_sec;
	if (end_ts.tv_nsec < 0) {
		end_ts.tv_nsec += 1000000000L;
		end_ts.tv_sec--;
	} /* if */
	printf("CPU time elapsed: %lu.%09lu\n",
		end_ts.tv_sec, end_ts.tv_nsec);
} /* print_timestamp */

/* main program */
int main (int argc, char **argv)
{
	char buffer[1024];
	AVL_TREE t = new_stravl_tree(strcasecmp);
	long NN = 0;
	int opt;

	{ char *p;
		if ((p = getenv("PAGER")) != 0) {
			pager = p;
		}
	}

	signal(SIGPIPE, SIG_IGN);

	while((opt = getopt(argc, argv, "ph")) != EOF) {
		switch (opt) {
			case 'p': flags |= FLAG_USEPAGER; break;
			case 'h': default:
					  help(); exit(EXIT_SUCCESS); break; 
		} /* switch */
	} /* while */

	help();
	while (fgets(buffer, sizeof buffer, stdin)) {
		char *p = strtok(buffer, "\n");
		AVL_ITERATOR i;

		if (!p) {
			FILE *o = flags & FLAG_USEPAGER
				? popen(pager, "w")
				: stdout;
            set_timestamp();
			stravl_tree_print(t, o);
            print_timestamp();
			if (flags & FLAG_USEPAGER) pclose(o);
			continue;
		} /* if */

		switch (*p) {
			AVL_MT op;
		case '-': p++;
            set_timestamp();
			if (!stravl_tree_del(t, p))
				printf("Error: no puedo borrar [%s]\n", p);
            print_timestamp();
			continue;
#if HAS_MALLOC_STATS
        case 's': malloc_stats();
                  continue;
#endif
		case '?': p++;
            set_timestamp();
			printf("%s: %s\n", p,
				stravl_tree_has(t, p)
					? "TRUE"
					: "FALSE");
            print_timestamp();
			continue;
		case '+':
            p++;
            set_timestamp();
            stravl_tree_put(t, p, (void *)++NN);
            print_timestamp();
            continue;
		default: help(); continue;
		case '*': { FILE *o = flags & FLAG_USEPAGER
					  ? popen(pager, "w")
					  : stdout ;
                set_timestamp();
				for (i = stravl_tree_first(t); i; i = stravl_iterator_next(i)) {
					time_t t = (time_t) stravl_iterator_data(i);
					fprintf(o, "%-32s: [%8ld]\n",
						stravl_iterator_key(i),
						(long) stravl_iterator_data(i));
				} /* for */
                print_timestamp();
				if (flags & FLAG_USEPAGER) pclose(o);
			}
			continue;
		case '/': { FILE *o = flags & FLAG_USEPAGER
					  ? popen("less", "w")
					  : stdout ;
                set_timestamp();
				for (i = stravl_tree_last(t); i; i = stravl_iterator_prev(i)) {
					time_t t = (time_t) stravl_iterator_data(i);
					fprintf(o, "%-32s: [%8ld]\n",
						stravl_iterator_key(i),
						(long) stravl_iterator_data(i));
				} /* for */
                print_timestamp();
				if (flags & FLAG_USEPAGER) pclose(o);
			}
			continue;
		case '#':
            set_timestamp();
			printf("#: %d\n", stravl_tree_size(t));
            print_timestamp();
			continue;
		case '=': p++;
			op = MT_EQ;
			goto common;
		case '<': p++;
			if (*p == '=') {
				p++; op = MT_LE;
			} else {
				op = MT_L;
			} /* if */
			goto common;
		case '>': p++;
			if (*p == '=') {
				p++; op = MT_GE;
			} else {
				op = MT_G;
			} /* if */
common:
			set_timestamp();
			i = stravl_tree_atkey(t, p, op);
			if (i) printf("Busco \"%s\" -> [%s]\n", buffer, stravl_iterator_key(i));
			print_timestamp();
			continue;
		case '%':
			set_timestamp();
			stravl_tree_clear(t);
			print_timestamp();
			NN = 0;
			continue;
		case '@':
			{	int i, N = 10;
				sscanf(buffer+1, "%d", &N);
				set_timestamp();
				for (i = 0; i < N; i++) {
					int j, n;
					n = rand() % 8 + 6;
					for (j = 0; j < n; j++) {
						buffer[j] = (rand() % 26) + 'a';
					}
					buffer[j] = '\0';
					stravl_tree_put(t, buffer, (void *) ++NN);
				} /* for */
				print_timestamp();
			} /* block */
			break;
		case '!':
			set_timestamp();
			printf("Empty: %s\n", stravl_tree_empty(t) ? "TRUE" : "FALSE");
			print_timestamp();
			continue;
		case '.': goto exit;
		} /* switch */
		/*NOTREACHED*/
	} /* while */

exit:
	printf(
		"fin del programa\n"
	);

	return 0;
} /* main */

/* $Id: tstavl.c,v 1.8 2014/01/22 08:01:21 luis Exp $ */
/* vim: ts=4 sw=4 nu
 */
