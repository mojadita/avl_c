/* $Id: tstavl2.c,v 1.2 2013/11/28 23:48:20 luis Exp $
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
#include "avl.h"

#define N 100000

/* variables */
static char TSTAVL_CPP_RCSId[]="\n$Id: tstavl2.c,v 1.2 2013/11/28 23:48:20 luis Exp $\n";

void help()
{
	printf(
		"tstavl: prueba 2 de las funciones de test de árboles AVL "
		"(treemaps)\n"
		"(c) 2012 LUIS COLORADO SISTEMAS S.L.U.\n"
		"All rights reseved.\n"
	);
} /* help */

fcmp(int i, int j)
{
	return i < j
		? -1
		: i > j
			? 1
			: 0;
}

int fconst(int i)
{
	return i;
}

void fdest(int i)
{} /* nula a propósito */

/* main program */
int main (int argc, char **argv)
{
	AVL_TREE t = new_avl_tree((AVL_FCOMP)strcmp, strdup, free);
	int i;
	AVL_ITERATOR p;

	srand(time(NULL));
	help();

	for (i = 0; i < N; i++) {
		char buffer[32];
		snprintf(buffer, sizeof buffer, "%09d", rand());
		avl_tree_put(t, buffer, (void *)i);
	}

	for (p = avl_tree_first(t); p; p = avl_iterator_next(p)) {
		printf("[%d]->%d\n", avl_iterator_key(p), avl_iterator_data(p));
	}
	avl_tree_print(t, stdout);

	return 0;
} /* main */

/* $Id: tstavl2.c,v 1.2 2013/11/28 23:48:20 luis Exp $ */
/* vim: ts=4 sw=4 nu
 */
