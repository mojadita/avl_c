/* $Id: stravl.c,v 1.1 2014/01/07 20:03:15 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Tue Jan  7 20:21:19 CET 2014
 * Disclaimer: (C) 2014 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stravl.h"

AVL_TREE new_stravl_tree(STRAVL_FCOMP fc)
{
	if (!fc) fc = strcmp;
	return new_avl_tree(
			(AVL_FCOMP)fc,
			(AVL_FCONS)strdup,
			(AVL_FDEST)free,
			(AVL_FPRNT)fputs);
} /* new stravl_tree */

AVL_ITERATOR stravl_tree_put(
		AVL_TREE t, /* the tree descriptor */
		const char *k, /* the key string */
		void *d) /* the data */
{
	return avl_tree_put(t, (void *)k, d);
} /* stravl_tree_put */

int stravl_tree_del(AVL_TREE t, const char *k)
{
	return avl_tree_del(t, k);
} /* stravl_tree_del */

int stravl_tree_has(AVL_TREE t, const char *k)
{
	return avl_tree_has(t, (void *)k);
}

const char *stravl_iterator_key(
		AVL_ITERATOR i)
{
	return (const char *)avl_iterator_key(i);
} /* stravl_iterator_key */

/* $Id: stravl.c,v 1.1 2014/01/07 20:03:15 luis Exp $ */
