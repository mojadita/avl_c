/* $Id: tstavl.c,v 1.6 2014/01/07 23:46:03 luis Exp $
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
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "stravl.h"

/* variables */
static char TSTAVL_CPP_RCSId[]="\n$Id: tstavl.c,v 1.6 2014/01/07 23:46:03 luis Exp $\n";

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
		".     termina el programa.\n"
	);
} /* help */

int printkey(FILE *o, char *s)
{
	return fprintf(o, "%s", s);
} /* printkey */

/* main program */
int main (int argc, char **argv)
{
	char buffer[1024];
	AVL_TREE t = new_stravl_tree(strcmp);
	int NN = 0;

	help();
	while (fgets(buffer, sizeof buffer, stdin)) {
		char *p = strtok(buffer, "\n");
		AVL_ITERATOR i;

		if (!p) {
			stravl_tree_print(t, stdout);
			continue;
		} /* if */

		switch (*p) {
			AVL_MT op;
		case '-': p++;
			if (!stravl_tree_del(t, p))
				printf("Error: no puedo borrar [%s]\n", p);
			continue;
		case '?': p++;
			printf("%s: %s\n", p,
				stravl_tree_has(t, p)
					? "TRUE"
					: "FALSE");
			continue;
		case '+': p++; stravl_tree_put(t, p, (void *)++NN); continue;
		default: help(); continue;
		case '*':
			for (i = stravl_tree_first(t); i; i = stravl_iterator_next(i)) {
				time_t t = (time_t) stravl_iterator_data(i);
				printf("%-32s: [%8d]\n",
					stravl_iterator_key(i),
					(int) stravl_iterator_data(i));
			} /* for */
			continue;
		case '/':
			for (i = stravl_tree_last(t); i; i = stravl_iterator_prev(i)) {
				time_t t = (time_t) stravl_iterator_data(i);
				printf("%-32s: [%8d]\n",
					stravl_iterator_key(i),
					(int) stravl_iterator_data(i));
			} /* for */
			continue;
		case '#':
			printf("#: %d\n", stravl_tree_size(t));
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
			i = stravl_tree_atkey(t, p, op);
			if (i) printf("Busco \"%s\" -> [%s]\n", buffer, stravl_iterator_key(i));
			continue;
		case '%':
			stravl_tree_clear(t);
			NN = 0;
			continue;
		case '@':
			{	int i, N = 10;
				sscanf(buffer+1, "%d", &N);
				for (i = 0; i < N; i++) {
					int j, n;
					n = rand() % 8 + 6;
					for (j = 0; j < n; j++) {
						buffer[j] = (rand() % 26) + 'a';
					}
					buffer[j] = '\0';
					stravl_tree_put(t, buffer, (void *) ++NN);
				} /* for */
			} /* block */
			break;
		case '!':
			printf("Empty: %s\n", stravl_tree_empty(t) ? "TRUE" : "FALSE");
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

/* $Id: tstavl.c,v 1.6 2014/01/07 23:46:03 luis Exp $ */
/* vim: ts=4 sw=4 nu
 */
