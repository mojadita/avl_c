/* tstavl3.c -- program to print the sizeof struct avl_node
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Tue Oct 14 13:42:30 EEST 2014
 * Copyright: (C) 2009-2014 LUIS COLORADO SISTEMAS S.L.U.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "avlP.h"

#define P(X) printf(PR("%s = %zu\n"), #X, X)

int main()
{
	P(sizeof(struct avl_node));
	P(sizeof(struct avl_tree));
	return EXIT_SUCCESS;
} /* main */

/* tstavl3.c */
