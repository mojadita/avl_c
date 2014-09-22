/* $Id: intavl.c,v 1.1 2014/01/07 20:03:15 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Tue Jan  7 20:21:19 CET 2014
 * Disclaimer: (C) 2014 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <stdio.h>
#include "intavl.h"

static int intcmp(const int i1, const int i2)
{
	if (i1 < i2) return -1;
	if (i1 > i2) return +1;
	return 0;
} /* intcmp */

static void *intdup(void *i)
{
	return i; /* retornamos el propio valor */
} /* intdup */

static void intfree(void *i)
{
	/* nada */
} /* intfree */

static int intprnt(const int i, FILE *o)
{
	return fprintf(o, "%d", i);
} /* intprnt */

AVL_TREE new_intavl_tree()
{
	return new_avl_tree(
			(AVL_FCOMP)intcmp,
			(AVL_FCONS)intdup,
			(AVL_FDEST)intfree,
			(AVL_FPRNT)intprnt);
} /* new intavl_tree */

AVL_ITERATOR intavl_tree_put(
		AVL_TREE t, /* the tree descriptor */
		const int k, /* the key string */
		void *d) /* the data */
{
	return avl_tree_put(t, (void *)k, d);
} /* intavl_tree_put */

int intavl_tree_del(AVL_TREE t, const int k)
{
	return avl_tree_del(t, (void *)k);
} /* intavl_tree_del */

int intavl_tree_has(AVL_TREE t, const int k)
{
	return avl_tree_has(t, (void *)k);
}

int intavl_iterator_key(AVL_ITERATOR i)
{
	return (int) avl_iterator_key(i);
} /* intavl_iterator_key */

/* $Id: intavl.c,v 1.1 2014/01/07 20:03:15 luis Exp $ */
