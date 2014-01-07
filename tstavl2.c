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
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "intavl.h"

#define N 10000

/* variables */
static char TSTAVL_CPP_RCSId[]="\n$Id: tstavl2.c,v 1.7 2014/01/07 23:46:03 luis Exp $\n";

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

	srand(time(NULL));
	help();

	for (i = 0; i < N; i++) {
		intavl_tree_put(t, rand(), (void *)i);
	}

#if 0
	for (p = intavl_tree_first(t); p; p = intavl_iterator_next(p)) {
		printf("[%d]->%d\n", intavl_iterator_key(p), intavl_iterator_data(p));
	}
#endif
	intavl_tree_print(t, stdout);

	printf("size = %d\n", avl_tree_size(t));

	return 0;
} /* main */

/* $Id: tstavl2.c,v 1.7 2014/01/07 23:46:03 luis Exp $ */
/* vim: ts=4 sw=4 nu
 */
