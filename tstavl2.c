/* $Id: tstavl2.c,v 1.7 2014/01/07 23:46:03 luis Exp $
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
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include "intavl.h"

#define N 1000000

/* variables */
static char TSTAVL_CPP_RCSId[]="\n$Id: tstavl2.c,v 1.7 2014/01/07 23:46:03 luis Exp $\n";

#define PRINT_TREE  (1 << 0)
int flags = 0;
int n = N;

void help()
{
	printf(
		"tstavl: prueba 2 de las funciones de test de árboles AVL "
		"(treemaps)\n"
		"(c) 2012 LUIS COLORADO SISTEMAS S.L.U.\n"
		"All rights reseved.\n"
	);
} /* help */

/* main program */
int main (int argc, char **argv)
{
	AVL_TREE t = new_intavl_tree();
	int i;
	AVL_ITERATOR p;
	struct timespec t0, t1;
    int opt;
    int seed;

    clock_gettime(CLOCK_REALTIME, &t0);
    seed = t0.tv_nsec;

    while ((opt = getopt(argc, argv, "pn:r:")) != EOF) {
        switch (opt) {
        case 'p': flags |= PRINT_TREE; break;
        case 'n': n = atol(optarg);
                  if (n == 0) n = N;
                  break;
        case 'r': seed = atol(optarg);
                  printf("seed = %d\n", seed);
                  break;
        } /* switch */
    } /* while */

	srand(seed);
	help();

    clock_gettime(CLOCK_REALTIME, &t0);
	for (i = 0; i < n; i++) {
		int d = rand();
#if 0
		intavl_tree_print(t, stdout);
		printf("adding %d\n", d);
#endif
		intavl_tree_put(t, d, (void *)i);
		if (avl_tree_chk(t)) break;
	}
    clock_gettime(CLOCK_REALTIME, &t1);

    if (flags & PRINT_TREE)
        intavl_tree_print(t, stdout);
#if 0
	for (p = intavl_tree_first(t); p; p = intavl_iterator_next(p)) {
		printf("[%d]->%d\n", intavl_iterator_key(p), intavl_iterator_data(p));
	}
#endif
	t1.tv_nsec -= t0.tv_nsec;
	if (t1.tv_nsec < 0) {
		t1.tv_nsec += 1000000000;
		t1.tv_sec--;
	}
	t1.tv_sec -= t0.tv_sec;
    printf("           n: %d\n", n);
    printf("        seed: %d\n", seed);
	printf("elapsed time: %ld.%09ld\n", t1.tv_sec, t1.tv_nsec);
    clock_getres(CLOCK_REALTIME, &t0);
    printf("   clock res: %ld.%09ld\n", t0.tv_sec, t0.tv_nsec);
    printf("  collisions: %d\n", n - avl_tree_size(t));

	return 0;
} /* main */

/* $Id: tstavl2.c,v 1.7 2014/01/07 23:46:03 luis Exp $ */
/* vim: ts=4 sw=4 nu
 */
